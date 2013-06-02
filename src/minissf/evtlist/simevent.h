// one header for all priority queues as eventlist (binary heap, splay tree, ladder queue)

#ifndef __MINISSF_SIMEVENT_H__
#define __MINISSF_SIMEVENT_H__

#include "kernel/throwable.h"

namespace minissf {

template<typename T> class SimEvent;
template<typename T> class Eventlist;

// here's an abstract class for eventlist that includes all common
// operations for handling events
template<typename T>
class Eventlist {
 public:
  // the destructor; do nothing here, but needed by the compiler
  virtual ~Eventlist() {}

  // return the total number of events currently stored in the data structure
  virtual int size() const = 0;

  // return true if there's no more event left
  virtual bool empty() const { return bool(size() == 0); }

  // return the event with the samllest timestamp; note that the event
  // will NOT be removed from the data structure; if that's what you
  // need, you should use deleteMin() instead; if the eventlist is
  // empty, null will be returned (it's too simple a case here to
  // throw an exception)
  virtual SimEvent<T>* getMin() const = 0;

  // remove the event with the samllest timestamp and return it; if
  // the eventlist is empty, null will be returned (again, it's too
  // simple a case here to throw an exception)
  virtual SimEvent<T>* deleteMin() = 0;

  // insert an event into the eventlist
  virtual void insert(SimEvent<T>* evt) = 0;

  // remove and reclaim an event from the eventlist; an exception will
  // be raised if the event is not on the eventlist
  virtual void cancel(SimEvent<T>* evt) = 0;

  // adjust the event in the event list due to event timestamp change;
  // by default, we throw an exception
  virtual void adjust(SimEvent<T>* evt) { SSF_THROW("unable to adjust event in the eventlist"); }

  // remove all events from the data structure and reclaim them!
  virtual void clear() = 0;
}; /*class EventList*/

// here's an abstract class for simulation events
template<typename T>
class SimEvent {
  template<typename V> friend class EventList;

 public:
  // the constructor
  SimEvent(T ts) : timestamp(ts), eventlist(0) {}

  // the destructor
  virtual ~SimEvent() {
    if(eventlist) SSF_THROW("attempting to delete an already scheduled event");
  }

  // getter and setter of the event's timestamp
  T time() const { return timestamp; }
  void setTime(T ts) {
    timestamp = ts; 
    if(eventlist) { eventlist->adjust(this); }
  }

  // operators for comparing the timestamps of events
  friend int operator > (const SimEvent& e1, const SimEvent& e2) { return e1.time() > e2.time(); }
  friend int operator < (const SimEvent& e1, const SimEvent& e2) { return e1.time() < e2.time(); }
  friend int operator >= (const SimEvent& e1, const SimEvent& e2) { return e1.time() >= e2.time(); }
  friend int operator <= (const SimEvent& e1, const SimEvent& e2) { return e1.time() <= e2.time(); }

  // we don't use the following operators for simply comparing the timestamps
  //friend int operator == (const SimEvent& e1, const SimEvent& e2) { return e1.time() == e2.time(); }
  //friend int operator != (const SimEvent& e1, const SimEvent& e2) { return e1.time() != e2.time(); }

 protected:
  T timestamp; // the timestamp of this event is used to determine ordering
  Eventlist<T>* eventlist; // if the event is scheduled, it points to the event list holding this event
}; /*class SimEvent*/

}; /*namespace minissf*/

#include "evtlist/binheap.h"
#include "evtlist/splaytree.h"
#include "evtlist/ladderq.h"

#endif /*__MINISSF_SIMEVENT_H__*/

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
