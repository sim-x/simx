#include <assert.h>
#include "kernel/stargate.h"
#include "kernel/universe.h"

namespace minissf {

MapOutport::MapOutport(Stargate* sg, MapInport* ip, VirtualTime md) : 
  stargate(sg), inport(ip), min_offset(md) {}

MapOutport::~MapOutport() {
  // we reclaim the inport if we can (i.e., if on the same machine)
  if(inport) delete inport;
}

MapInport::MapInport(inChannel* inch) :
  ic(inch), extra_delay(0), next(0) {}

MapInport::~MapInport() {
  if(next) delete next; // delete the chain
}

// this is the constructor when both timelines are local (on the same machine)
Stargate::Stargate(Timeline* lp1, Timeline* lp2) :
  source_timeline(lp1), target_timeline(lp2), 
  source_timeline_id(lp1->serialno), target_timeline_id(lp2->serialno),
  outportno(0), in_sync(true), min_delay(0), time(0), mailbox(0), mailbox_tail(0)
{ constructor(); }

// this is the constructor from a local timeline to a remote timeline
Stargate::Stargate(Timeline* lp1, int lp2id, int port) :
  source_timeline(lp1), target_timeline(0), 
  source_timeline_id(lp1->serialno), target_timeline_id(lp2id),
  outportno(port), in_sync(true), min_delay(0), time(0), mailbox(0), mailbox_tail(0)
{ constructor(); }

// this is the constructor from a remote timeline to a local timeline
Stargate::Stargate(int lp1id, int port, Timeline* lp2) :
  source_timeline(0), target_timeline(lp2), 
  source_timeline_id(lp1id), target_timeline_id(lp2->serialno),
  outportno(port), in_sync(true), min_delay(0), time(0), mailbox(0), mailbox_tail(0)
{ constructor(); }

// do the common work for all the constructors above
void Stargate::constructor()
{
  if(source_timeline) source_timeline->add_outbound_stargate(this);
  if(target_timeline) target_timeline->add_inbound_stargate(this);
  ssf_thread_mutex_init(&mailbox_mutex);
  Universe::register_stargate(this); // so that we can reclaim them in the end
}

Stargate::~Stargate()
{
  // in case there should be left-over events, reclaim them
  while(mailbox) { 
    ChainedEvent* evt = mailbox;
    mailbox = mailbox->get_next_event();
    delete evt;
  }
  mailbox_tail = 0;
}

void Stargate::set_delay(VirtualTime t)
{
  if(t <= 0) SSF_THROW("cross-timeline delay must be greater than zero");

  // we set the minimum delay by choosing the smaller one (min_delay=0
  // means it's infinite)
  if(min_delay == 0 || t < min_delay) min_delay = t;
}

void Stargate::set_time(VirtualTime t, bool called_by_sender)
{
  if(called_by_sender) { // called by the one originating the null message
    assert(source_timeline);

    // if the channel time already goes beyond the simulation end
    // time, there's no need to update this channel any more
    if(time >= Universe::args_endtime) return;

    VirtualTime beforetime = time;
    time = t+min_delay;
    assert(beforetime < time);

    if(!target_timeline) { // if it's going to remote machine, we deliver a null message via mpi
#ifdef HAVE_MPI_H
      source_timeline->record_stats_remote_null_messages();
      ChannelEvent* evt = new ChannelEvent(Timestamp(time,0,0), 0, outportno); // null message
      evt->stargate = this;
      if((Universe::args_debug_mask&Universe::DEBUG_FLAG_LPSCHED) != 0) {
	printf(">> [%d:%d] stargate [%d->%d]: set_time(t=%lg, true): remote null message, time=%lg->%lg\n",
	       source_timeline->universe->args_rank, source_timeline->universe->processor_id,
	       source_timeline_id, target_timeline_id, t.second(), beforetime.second(), time.second());
      }
      source_timeline->universe->transport_message(evt);
#else
      assert(0);
#endif
    } else { // if this is local machine delivery
      if(source_timeline->universe == target_timeline->universe) {
	// if both timelines reside on the same processor: if the
	// target timeline is blocked on lbts to advance and if we
	// can, we change the timeline into runnable
	source_timeline->record_stats_local_null_messages();
	if((Universe::args_debug_mask&Universe::DEBUG_FLAG_LPSCHED) != 0) {
	  printf(">> [%d:%d] stargate [%d->%d]: set_time(t=%lg, true): local null message, time=%lg->%lg\n",
		 source_timeline->universe->args_rank, source_timeline->universe->processor_id,
		 source_timeline_id, target_timeline_id, t.second(), beforetime.second(), time.second());
	}
	if(target_timeline->state == Timeline::STATE_WAITING && 
	   beforetime == target_timeline->lbts) {
	  target_timeline->retrieve_incoming_and_calculate_lowerbound();
	  if(beforetime < target_timeline->lbts)
	    target_timeline->universe->make_timeline_runnable(target_timeline);
	}
      } else {
	// if the source and target timelines reside on different
	// processors: we send a null message to that universe
	source_timeline->record_stats_shmem_null_messages();
	if((Universe::args_debug_mask&Universe::DEBUG_FLAG_LPSCHED) != 0) {
	  printf(">> [%d:%d] stargate [%d->%d]: set_time(t=%lg, true): shmem null message, time=%lg->%lg\n",
		 source_timeline->universe->args_rank, source_timeline->universe->processor_id,
		 source_timeline_id, target_timeline_id, t.second(), beforetime.second(), time.second());
	}
	ChannelEvent* evt = new ChannelEvent(Timestamp(time,0,0), 0, outportno); // null message
	evt->stargate = this;
	ssf_thread_mutex_lock(&target_timeline->universe->mailbox_mutex);
	if(!target_timeline->universe->mailbox) 
	  ssf_thread_cond_signal(&target_timeline->universe->mailbox_cond);
	evt->append_to_list(&target_timeline->universe->mailbox, 
			    &target_timeline->universe->mailbox_tail);
	ssf_thread_mutex_unlock(&target_timeline->universe->mailbox_mutex);
      }
    }
  } else { // called by the processor receiving the null message
    if(in_sync) {
      // in a rather remote situation, handle_io_events() would pick
      // up a null message from the last synchronization period; the
      // channel was asynchrous, but then due to training, it is set
      // to be synchronous now; in this case, we can simply ignore
      // such a null event
      return;
    }
    if(!source_timeline) { // only when it's from remote machine
      if(time < t) time = t; // we don't need to add min_delay here (already done so at the source)
      else return; // it's possible if the message arrives out of order
    }
    assert(target_timeline);
    if((Universe::args_debug_mask&Universe::DEBUG_FLAG_LPSCHED) != 0) {
      printf(">> [%d:%d] stargate [%d->%d]: set_time(t=%lg, false): handle null message, time=%lg\n",
	     target_timeline->universe->args_rank, target_timeline->universe->processor_id,
	     source_timeline_id, target_timeline_id, t.second(), time.second());
    }
    //printf("##### state=%d: time=%lg, lbts=%lg\n", target_timeline->state,
    //	   time.second(), target_timeline->lbts.second());
    VirtualTime beforetime = target_timeline->lbts;
    if(target_timeline->state == Timeline::STATE_WAITING && time > beforetime) {
      target_timeline->retrieve_incoming_and_calculate_lowerbound();
      if(beforetime < target_timeline->lbts)
	target_timeline->universe->make_timeline_runnable(target_timeline);
    }
  }
}

void Stargate::send_message(ChannelEvent* evt)
{
  // this function is called when the target timeline is either in a
  // different universe on the same machine, or on a different machine
  // from the source timeline

  // this is to syphon off the events that traverse the synchronous stargate!
  if(in_sync && source_timeline) {
    assert(evt);
    if(target_timeline) { // on the same machine
      assert(source_timeline->universe->local_binque);
      evt->stargate = this;
      source_timeline->universe->local_binque->insert_event(evt);
      //printf("insert local bin: %lg\n", ((VirtualTime)evt->time()).second());
    } else { // on the remote machine
      assert(source_timeline->universe->global_binque);
      evt->stargate = this;
      source_timeline->universe->global_binque->insert_event(evt);
      //printf("insert global bin: %lg\n", ((VirtualTime)evt->time()).second());
    }
    return;
  }

  if(!target_timeline) { // remote machine delivery via message passing
    assert(source_timeline && !evt->inport);
#ifdef HAVE_MPI_H
    source_timeline->record_stats_remote_messages();
    if((Universe::args_debug_mask&Universe::DEBUG_FLAG_LPSCHED) != 0) {
      printf(">> [%d:%d] stargate [%d->%d]: send_message(event @ %lg): remote message\n",
	     source_timeline->universe->args_rank, source_timeline->universe->processor_id,
	     source_timeline_id, target_timeline_id, ((VirtualTime)evt->time()).second());
    }
    evt->stargate = this;
    source_timeline->universe->transport_message(evt);
#else
    assert(0);
#endif
  } else { // local machine delivery (between different processors) via shared memory
    assert(!source_timeline || // this is possible when called by the reader thread
	   source_timeline->universe != target_timeline->universe);
    assert(evt->inport);
    if(source_timeline) source_timeline->record_stats_shmem_messages();
    if((Universe::args_debug_mask&Universe::DEBUG_FLAG_LPSCHED) != 0) {
      if(source_timeline) {
	printf(">> [%d:%d] stargate [%d->%d]: send_message(event @ %lg): shmem message\n",
	       source_timeline->universe->args_rank, source_timeline->universe->processor_id,
	       source_timeline_id, target_timeline_id, ((VirtualTime)evt->time()).second());
      } else {
	printf(">> [%d:*] stargate [%d->%d]: send_message(event @ %lg): receiving message\n",
	       Universe::args_rank, source_timeline_id, target_timeline_id,
	       ((VirtualTime)evt->time()).second());
      }
    }
    if(evt->time() < time) {
      printf("ERROR: straggler event @ time=%lg going through stargate [%d->%d(sync=%d,delay=%lg)] at time=%lg\n",
	     ((VirtualTime)evt->time()).second(), source_timeline_id, 
	     target_timeline_id, in_sync, min_delay.second(), time.second());
      abort();
    }
    ssf_thread_mutex_lock(&mailbox_mutex);
    evt->append_to_list(&mailbox, &mailbox_tail);
    ssf_thread_mutex_unlock(&mailbox_mutex);
  }
}

void Stargate::receive_messages()
{
  assert(target_timeline);
  if(!source_timeline || 
     source_timeline->universe != target_timeline->universe) {
    // stargate's mailbox is used only when it's connected from
    // another machine or from another processor on the same machine
    ssf_thread_mutex_lock(&mailbox_mutex);
    ChannelEvent* evt = (ChannelEvent*)mailbox; 
    mailbox = mailbox_tail = 0;
    ssf_thread_mutex_unlock(&mailbox_mutex);
    if((Universe::args_debug_mask&Universe::DEBUG_FLAG_LPSCHED) != 0 && evt) {
      printf(">> [%d:%d] stargate [%d->%d]: receive_message batch events:\n",
	     target_timeline->universe->args_rank, target_timeline->universe->processor_id,
	     source_timeline_id, target_timeline_id);
    }
    while(evt) {
      if((Universe::args_debug_mask&Universe::DEBUG_FLAG_LPSCHED) != 0) {
	printf(">> [%d:%d]   event at %lg\n", target_timeline->universe->args_rank, 
	       target_timeline->universe->processor_id, ((VirtualTime)evt->time()).second());
      }
      ChannelEvent* nxt = (ChannelEvent*)evt->get_next_event();
      target_timeline->insert_event(evt);
      evt = nxt;
    }
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
