#include <assert.h>
#include "ssfapi/process.h"
#include "kernel/kernel_event.h"
#include "kernel/timeline.h"
#include "kernel/universe.h"

namespace minissf {

Process::Process(Entity* owner) :
  entity_owner(owner), start_function(0), start_container(0) 
{ common_constructor(); }

#if 0 /* disabled after version 1.1 */
Process::Process(Entity* owner, ProcedureFunction function) :
  entity_owner(owner), start_function(function), start_container(owner)
{ 
  if(!function) SSF_THROW("invalid starting procedure");
  common_constructor(); 
}

Process::Process(Entity* owner, ProcedureFunction function, ProcedureContainer* container) :
  entity_owner(owner), start_function(function), start_container(container)
{
  if(!function || !container) SSF_THROW("invalid starting procedure");
  common_constructor();
}
#endif

void Process::common_constructor()
{
  if(!entity_owner) SSF_THROW("null entity owner");

  // if both container and function are null, it means the start
  // procedure is the action() of this class; if both container and
  // function are not null, it means we explicitly identify the
  // starting procedure from a procedure container method

  process_state = PROCESS_STATE_CREATING;
  process_stack = 0;
  hold_event = 0;
  waiton_node = 0;
  active_inchannel = 0;
  active_event_retrieved = false;
  process_timedout = false;
  static_sensitivity = false;
  procedure_context = 0;
  
  entity_owner->add_process(this); // will schedule init() to be called

  ProcessEvent* pevt = new ProcessEvent(this);
  entity_owner->insert_event(pevt);
}

Process::~Process()
{
  if(process_state == PROCESS_STATE_RUNNING)
    SSF_THROW("deleting a running process");

  clear_pending_wait();
  desensitize_static_inchannels();
  while(process_stack) delete pop_stack();
  entity_owner->delete_process(this);
}

VirtualTime Process::now() { return entity_owner->now(); }

void Process::waitOn(const SET(inChannel*)& icset)
{
  PROPER_PROCEDURE(this);
  if(!icset.empty()) {
    for(SET(inChannel*)::iterator iter = icset.begin();
	iter != icset.end(); iter++)
      sensitize_inchannel(*iter);
    suspend_process();
  } else {
    SSF_THROW("empty inchannel set"); // maybe we shouldn't abort
    entity_owner->timeline->deactivate_process(PROCESS_STATE_TERMINATING);
  }
}

void Process::waitOn(inChannel* ic)
{
  PROPER_PROCEDURE(this);
  if(ic) {
    sensitize_inchannel(ic);
    suspend_process();
  } else {
    SSF_THROW("null inchannel"); // maybe we shouldn't abort
    entity_owner->timeline->deactivate_process(PROCESS_STATE_TERMINATING);
  }
}

void Process::waitForever()
{ 
  PROPER_PROCEDURE(this);
  entity_owner->timeline->deactivate_process(PROCESS_STATE_TERMINATING);
}

void Process::suspendForever()
{ 
  PROPER_PROCEDURE(this);
  suspend_process();
}

void Process::waitFor(VirtualTime delay)
{
  PROPER_PROCEDURE(this);
  if(delay < 0) SSF_THROW("negative delay: " << delay);
  hold_for(delay);
}

void Process::waitUntil(VirtualTime time)
{
  PROPER_PROCEDURE(this);
  if(time < now()) SSF_THROW("time in the past: " << time);
  hold_until(time);
}

bool Process::waitOnFor(const SET(inChannel*)& icset, VirtualTime delay)
{
  PROPER_PROCEDURE(this);
  if(delay < 0) SSF_THROW("negative delay: " << delay);
  for(SET(inChannel*)::iterator iter = icset.begin();
      iter != icset.end(); iter++)
    sensitize_inchannel(*iter);
  hold_for(delay);
  return true;
}

bool Process::waitOnFor(inChannel* ic, VirtualTime delay)
{
  PROPER_PROCEDURE(this);
  if(delay < 0) SSF_THROW("negative delay: " << delay);
  if(ic) sensitize_inchannel(ic);
  hold_for(delay);
  return true;
}

bool Process::waitOnUntil(const SET(inChannel*)& icset, VirtualTime time)
{
  PROPER_PROCEDURE(this);
  if(time < now()) SSF_THROW("time in the past: " << time);
  for(SET(inChannel*)::iterator iter = icset.begin();
      iter != icset.end(); iter++)
    sensitize_inchannel(*iter);
  hold_until(time);
  return true;
}

bool Process::waitOnUntil(inChannel* ic, VirtualTime time)
{
  PROPER_PROCEDURE(this);
  if(time < now()) SSF_THROW("time in the past: " << time);
  if(ic) sensitize_inchannel(ic);
  hold_until(time);
  return true;
}

void Process::waitsOn(const SET(inChannel*)& icset)
{
  desensitize_static_inchannels(); // clear out the old ones
  for(SET(inChannel*)::iterator iter = icset.begin();
      iter != icset.end(); iter++)
    sensitize_static_inchannel(*iter);
}

void Process::waitsOn(inChannel* ic)
{
  desensitize_static_inchannels(); // clear out the old ones
  if(ic) sensitize_static_inchannel(ic);
}

void Process::waitOn()
{
  PROPER_PROCEDURE(this);
  if(!static_inchannels.empty()) {
    static_sensitivity = true;
    suspend_process();
  } else {
    SSF_THROW("empty static inchannel"); // maybe we shouldn't abort
    entity_owner->timeline->deactivate_process(PROCESS_STATE_TERMINATING);
  }
}

bool Process::waitOnFor(VirtualTime delay)
{
  PROPER_PROCEDURE(this);
  if(delay < 0) SSF_THROW("negative delay: " << delay);
  static_sensitivity = true;
  hold_for(delay);
  return true;
}

bool Process::waitOnUntil(VirtualTime time)
{
  PROPER_PROCEDURE(this);
  if(time < now()) SSF_THROW("time in the past: " << time);
  static_sensitivity = true;
  hold_until(time);
  return true;
}

void Process::push_stack(Procedure* p)
{
  assert(p);
  p->process_owner = this;
  p->parent_procedure = process_stack;
  process_stack = p;
}

Procedure* Process::pop_stack()
{
  assert(process_stack);
  Procedure* p = process_stack;
  process_stack = p->parent_procedure;
  return p;
}

void Process::sensitize_inchannel(inChannel* ic)
{
  assert(ic);
  WaitNode* wnode = new WaitNode;
  wnode->p = this;
  wnode->ic = ic;
  wnode->next_p = waiton_node;
  waiton_node = wnode;
  ic->set_sensitivity(wnode);
}

void Process::desensitize_inchannels()
{
  while(waiton_node) {
    WaitNode* n = waiton_node->next_p;
    assert(waiton_node->p == this);
    waiton_node->ic->set_insensitivity(waiton_node);
    delete waiton_node;
    waiton_node = n;
  }
}

void Process::sensitize_static_inchannel(inChannel* ic)
{
  assert(ic);
  ic->set_static_sensitivity(this);
  static_inchannels.insert(ic);
}

void Process::desensitize_static_inchannels()
{
  for(SET(inChannel*)::iterator iter = static_inchannels.begin();
      iter != static_inchannels.end(); iter++)
    (*iter)->set_static_insensitivity(this);
  static_inchannels.clear();
}

void Process::hold_for(VirtualTime delay)
{
  hold_until(now()+delay);
}

void Process::hold_until(VirtualTime time)
{
  assert(!hold_event);
  hold_event = new HoldEvent(time, this);
  entity_owner->insert_event(hold_event);
  suspend_process();
}

void Process::execute_process()
{
  activate_procedure();
  if(process_state == PROCESS_STATE_TERMINATING)
    terminate_process();
}

void Process::activate_procedure()
{
  // if start_function=0 and start_container=0, that's
  // Process::action(), which is allowable only as a root procedure;
  // otherwise, none of them should be 0

  procedure_context++;
  entity_owner->timeline->record_stats_procedure_calls();
  if(!process_stack) { // if the stack is empty, go for the starting procedure
    if(start_function) (start_container->*start_function)(this);
    else action();
  } else {
    assert(process_stack->process_owner == this);
    if(process_stack->procedure_container)
      (process_stack->procedure_container->*process_stack->procedure_function)(this);
    else action();
  }
  procedure_context--;
}

void Process::suspend_process()
{
  entity_owner->timeline->deactivate_process(PROCESS_STATE_WAITING);
}

void Process::terminate_process()
{
  wrapup();
  delete this;
}

void Process::clear_pending_wait()
{
  if(process_state == PROCESS_STATE_RUNNING ||
     process_state == PROCESS_STATE_WAITING) {
    if(hold_event) { entity_owner->cancel_event(hold_event); hold_event = 0; }
    if(waiton_node) desensitize_inchannels();
    static_sensitivity = false;
  } else assert(!hold_event && !waiton_node && !static_sensitivity);
}

void Process::clear_process_context()
{
  active_event_retrieved = false;
  process_timedout = false;
  if(active_inchannel) {
    active_inchannel->processes_activated--;
    if(!active_inchannel->processes_activated) 
      active_inchannel->clear_pending_event();
    active_inchannel = 0;
  }
}

}; /*namespace minissf*/

/*
 * Copyright (c) 2011-2013 Florida International University.
 *
 * Permission is hereby granted, free of charge, to any individual or
 * institution obtaining a copy of this software and associated
 * documentation files (the "software"), to use, copy, modify, and
 * distribute without restriction.
 *
 * The software is provided "as is", without warranty of any kind,
 * express or implied, including but not limited to the warranties of
 * merchantability, fitness for a particular purpose and
 * noninfringement.  In no event shall Florida International
 * University be liable for any claim, damages or other liability,
 * whether in an action of contract, tort or otherwise, arising from,
 * out of or in connection with the software or the use or other
 * dealings in the software.
 *
 * This software is developed and maintained by
 *
 *   Modeling and Networking Systems Research Group
 *   School of Computing and Information Sciences
 *   Florida International University
 *   Miami, Florida 33199, USA
 *
 * You can find our research at http://www.primessf.net/.
 */
