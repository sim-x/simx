#include <assert.h>
#include <stdlib.h> // for abort
#include "kernel/timeline.h"
#include "ssfapi/entity.h"
#include "ssfapi/process.h"

namespace minissf {

Timeline::Timeline() : 
  TimelineQueueNode(VirtualTime(0)), universe(0), serialno(0), 
  state(STATE_START), emulated(false), emulated_set(false), 
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
  simlist.clear(); emulist.clear(); // remaining events will be reclaimed here
  inbound.clear(); outbound.clear(); // stargates will be reclaimed by universe
  inbound_async.clear(); outbound_async.clear(); // stargates will be reclaimed by universe
}

bool Timeline::is_emulated()
{
  if(!emulated_set) {
    emulated_set = true;
    for(SET(Entity*)::iterator iter = entities.begin();
	!emulated && iter != entities.end(); iter++) 
      emulated = (*iter)->isEmulated();
  }
  return emulated;
}

void Timeline::add_entity(Entity* entity)
{
  assert(entity && !entity->timeline);
  entity->timeline = this;
  entities.insert(entity);
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
  // note that emulation events may end up in simulation event list
  // (if the timeline is not emulated)
  //if(now() > evt->time()) printf("-> %lg %lg\n", now().second(), evt->time().key1/1e9);
  assert(evt);
  if(now() > evt->time()) {
    printf("ERROR: [%d:%d] timeline=%d => now=%lg evt=%lg\n", 
	   Universe::args_rank, universe->processor_id, serialno,
	   now().second(), VirtualTime(evt->time()).second());
    abort();
  }
  //assert(now() <= evt->time());
  if(is_emulated() && evt->is_emulated()) emulist.insert(evt);
  else simlist.insert(evt);
}

void Timeline::cancel_event(KernelEvent* evt)
{
  assert(evt && now() <= evt->time());
  if(is_emulated() && evt->is_emulated()) emulist.cancel(evt);
  else simlist.cancel(evt);
}

VirtualTime Timeline::run()
{
  // process events all the way up to and include lbts
  while(simclock <= lbts) {
    KernelEvent* evt = peek_next_event();

    // pacing time is required only for emulated timelines
    if(is_emulated()) {
      // if no more simulation events until lbts; this timeline shall
      // be paced until real time reachese lbts!  the method
      // Universe::pace_timeline() returns true if the timeline is
      // indeed put on hold, in which case we return the current
      // simulation clock (which is smaller than lbts) so that
      // scheduler knows it is put on hold; otherwise, if
      // Universe::pace_timeline() returns false, we know we have
      // already reached lbts, we return lbts so that the schedule
      // knows the timeline has done processing the event until the
      // event horizon
      if(!evt || evt->emulation_time() > lbts) {
	if(universe->pace_timeline(this, lbts)) return simclock;
	else return simclock=lbts;
      }

      // events (emulated or simulated) must be paced to happen at the
      // exact time; if the timeline is put on hold, we return
      // prematurely (i.e., with simclock smaller than lbts) so that
      // the scheduler knows it's put on hold
      if(universe->pace_timeline(this, evt->emulation_time())) 
	return simclock;
    }

    // remove the event from the event list
    if(!evt || evt->time() > lbts) break;
    pop_next_event(evt);
    simclock = evt->time();

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

  return simclock=lbts;
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
  return simlist.empty() && emulist.empty();
}

KernelEvent* Timeline::peek_next_event()
{
  KernelEvent* simevt = (KernelEvent*)simlist.getMin();
  if(!simevt) return (KernelEvent*)emulist.getMin();
  else {
    KernelEvent* emuevt = (KernelEvent*)emulist.getMin();
    if(!emuevt || simevt->time() < emuevt->time()) return simevt;
    else return emuevt;
  }
}

void Timeline::pop_next_event(KernelEvent* evt)
{
  assert(evt);
  if(evt == (KernelEvent*)simlist.getMin())
    simlist.deleteMin();
  else {
    assert(evt == (KernelEvent*)emulist.getMin());
    emulist.deleteMin();
  }
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
  if(is_emulated()) {
    KernelEvent* simevt = (KernelEvent*)simlist.getMin();
    KernelEvent* emuevt = (KernelEvent*)emulist.getMin();
    if(simevt) {
      if(emuevt) {
	VirtualTime x = simevt->emulation_time();
	VirtualTime y = emuevt->emulation_time();
	if(x < y) return x;
	else return y;
      } else return simevt->emulation_time();
    } else {
      if(emuevt) return emuevt->emulation_time();
      else return VirtualTime::INFINITY;
    }
  } else return VirtualTime::INFINITY;
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
