#include <assert.h>
#include <stdlib.h> // for abort
#include "kernel/timeline.h"
#include "ssfapi/entity.h"
#include "ssfapi/process.h"

namespace minissf {

Timeline::Timeline() : 
  TimelineQueueNode(VirtualTime(0)), universe(0), serialno(0), 
  state(STATE_START), 
  emulated(false), emulated_set(false), emulated_timer_set(false),
  responsiveness(VirtualTime::INFINITY), 
  lbts(0), simclock(0), 
  stats_processed_events(0),
  stats_process_context_switches(0),
  stats_procedure_calls(0),
  stats_remote_messages(0),
  stats_shmem_messages(0),
  stats_local_messages(0),
  stats_remote_null_messages(0),
  stats_shmem_null_messages(0),
  stats_local_null_messages(0),
  stats_lbts_calculations(0),
  stats_subsequent_updates(0)
{
  if(Universe::args_progress_interval > 0)
    insert_event(new TickEvent(Universe::args_progress_interval));
}

Timeline::~Timeline() 
{
  assert(state == STATE_DONE || state == STATE_START);
  while(!entities.empty()) {
    SET(Entity*)::iterator iter = entities.begin();
    assert(iter != entities.end());
    assert((*iter)->timeline == this);
    delete (*iter);
  }
  entities.clear();
  assert(active_processes.empty());
  evtlist.clear(); //simlist.clear(); emulist.clear(); // remaining events will be reclaimed here
  inbound.clear(); outbound.clear(); // stargates will be reclaimed by universe
  inbound_async.clear(); outbound_async.clear(); // stargates will be reclaimed by universe
}

bool Timeline::is_emulated()
{
  if(!emulated_set) {
    emulated_set = true;
    emulated = false; responsiveness = VirtualTime::INFINITY;
    for(SET(Entity*)::iterator iter = entities.begin();
	iter != entities.end(); iter++) {
      if((*iter)->isEmulated()) {
	emulated = true;
	if((*iter)->responsiveness < responsiveness)
	  responsiveness = (*iter)->responsiveness;
      }
    }
  }
  return emulated;
}

void Timeline::add_entity(Entity* entity)
{
  assert(entity && !entity->timeline);
  entity->timeline = this;
  entities.insert(entity);
  emulated_set = false; // make sure its emulate-ability is recounted
}

void Timeline::delete_entity(Entity* entity)
{
  assert(entity && entity->timeline == this);
  entity->timeline = 0;
  entities.erase(entity);
}

void Timeline::merge_timeline(Timeline* timeline)
{
  assert(timeline);
  if(this == timeline) return;
  /*
  for(SET(Entity*)::iterator iter = timeline->entities.begin();
      iter != timeline->entities.end(); iter++) {
    Entity* entity = *iter; 
    timeline->delete_entity(entity);
    add_entity(entity);
  }
  */
  while(!timeline->entities.empty()) {
    SET(Entity*)::iterator iter = timeline->entities.begin();
    Entity* entity = *iter; 
    timeline->delete_entity(entity);
    add_entity(entity);
  }
  Universe::delete_timeline(timeline);
}

int Timeline::settle_serialno(int tmlnid, int startentid)
{
  serialno = tmlnid;
  for(SET(Entity*)::iterator iter = entities.begin();
      iter != entities.end(); iter++) {
    (*iter)->serialno = startentid++;
  }
  return startentid;
}

int Timeline::get_portno_space() const
{
  int id = 0;
  for(SET(Entity*)::iterator iter = entities.begin();
      iter != entities.end(); iter++) {
    id += (*iter)->outchannels.size();
  }
  return id;
}

int Timeline::settle_portno(int id)
{
  for(SET(Entity*)::iterator iter = entities.begin();
      iter != entities.end(); iter++) {
    Entity* ent = *iter;
    for(SET(outChannel*)::iterator citer = ent->outchannels.begin();
	citer != ent->outchannels.end(); citer++)
      (*citer)->portno = id++;
  }
  return id;
}

void Timeline::insert_event(KernelEvent* evt)
{
  assert(evt);
  if(now() > evt->time()) {
    printf("ERROR: [%d:%d] timeline=%d => now=%lg evt=%lg\n", 
	   Universe::args_rank, universe->processor_id, serialno,
	   now().second(), VirtualTime(evt->time()).second());
    abort();
  }
  //assert(now() <= evt->time());
  evtlist.insert(evt);
  /*
  if(evt->is_emulated()) emulist.insert(evt);
  else simlist.insert(evt);
  */
}

void Timeline::cancel_event(KernelEvent* evt)
{
  assert(evt && now() <= evt->time());
  evtlist.cancel(evt);
  /*
  if(evt->is_emulated()) emulist.cancel(evt);
  else simlist.cancel(evt);
  */
}

void Timeline::insert_emulated_event(EmulatedEvent* ee)
{
  universe->insert_emulated_event(this, ee);
}

VirtualTime Timeline::run()
{
  if(is_emulated() && !emulated_timer_set) {
    emulated_timer_set = true;
    if(responsiveness <= Universe::args_endtime &&
       responsiveness <= Universe::args_time_slice)
      insert_event(new EmulatedTimerEvent(responsiveness, responsiveness));
  }

  // process events all the way up to (and include) the event horizon,
  // which is the LBTS limited to be within a time slice
  VirtualTime horizon = simclock + Universe::args_time_slice;
  if(lbts < horizon) horizon = lbts;

  while(simclock <= horizon) {
    KernelEvent* evt = peek_next_event();
    if(!evt || evt->time() > horizon) break;

    // pacing time is required only for an emulated timeline, and only
    // for an emulated event, and only when the simclock is not yet at
    // event horizon (to avoid the case that emulated event is right
    // at event horizon); pacing is needed so that the emulation event
    // can happen at the expected real time; if the timeline is put on
    // hold for that (pacing_timeline() returns true in this case), we
    // return prematurely (i.e., with the current simclock, which must
    // be smaller than event horizon), so that the scheduler knows it
    // is put on the pacing timeline list; otherwise, if the timeline
    // is not emulated, or the event is not emulated, or the current
    // time is already at event horizon, or the real time has already
    // passed over the time of the emulated event, we shall go ahead
    // processing the event normally (the control will fall through
    // the following statement)
    if(is_emulated() && evt->is_emulated() && simclock < horizon &&
       universe->pace_in_timeline(this, evt->time()))
      return simclock;

    // remove the event from the event list and update simulation clock
    pop_next_event(evt);
    simclock = evt->time();

    /*
    printf("event real=%lld now=%lld (%s)\n", 
	   universe->get_wallclock_time().get_ticks(),
	   simclock.get_ticks(), 
	   evt->is_emulated()?"yes":"no");
    */
    
    // process the event and reclaim it
    record_stats_processed_events();
    evt->process_event(this);
    delete evt;

    // execute all active processes
    Process* p = get_active_process(); 
    Process* q = 0;
    while(p) {
      p->execute_process();
      if(p != q) {
	record_stats_process_context_switches();
        q = p;
      }
      p = get_active_process();
    }
  }

  if(horizon < lbts) universe->make_timeline_runnable(this);
  //ssf_thread_yield();
  return simclock=horizon;
}

void Timeline::activate_process(Process* p)
{
  // we should ignore processes marked as running since they have
  // already been put onto the ready list
  assert(p);
  if(p->process_state != Process::PROCESS_STATE_RUNNING) {
    p->process_state = Process::PROCESS_STATE_RUNNING;
    active_processes.push_back(p);
  }
}

Process* Timeline::get_active_process()
{
  if(active_processes.empty()) return 0;
  else {
    Process* p = active_processes.front();
    p->clear_pending_wait(); // should be included when processing the event
    return p;
  }
}

void Timeline::deactivate_process(int newstate)
{
  assert(!active_processes.empty());

  // clean up the state of the process
  Process* p = active_processes.front();
  p->process_state = newstate;
  p->clear_process_context();

  // remove the process from the ready queue
  active_processes.pop_front();
}

bool Timeline::no_more_events()
{
  return evtlist.empty();
  //return simlist.empty() && emulist.empty();
}

KernelEvent* Timeline::peek_next_event()
{
  return (KernelEvent*)evtlist.getMin();
  /*
  KernelEvent* simevt = (KernelEvent*)simlist.getMin();
  if(!simevt) return (KernelEvent*)emulist.getMin();
  else {
    KernelEvent* emuevt = (KernelEvent*)emulist.getMin();
    if(!emuevt || simevt->time() < emuevt->time()) return simevt;
    else return emuevt;
  }
  */
}

void Timeline::pop_next_event(KernelEvent* evt)
{
  assert(evt == (KernelEvent*)evtlist.getMin());
  evtlist.deleteMin();
  /*
  if(evt->is_emulated()) emulist.deleteMin();
  else simlist.deleteMin();
  */
  /*
  if(evt == (KernelEvent*)simlist.getMin())
    simlist.deleteMin();
  else {
    assert(evt == (KernelEvent*)emulist.getMin());
    emulist.deleteMin();
  }
  */
}

void Timeline::add_inbound_stargate(Stargate* sg)
{
  assert(sg);
  inbound.push_back(sg);
}

void Timeline::add_outbound_stargate(Stargate* sg)
{
  assert(sg);
  outbound.push_back(sg);
}

void Timeline::classify_async()
{
  // previously async channels may contain messages to be received
  for(VECTOR(Stargate*)::iterator iter = inbound_async.begin();
      iter != inbound_async.end(); iter++) {
    assert(this == (*iter)->target_timeline);
    (*iter)->receive_messages();
  }

  inbound_async.clear();
  outbound_async.clear();
  for(VECTOR(Stargate*)::iterator in_iter = inbound.begin();
      in_iter != inbound.end(); in_iter++) {
    if(!(*in_iter)->in_sync) {
      inbound_async.push_back(*in_iter);
      //(*in_iter)->time = now+(*in_iter)->min_delay; // update channel time (caused hideous bug!)
    }
  }
  for(VECTOR(Stargate*)::iterator out_iter = outbound.begin();
      out_iter != outbound.end(); out_iter++) {
    if(!(*out_iter)->in_sync) outbound_async.push_back(*out_iter);
  }
}

void Timeline::retrieve_incoming_and_calculate_lowerbound()
{
  record_stats_lbts_calculations();
  // upper bound of lbts should be minimum of next_epoch and next_decade
  lbts = universe->next_epoch;
  if(universe->next_decade < lbts) lbts = universe->next_decade;
  for(VECTOR(Stargate*)::iterator iter = inbound_async.begin();
      iter != inbound_async.end(); iter++) {
    assert(this == (*iter)->target_timeline);
    VirtualTime x = (*iter)->get_time();
    if(x < lbts) lbts = x;
    (*iter)->receive_messages();
  }
}

void Timeline::update_subsequent_timelines()
{
  record_stats_subsequent_updates();
  for(VECTOR(Stargate*)::iterator iter = outbound_async.begin();
      iter != outbound_async.end(); iter++) {
    assert(this == (*iter)->source_timeline);
    (*iter)->set_time(simclock, true);
  }
}

VirtualTime Timeline::next_emulation_due_time()
{
  // this function is used by scheduler to calculate *ready*
  // timelines' priority (the smaller timestamp, the higher the
  // priority to be scheduled); the "right" way to calculate the
  // priority should be to consider the number of simulated events
  // that need to be processed before the emulated event, but we can't
  // get that information easily; here if a timeline has a most
  // pressing event, it needs to be processed first.
  KernelEvent* evt = (KernelEvent*)evtlist.getMin();
  if(evt) return evt->time();
  else return VirtualTime::INFINITY;
  /*
  KernelEvent* emuevt = (KernelEvent*)emulist.getMin();
  if(emuevt) return emuevt->emulation_time();
  else return VirtualTime::INFINITY;
  */
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
