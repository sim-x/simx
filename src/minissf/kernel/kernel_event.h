// contain all simulation kernel event types supporting various
// schedulable functions of ssf

#ifndef __MINISSF_KERNEL_EVENT_H__
#define __MINISSF_KERNEL_EVENT_H__

#include "ssfapi/ssf_common.h"
#include "evtlist/simevent.h"

namespace minissf {

typedef SplayTree<Timestamp> KernelEventList;
typedef SplayTreeNode<Timestamp> KernelEventNode;

// this is the base class for all kernel events
class KernelEvent : public KernelEventNode {
 public:
  KernelEvent(Entity* entity, VirtualTime t);
  KernelEvent(Timestamp t); // used by channel event
  virtual ~KernelEvent() {}
  
  // does this event needs to be pinned down to real time
  virtual bool is_emulated() = 0;

  // emulation time may be smaller than time for simulated event if
  // we consider real-time lookahead (or slackness)
  VirtualTime emulation_time();

  // process the event dispatched by the given timeline
  virtual void process_event(Timeline* timeline) = 0;
}; /*class KernelEvent*/

// this is the event for progress ticking
class TickEvent : public KernelEvent {
 public:
  TickEvent(VirtualTime t);
  virtual ~TickEvent() {}
  virtual bool is_emulated() { return false; }
  virtual void process_event(Timeline* timeline);
}; /*class TickEvent*/

// this is the event for timers
class TimerEvent : public KernelEvent {
 public:
  TimerEvent(VirtualTime timeout, Timer* tmr);
  virtual ~TimerEvent() {}

  // a timer event may be pinned down if the owner entity is emulated
  virtual bool is_emulated();

  // the timer is rescheduled; the event needs to be adjusted in the eventlist
  void setTime(VirtualTime t);

  // process this event
  virtual void process_event(Timeline* timeline);

 protected:
  Timer* timer;
}; /*class TimerEvent*/

// this is the event for process wait statements
class HoldEvent : public KernelEvent {
 public:
  HoldEvent(VirtualTime timeout, Process* p);
  virtual ~HoldEvent() {}

  // a hold event may be pinned down if the owner entity is emulated
  virtual bool is_emulated();

  // process the event
  virtual void process_event(Timeline* timeline);

 protected:
   Process* process;
}; /*class HoldEvent*/

// this is the event generated for each new process
class ProcessEvent : public KernelEvent {
 public:
  ProcessEvent(Process* p);
  virtual ~ProcessEvent() {}

  // a process event may be pinned down if the owner entity is emulated
  virtual bool is_emulated();

  // process the event
  virtual void process_event(Timeline* timeline);

 protected:
   Process* process;
}; /*class ProcessEvent*/

// this is the event to wait up the process for signal()
class SemaphoreEvent : public KernelEvent {
 public:
  SemaphoreEvent(Process* p);
  virtual ~SemaphoreEvent() {}

  // a semaphore event may be pinned down if the owner entity is emulated
  virtual bool is_emulated();

  // process the event
  virtual void process_event(Timeline* timeline);

 protected:
   Process* process;
}; /*class SemaphoreEvent*/

// this is the event that's got to sent across channel
class ChannelEvent : public KernelEvent {
 public:
  // the constructor of a channel event to be sent to the same
  // machine (either on the same or a different processor)
  ChannelEvent(outChannel* oc, VirtualTime arrival, Event* evt, MapInport* inport);

  // the constructor of a channel event to be sent to a remote machine
  ChannelEvent(outChannel* oc, VirtualTime arrival, Event* evt, int outportno);

  // the constructor of a channel event as it "snakes" through the
  // inport chain (see process_event method)
  ChannelEvent(Timestamp t, Event* evt, bool emulated, MapInport* inport);

  // the constructor of a channel event from a remote machine
  ChannelEvent(Timestamp t, Event* evt, bool emulated, int outportno);

  // the destructor
  virtual ~ChannelEvent();

  // if a channel event needs to be pinned down?
  virtual bool is_emulated() { return emulated; }

  // channel events are stored at the mailboxes as linked lists, also
  // in the binque as well; the difference is that in the mailboxes,
  // the events need to keep the FIFO order (so that it won't cause
  // synchronization issue); in the binque it doesn't matter as they
  // are sorted using all the three keys
  inline ChannelEvent*& get_next_event() { return nextevt; }
  inline void append_to_list(ChannelEvent** head, ChannelEvent** tail) { 
    nextevt = 0; // must reset to null
    if(*head) { (*tail) = (*tail)->nextevt = this; }
    else { (*head) = (*tail) = this; }
  }

  // return the ssf event carried by this kernel event
  inline Event* get_event() { return event; }
  inline Event* delete_event() { Event* retevt = event; event = 0; return retevt; }

  // process the event
  virtual void process_event(Timeline* timeline);

#ifdef HAVE_MPI_H
  void pack(MPI_Comm comm, char* buffer, int& pos, int bufsiz);
  static ChannelEvent* unpack(MPI_Comm comm, char* buffer, int& pos, int bufsiz);
#endif

 protected:
  Event* event;
  MapInport* inport;
  Stargate* stargate; // this field is used by writer thread to infer target machine when the event is to be sent to another machine; and also used by the receiving timeline at a different universe to infer the stargate it needs to update the time; also used by switch board to record the stargate for delivery the message 
  bool emulated;
  int outportno;
  ChannelEvent* nextevt;

  friend class Stargate;
  friend class Universe;
}; /*class ChannelEvent*/

}; /*namespace minissf*/

#endif /*__MINISSF_KERNEL_EVENT_H__*/

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
