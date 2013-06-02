#include <assert.h>
#include "kernel/kernel_event.h"
#include "kernel/timeline.h"
#include "ssfapi/ssf_timer.h"
#include "ssfapi/process.h"
#include "ssfapi/entity.h"
#include "ssfapi/event.h"
#include "ssfapi/outchannel.h"

namespace minissf {

// XXX: what to use here? and how to set programmatically?
#define REALTIME_LOOKAHEAD VirtualTime(1.0, VirtualTime::MILLISECOND)

/* base class for all kernel events */

KernelEvent::KernelEvent(Entity* entity, VirtualTime t) :
  KernelEventNode(Timestamp(t, entity->serialno, entity->get_next_event_id())) {}

KernelEvent::KernelEvent(Timestamp t) : KernelEventNode(t) {}

VirtualTime KernelEvent::emulation_time()
{
  if(is_emulated()) return time();
  else return time()-REALTIME_LOOKAHEAD;
}

/* event for ticking progress mark */

TickEvent::TickEvent(VirtualTime t) : 
  KernelEvent(Timestamp(t,0,0)) {}

void TickEvent::process_event(Timeline* timeline)
{
  printf(">> %lg: timeline[%d] tick: %lu events\n", 
	 timeline->simclock.second(), timeline->serialno, 
	 --timeline->stats_processed_events); // this event should not be counted
  VirtualTime t = timeline->simclock+Universe::args_progress_interval;
  if(t < Universe::args_endtime) {
    TickEvent* evt = new TickEvent(t);
    timeline->insert_event(evt);
  }
}

/* timer event for user defined timers */

TimerEvent::TimerEvent(VirtualTime timeout, Timer* tmr) :
  KernelEvent(tmr->entity_owner, timeout), timer(tmr) {}

bool TimerEvent::is_emulated() { return timer->entity_owner->isEmulated(); }

void TimerEvent::setTime(VirtualTime t)
{
  Timestamp ts(t, timer->entity_owner->serialno, 
	       timer->entity_owner->get_next_event_id());
  KernelEvent::setTime(ts);
}

void TimerEvent::process_event(Timeline* timeline)
{
  assert(timer->timer_event == this);
  timer->timer_event = 0;
  timer->callback();
}

/* hold event for wait statements */

HoldEvent::HoldEvent(VirtualTime timeout, Process* p) :
  KernelEvent(p->entity_owner, timeout), process(p) {}

bool HoldEvent::is_emulated() { return process->entity_owner->isEmulated(); }

void HoldEvent::process_event(Timeline* timeline)
{
  assert(process->hold_event == this);
  process->hold_event = 0;
  process->process_timedout = true;

  // if the process is also waiting on a set of static in-channels,
  // sign off; or, if it's waiting on a set of dynamic in-channels,
  // insensitize it
  if(process->static_sensitivity) process->static_sensitivity = false;
  else if(process->waiton_node) process->desensitize_inchannels();

  // activate the process
  timeline->activate_process(process);
}

/* process event for new process */

ProcessEvent::ProcessEvent(Process* p) :
  KernelEvent(p->entity_owner, p->now()), process(p) {}

bool ProcessEvent::is_emulated() { return process->entity_owner->isEmulated(); }

void ProcessEvent::process_event(Timeline* timeline)
{
  process->init();
  timeline->activate_process(process);
}

/* semaphore event for waking up process */

SemaphoreEvent::SemaphoreEvent(Process* p) :
  KernelEvent(p->entity_owner, p->now()), process(p) {}

bool SemaphoreEvent::is_emulated() { return process->entity_owner->isEmulated(); }

void SemaphoreEvent::process_event(Timeline* timeline)
{
  timeline->activate_process(process);
}

/* channel event for communications between entities */

ChannelEvent::ChannelEvent(outChannel* oc, VirtualTime arrival, Event* evt, MapInport* ip) :
  KernelEvent(oc->entity_owner, arrival), event(evt), 
  inport(ip), stargate(0), emulated(oc->entity_owner->isEmulated()), 
  outportno(0), nextevt(0) {}

ChannelEvent::ChannelEvent(outChannel* oc, VirtualTime arrival, Event* evt, int pno) :
  KernelEvent(oc->entity_owner, arrival), event(evt),
  inport(0), stargate(0), emulated(oc->entity_owner->isEmulated()), 
  outportno(pno), nextevt(0) {}

ChannelEvent::ChannelEvent(Timestamp t, Event* evt, bool emu, MapInport* ip) :
  KernelEvent(t), event(evt), inport(ip), stargate(0), emulated(emu),
  outportno(0), nextevt(0) {}

ChannelEvent::ChannelEvent(Timestamp t, Event* evt, bool emu, int pno) :
  KernelEvent(t), event(evt), inport(0), stargate(0), emulated(emu),
  outportno(pno), nextevt(0) {}

ChannelEvent::~ChannelEvent() {
  if(event) delete event;
}

void ChannelEvent::process_event(Timeline* timeline) 
{
  assert(inport);
  Event* evt = delete_event(); assert(evt);
  if(inport->next) {
    Timestamp ts = time();
    ts.key1 += int64(inport->next->extra_delay);
    ChannelEvent* chevt = new ChannelEvent
      (ts, evt->clone(), is_emulated(), inport->next);
    timeline->insert_event(chevt);
  }
  inport->ic->schedule_arrival(evt);
}

#ifdef HAVE_MPI_H
#define MAXEVTSIZ 4096
void ChannelEvent::pack(MPI_Comm comm, char* buffer, int& pos, int bufsiz)
{
  Timestamp ts = time();
  ssf_mpi_pack(&ts.key1, 1, MPI_LONG_LONG_INT, buffer, bufsiz, &pos, comm);
  ssf_mpi_pack(&ts.key2, 1, MPI_UNSIGNED, buffer, bufsiz, &pos, comm);
  ssf_mpi_pack(&ts.key3, 1, MPI_UNSIGNED, buffer, bufsiz, &pos, comm);

  ssf_mpi_pack(&outportno, 1, MPI_UNSIGNED, buffer, bufsiz, &pos, comm);

  int32 emu = emulated ? 1 : 0;
  ssf_mpi_pack(&emu, 1, MPI_INT, buffer, bufsiz, &pos, comm);

  int32 event_ident, data_size;
  char* sbuf = 0;
  if(event) {
    event_ident = event->event_class_ident();
    sbuf = new char[MAXEVTSIZ]; /*(char*)QuickObject::quick_new(MAXEVTSIZ);*/ assert(sbuf);
    data_size = event->pack(sbuf, bufsiz-pos-16); // 16 is for safety
    //printf("packing bufsiz=%d pos=%d ds=%d\n", bufsiz, pos, data_size);
  } else event_ident = data_size = 0;
  ssf_mpi_pack(&event_ident, 1, MPI_INT, buffer, bufsiz, &pos, comm);
  ssf_mpi_pack(&data_size, 1, MPI_INT, buffer, bufsiz, &pos, comm);
  //printf("packing %d bytes\n", data_size);
  if(data_size > 0)
    ssf_mpi_pack(sbuf, data_size, MPI_CHAR, buffer, bufsiz, &pos, comm);
  if(sbuf) delete[] sbuf; /*QuickObject::quick_delete(sbuf);*/
}

ChannelEvent* ChannelEvent::unpack(MPI_Comm comm, char* buffer, int& pos, int bufsiz)
{
  Timestamp ts;
  ssf_mpi_unpack(buffer, bufsiz, &pos, &ts.key1, 1, MPI_LONG_LONG_INT, comm);
  ssf_mpi_unpack(buffer, bufsiz, &pos, &ts.key2, 1, MPI_UNSIGNED, comm);
  ssf_mpi_unpack(buffer, bufsiz, &pos, &ts.key3, 1, MPI_UNSIGNED, comm);

  uint32 outportno;
  ssf_mpi_unpack(buffer, bufsiz, &pos, &outportno, 1, MPI_UNSIGNED, comm);

  int32 emu;
  ssf_mpi_unpack(buffer, bufsiz, &pos, &emu, 1, MPI_INT, comm);

  int32 event_ident;
  int32 data_size;
  char* sbuf = 0;
  ssf_mpi_unpack(buffer, bufsiz, &pos, &event_ident, 1, MPI_INT, comm);
  ssf_mpi_unpack(buffer, bufsiz, &pos, &data_size, 1, MPI_INT, comm);
  if(data_size > 0) {
    //printf("unpacking ds=%d\n", data_size); fflush(0);
    sbuf = new char[data_size]; /*(char*)QuickObject::quick_new(data_size);*/ assert(sbuf);
    ssf_mpi_unpack(buffer, bufsiz, &pos, sbuf, data_size, MPI_CHAR, comm);
  }

  Event* event = 0;
  if(event_ident > 0) {
    event = Event::create_registered_event(event_ident, sbuf, data_size);
    if(!event) SSF_THROW("unable to create event with id=" << event_ident);
  }
  if(sbuf) delete[] sbuf; /*QuickObject::quick_delete(sbuf);*/
  return new ChannelEvent(ts, event, emu, outportno);
}
#endif

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
