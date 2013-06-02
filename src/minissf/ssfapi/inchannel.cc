#include <assert.h>
#include "ssfapi/inchannel.h"
#include "kernel/universe.h"
#include "kernel/timeline.h"

namespace minissf {

inChannel::inChannel(Entity* theowner) :
  entity_owner(theowner), wait_head(0), wait_tail(0),
  active_event(0), processes_activated(0)
{
  if(!Universe::is_initializing()) 
    SSF_THROW("can only be created during simulation initialization");
  if(!entity_owner) SSF_THROW("null entity owner");
  entity_owner->add_inchannel(this);
}

inChannel::inChannel(Entity* theowner, const char* thename) :
  entity_owner(theowner), wait_head(0), wait_tail(0),
  active_event(0), processes_activated(0), name(thename)
{
  if(!Universe::is_initializing()) 
    SSF_THROW("can only be created during simulation initialization");
  if(!entity_owner) SSF_THROW("null entity owner");
  entity_owner->add_inchannel(this);
}

inChannel::~inChannel()
{
  if(!Universe::is_finalizing()) 
    SSF_THROW("can only be deleted during simulation finalization");
 
  assert(!wait_head && !wait_tail);
  assert(static_processes.empty());
  assert(!active_event);
  entity_owner->delete_inchannel(this);
}

Event* inChannel::activeEvent()
{
  Process* p = entity_owner->timeline->current_process();
  PROPER_PROCEDURE(p);

  if(active_event) {
    if(p->active_event_retrieved) {
      SSF_THROW("called previously in the same activation");
      return 0;
    } else {
      p->active_event_retrieved = true;
      //processes_activated--;
      if(processes_activated > 1) return active_event->clone();
      else {
	Event* retevt = active_event;
	active_event = 0;
	return retevt;
      }
    }
  } else return 0;
}

void inChannel::set_sensitivity(WaitNode* wnode)
{
  // put the node into the double-linked list
  wnode->prev_c = 0;
  wnode->next_c = wait_head;
  if(wait_head) {
    wait_head->prev_c = wnode;
    wait_head = wnode;
  } else wait_head = wait_tail = wnode;
}

void inChannel::set_insensitivity(WaitNode* wnode)
{
  // get the node off the double-linked list
  if(wnode->prev_c) wnode->prev_c->next_c = wnode->next_c;
  else wait_head = wnode->next_c;
  if(wnode->next_c) wnode->next_c->prev_c = wnode->prev_c;
  else wait_tail = wnode->prev_c;
}

void inChannel::set_static_sensitivity(Process* proc)
{
  static_processes.insert(proc);
}

void inChannel::set_static_insensitivity(Process* proc)
{
  static_processes.erase(proc);
}

void inChannel::schedule_arrival(Event* evt)
{
  assert(evt);
  assert(!processes_activated);

  // check permanent sensitivity first
  for(SET(Process*)::iterator iter = static_processes.begin();
      iter != static_processes.end(); iter++) {
    Process* p = *iter;
    if(p->static_sensitivity) {
      // if the process is indeed expecting event arrival
      assert(!p->waiton_node);
      processes_activated++;
      p->active_inchannel = this;
      p->active_event_retrieved = false;
      p->process_timedout = false;
      if(p->hold_event) {
	p->entity_owner->cancel_event(p->hold_event);
	p->hold_event = 0;
      }
      entity_owner->timeline->activate_process(p);
    }
  }

  // scan through the temporary sensitive list
  WaitNode* wnode = wait_head;
  while(wnode) {
    assert(wnode->ic == this);
    WaitNode* wnode_next = wnode->next_c; // remember the next one
    Process* p = wnode->p;
    processes_activated++;
    p->active_inchannel = this;
    p->active_event_retrieved = false;
    p->process_timedout = false;
    if(p->hold_event) {
      p->entity_owner->cancel_event(p->hold_event);
      p->hold_event = 0;
    }
    entity_owner->timeline->activate_process(p);
    wnode = wnode_next; // go to the next waiting process
  }

  if(processes_activated) active_event = evt;
  else delete evt;
}

void inChannel::clear_pending_event()
{
  if(active_event) {
    delete active_event;
    active_event = 0;
    processes_activated = 0;
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
