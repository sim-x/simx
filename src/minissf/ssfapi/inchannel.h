/**
 * \file inchannel.h
 * \brief Header file for the inchannel class.
 *
 * This header file contains the definition of the inchannel class;
 * Users do not need to include this header file directly; it is
 * included from ssf.h.
 */

#ifndef __MINISSF_INCHANNEL_H__
#define __MINISSF_INCHANNEL_H__

#include "ssfapi/ssf_common.h"
#include "kernel/kernel_event.h"

namespace minissf {

class WaitNode;

/**
 * \brief Inchannel as portal for receiving events.
 *
 * An inchannel is the receiving end of the communication link between
 * entities. Messages, known as events, are traveling from an
 * outchannel to all inchannels that are mapped to the outchannel.
 * Processes can wait on an inchannel or a list of inchannels for
 * events to arrive. Events that arrive at an inchannel on which no
 * process is waiting are discarded by the system implicitly.
 */
class inChannel {
 public:
  /**
   * \brief The constructor of an unnamed inchannel.
   *
   * The constructor is called by passing an argument that points to
   * an entity as its owner. The ownership is immutable during the
   * lifetime of the inchannel. Using this constructor, the inchannel
   * is unknown externally and therefore cannot be referenced (i.e.,
   * mapped) outside the current address space.
   *
   * \param owner points to the owner entity of this inchannel
   */
  inChannel(Entity* owner);

  /**
   * \brief The constructor of a named inchannel.
   *
   * The constructor has two arguments: the owner entity and the
   * globally unique name of the inchannel. A named inchannel can be
   * referenced (i.e., mapped) from the outside of the address spaces
   * in a distrubuted environment (using the outChannel::mapto()
   * method).
   *
   * \param owner points to the owner entity of this inchannel
   * \param icname the globally unique string name of this inchannel
   */
  inChannel(Entity* owner, const char* icname);

  /**
   * \brief The destructor of the inchannel.
   *
   * Detroying an inchannel means to separate the links between the
   * outchannels mapped to this inchannel. The user shall not reclaim
   * an inchannel directly: all inchannels are reclaimed by the system
   * when the owner entity is destroyed at the end of the simulation.
   */
  virtual ~inChannel();

  /**
   * \brief Return the entity owner of this inchannel.
   *
   * The entity owner is permanently set upon creation of the
   * inchannel. The ownership is immutable.
   */
  inline Entity* owner() { return entity_owner; }

  /**
   * \brief Return the event arrived at the inchannel.
   *
   * This method can only be called within a procedure of a
   * process. It is expected to be called by the process waiting on
   * the inchannel: when the process unblocks and returns from the
   * wait statement, it should use this method to get the arrived
   * event immediately. An event arrived at the inchannel will not be
   * stored; it must be retrieved using this method or it will be
   * discarded implicitly by the system afterwards. Also, a newly
   * arrived event can be retrieved at most once by any waiting
   * process. Once it has been retrieved, when calling this method
   * again, the method will throw an exeception and return null. If
   * multiple processes are waiting on the same inchannel, each
   * waiting process can retrieve a copy of the arrived event. If
   * multiple events arrive at the inchannel simultaneously, each
   * event arrival will be treated separately. That is, if the process
   * waits on the inchannel in a loop, each iteration will handle only
   * one arrival event.
   */
  Event* activeEvent();

 protected:
  friend class Entity;
  friend class Process;
  friend class Event;
  friend class outChannel;
  friend class Timeline;
  friend class ChannelEvent;
  friend class Universe;

  void set_sensitivity(WaitNode* wnode); // make a process sensitive to this inchannel
  void set_insensitivity(WaitNode* wnode); // remove a process from the sensitivity list
  void set_static_sensitivity(Process* p); // make a process statically sensitive to this inchannel
  void set_static_insensitivity(Process* p); // remove the inchannel from the process's static set
  void schedule_arrival(Event* evt); // deliver an event to the inchannel
  void clear_pending_event(); // clean up the arrival event if it's not retrieved

 protected:
  // each inchannel is owned by an entity; the ownership is immutable
  Entity* entity_owner;

  // point to the head and tail of the linked list of processes
  // dynamically sensitive to this inchannel
  WaitNode* wait_head;
  WaitNode* wait_tail;

  // an event arrived at this inchannel is stored here temporarily
  // for user retrievial
  Event* active_event;

  // the number of processes activated by the arrival event
  int processes_activated;

  // if provided, this is the name of the inchannel which shall be
  // globally unique
  STRING name; 

  // keep the set of processes that are declared statically sensitive
  // to this inchannel
  SET(Process*) static_processes;
}; /*class inChannel*/

}; // namespace minissf

#include "ssfapi/entity.h"
#include "ssfapi/process.h"

#endif /*__MINISSF_INCHANNEL_H__*/

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
