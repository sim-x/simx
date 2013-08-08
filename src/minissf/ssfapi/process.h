/**
 * \file process.h
 * \brief Header file for the process class.
 *
 * This header file contains the definition of the process class;
 * Users do not need to include this header file directly; it is
 * included from ssf.h.
 */

#ifndef __MINISSF_PROCESS_H__
#define __MINISSF_PROCESS_H__

#include "ssfapi/ssf_common.h"
#include "kernel/kernel_event.h"
#include "ssfapi/procedure.h"

namespace minissf {

class WaitNode;

/**
 * \brief Encapsulation of a simulation process as a single thread of control.
 *
 * An SSF process is part of an entity and is used to specify the
 * entity's state evolution. One can think of an SSF process similar
 * to a regular Unix process. Each process is a thread of execution
 * interspersed with wait statements: a process can wait for an event
 * to arrive or wait for some specified simulation time to pass. Each
 * Process object encapsulates an SSF process. A process is started by
 * executing a procedure, which could be a method of an entity, or a
 * method of any class derived from the ProcedureContainer class. The
 * users can define their own process classes if per-process private
 * data is needed.
 */
class Process : public ProcedureContainer {
 public:
  /**
   * \brief Creating a process, which starts from the action() method.
   *
   * When this constructor is called, a new simulation process is
   * created, which will become eligible to run at the current
   * simulation time, by invoking the start procedure, which is the
   * action() method. Each process must be owned by an entity and the
   * ownership is immutable. In particular, a process cannot migrate
   * from one entity to another. The process, however, is free to
   * access the state variables of its owner entity and all other
   * entities that are co-aligned with its owner entity, including
   * instances of inchannels and outchannels owned by these entities.
   *
   * \param owner points to the entity owner of this process
   */
  Process(Entity* owner);

#if 0 /* disabled after version 1.1 */
  /**
   * \brief The constructor using an entity method as the starting procedure.
   *
   * This constructor is similar to the previous constructor except
   * that the user needs to provide a pointer to an entity class
   * method as the starting procedure.
   *
   * \param owner points to the entity owner of this process
   * \param function points to an entity method as the starting procedure
   */
  Process(Entity* owner, ProcedureFunction function);

  /**
   * \brief The constructor using a method of a procedure container as the starting procedure.
   *
   * This constructor is similar to the previous constructors except
   * that the user needs to provide a pointer to a method in the
   * ProcedureContainer class or its derived class, as well as a
   * pointer to the class itself. Note that both Entity and Process
   * are derived from the ProcedureContainer class. As such, this
   * contructor is a generic case, and the previous two constructors
   * are simply special cases.
   *
   * \param owner points to the entity owner of this process
   * \param function points to a method as the starting procedure
   * \param container points to a ProcedureContainer instance
   */
  Process(Entity* owner, ProcedureFunction function, ProcedureContainer* container);
#endif

  /**
   * \brief The destructor of the process.
   *
   * Usually the user do not need to delete a process explicitly. A
   * process object will be reclaimed by the system when the process
   * terminates. The user shall not delete a running process.
   */
  virtual ~Process();

  /**
   * \brief The starting procedure when this process is created
   * 
   * This method is not defined in the process base class; it must be
   * overridden by the derived class.
   */
  virtual void action() = 0;

  /**
   * \brief Initialize the process.
   *
   * This method is called by the system to initialize the process
   * when the process is created. The method is virtual and does
   * nothing by default; the user is expected to override this method
   * in the derived class to allow some initialization actions.
   */
  virtual void init() {}

  /**
   * \brief Wrap up the process.
   *
   * This method is called by the system to finalize the process
   * before the process is reclaimed. The method is virtual and does
   * nothing by default; the user is expected to override this method
   * in the derived class to incuding any finishing touch.
   */
  virtual void wrapup() {}

  /*
   * \brief Return the owner entity.
   *
   * Every process is owned by an entity. The ownership is specified
   * in the process's constructor and the ownership is immutable.
   */
  inline Entity* owner() { return entity_owner; }

  /** \brief An alias to the Entity::now() method. */
  VirtualTime now();

  /** @name Wait statements. */
  /** @{ */

  /**
   * \brief Block the process until a message arrives at one of the inchannels.
   *
   * This method is a wait statement and therefore must be called
   * within a procedure context. The process will be suspended until
   * an event arrives at any one of the inchannels specified in the
   * argument. If the set is empty, the process will be blocked
   * forever, and therfore the process will be terminated and
   * reclaimed by the system (similar to the waitForever() method).
   * The inchannels must be either owned by the owner entity of this
   * process or owned by entities co-aligned with the owner entity of
   * this process.
   *
   * \param icset the set of inchannels the process will wait on
   */
  void waitOn(const SET(inChannel*)& icset);

  /**
   * \brief Block the process until a message arrives at the inchannel.
   *
   * This method is similar to the previous method. Rather than
   * waiting on a set of inchannels, this method waits on only one
   * inchannel.
   *
   * \param ic the inchannel the process will wait on
   */
  void waitOn(inChannel* ic);

  /**
   * \brief Block on a static set of inchannels.
   *
   * The method is similar to the previous waitOn methods. The
   * inchannels that this process will wait on a set of inchannels
   * previously given through the waitsOn() method. In this case, we
   * can reduce the overhead by statically declare sensitive of the
   * process toward the inchannels and repeatedly call this function
   * without incuring the setup cost each time.
   */
  void waitOn();

  /**
   * \brief Terminate the process.  
   *
   * The method is a wait statement and therefore must be called
   * within a procedure context.  This method will cause the process
   * to suspend forever without the possibility of resumption.
   * Semantically, this is identical to having this process
   * terminated. And the simulator will do just that. The wrapup
   * method will be called before the process is reclaimed.
   */
  void waitForever();

  /**
   * \brief Terminate process but keep the object.
   *
   * This method is similar to the previous method in that the process
   * will be suspended forever. The difference from waitForever() is
   * that this one does not immediately reclaim the process. That is,
   * the process object will remain accessible until the end of the
   * simulation or until the user deletes it explicity.  In some cases
   * this is necessary, for example, when a derived process class
   * contains data fields that need to remain persistent after the
   * process has been terminated.
   */
  void suspendForever();

  /**
   * \brief Block the process for some time.
   *
   * The method is a wait statement and therefore must be called from
   * a procedure. The process will be suspended until the given
   * interval has elapsed.
   *
   * \param delay the time interval for waiting
   */ 
  void waitFor(VirtualTime delay);

  /**
   * \brief Block the process until the given time.
   *
   * The method is a wait statement and therefore must be called from
   * a procedure. The process will be suspended until the given time
   * has reached.
   *
   * \param time the future time until which the process will be unblocked
   */
  void waitUntil(VirtualTime time);

  /**
   * \brief Block the process until a message arrives at any of the
   * inchannels or for a given time interval.
   * 
   * The method is a wait statement and must be called within a
   * procedure context. The method suspends the process to wait for an
   * event to arrive on a set of inchannels or for a given amount of
   * simulation time to pass, whichever happens first. The process
   * will be suspended until one of the two conditions becomes true:
   * either an event is delivered to one of the inchannels specified,
   * or the given simulation time period has elapsed. The method
   * returns true if it's timed out, or false if an event arrives at
   * an inchannel. The result cannot be determined if the both happen
   * simultaneously. The inchannels must be either owned by the owner
   * entity of this process or owned by entities co-aligned with the
   * owner entity of this process.
   *
   * \param icset a set of inchannels the process will wait on
   * \param delay the maximum amount time the process will spend waiting
   * \returns true if the maximum time interval has passed without receiving an event
   * \returns false if an event arrives before the maximum time
   */
  bool waitOnFor(const SET(inChannel*)& icset, VirtualTime delay);

  /**
   * \brief Block the process until a message arrives at an inchannel
   * or for a given time interval.
   * 
   * This method is similar to the previous method. Rather than
   * waiting on a set of inchannels, this method waits on only one
   * inchannel.
   *
   * \param ic the inchannel the process will wait on
   * \param delay the maximum amount time the process will spend waiting
   * \returns true if the maximum time interval has passed without receiving an event
   * \returns false if an event arrives before the maximum time
   */
  bool waitOnFor(inChannel* ic, VirtualTime delay);

  /**
   * \brief Block the process on a static set of inchannels, or for a
   * given time interval.
   * 
   * This method is similar to the previous two waitOnFor()
   * methods. In this case, the process will wait on a set of
   * inchannels previously defined using the waitsOn() methods. 
   *
   * \param delay the maximum amount time the process will spend waiting
   * \returns true if the maximum time interval has passed without receiving an event
   * \returns false if an event arrives before the maximum time
   */
  bool waitOnFor(VirtualTime delay);

  /**
   * \brief Block the process until a message arrives at any of the
   * inchannels or until the given simulation time has been reached.
   * 
   * The method is a wait statement and must be called within a
   * procedure context. The method suspends the process to wait for an
   * event to arrive on a set of inchannels or until a given future
   * simulation time has been reached, whichever happens first. The
   * process will be suspended until one of the two conditions becomes
   * true: either an event is delivered to one of the inchannels
   * specified, or the given simulation time has been reached. The
   * method returns true if it's timed out, or false if an event
   * arrives at an inchannel. The result cannot be determined if the
   * both happen simultaneously. The inchannels must be either owned
   * by the owner entity of this process or owned by entities
   * co-aligned with the owner entity of this process.
   *
   * \param icset a set of inchannels the process will wait on
   * \param time the future time until which the process will be unblocked 
   * \returns true if the simulation time has been reached without receiving an event
   * \returns false if an event arrives before the maximum time
   */
  bool waitOnUntil(const SET(inChannel*)& icset, VirtualTime time);

  /**
   * \brief Block the process until a message arrives at inchannel or
   * until the given simulation time has been reached.
   * 
   * This method is similar to the previous method. Rather than
   * waiting on a set of inchannels, this method waits on only one
   * inchannel.
   *
   * \param ic the inchannel the process will wait on
   * \param time the future time until which the process will be unblocked 
   * \returns true if the simulation time has been reached without receiving an event
   * \returns false if an event arrives before the maximum time
   */
  bool waitOnUntil(inChannel* ic, VirtualTime time);

  /**
   * \brief Block the process on a static set of inchannels, or until
   * the given simulation time has been reached.
   * 
   * This method is similar to the previous two waitOnUntil() methods.
   * In this case, the process will wait on a set of inchannels
   * previously defined using the waitsOn() methods.
   *
   * \param time the future time until which the process will be unblocked 
   * \returns true if the maximum time interval has passed without receiving an event
   * \returns false if an event arrives before the maximum time
   */
  bool waitOnUntil(VirtualTime time);

  /** @} */

  /**
   * \brief Specify a set of inchannels the process will be statically
   * sensitive to.
   *
   * The waitOn() methods, when the inchannels are not specified, will
   * be blocked on the static set of inchannels. Using a static set of
   * inchannels can reduce the overhead for setting up and tearing
   * down internal data structures needed for establishing the
   * association between the inchannels and the processes waiting on
   * the inchannels.  Since it is common for a process in a simulation
   * to always wait on a fixed set of inchannels, the user can call
   * this method to make the process statically sensitive to the
   * inchannels once for all. The method is not a wait statement and
   * therefore it can be called anywhere. Also, the method can be
   * invoked multiple times---the latest one is the one that
   * matters. The inchannels must be either owned by the owner entity
   * of this process or owned by entities co-aligned with the owner
   * entity of this process.
   *
   * \param icset a set of inchannels the process will be statically sensitive to
   *
   * @see waitOn()
   */
  void waitsOn(const SET(inChannel*)& icset);

  /**
   * \brief Specify an inchannels the process will be statically
   * sensitive to.
   *
   * The method is similar to the previous one except, rather than
   * provided with set of inchannels, only one inchannel is declared
   * that the process will be statically sensitive to.
   *
   * \param ic an inchannel the process will be statically sensitive to
   */
  void waitsOn(inChannel* ic);

  /**
   * \brief Return the inchannel that contains the newly arrived event.
   *
   * Suppose the process is blocking on a set of inchannels, and an
   * event arrives at one of the inchannels, the process will be
   * unblocked. The process can receive the event from the inchannel
   * using the inChannels::activeEvent() method. To tell which
   * inchannel in the set of inchannels contains the newly arrived
   * event, the user needs to call this methods.
   */
  inline inChannel* activeChannel() { return active_inchannel; }

 public:
  // used by embedded code to check whether a procedure is called
  // properly; the user shouldn't call procedures directly
  inline bool in_procedure_context() { return procedure_context; }

  // used by embedded code to check whether a process is unblocked
  // from timeout or due to event arrival
  inline bool timed_out() { return process_timedout; }

 private:
  friend class Entity;
  friend class Event;
  friend class inChannel;
  friend class outChannel;
  friend class Semaphore;
  friend class HoldEvent;
  friend class ProcessEvent;
  friend class SemaphoreEvent;
  friend class Procedure;
  friend class Timeline;

  // the four possible states of a process
  enum {
    PROCESS_STATE_CREATING,   // new born
    PROCESS_STATE_RUNNING,    // running
    PROCESS_STATE_WAITING,    // waiting (for events or timeout)
    PROCESS_STATE_TERMINATING // about to be terminated
  };

  Entity* entity_owner; // the owner entity
  int process_state; // can be any of the four states
  ProcedureFunction start_function; // root procedure function
  ProcedureContainer* start_container; // root procedure container
  Procedure* process_stack; // heap stack
  HoldEvent* hold_event; // scheduled event for timed wait
  WaitNode* waiton_node; // cross link between inchannels and waiting processes
  inChannel* active_inchannel; // only one inchannel may activate a process at a time!
  bool active_event_retrieved; // true when active inchannel active event has been retrieved
  bool process_timedout; // set when process is activivated due to timeout
  bool static_sensitivity; // true if blocking on static inchannels
  int procedure_context; // process is active so procedures are invoked properly
  SET(inChannel*) static_inchannels; // all statically sensitive inchannels

 public: // since the following methods are also called by embedded code, we made them public
  // dealing with the procedures on the stack
  inline Procedure* top_stack() { return process_stack; }
  void push_stack(Procedure* proc);
  Procedure* pop_stack();

 private:
  // doing the common work for all the constructors
  void common_constructor();

  // dealing with waiting for inchannels, statically or dynamically
  void sensitize_inchannel(inChannel* ic); // make dynamically sensitive to an in-channel
  void desensitize_inchannels();// remove dynamic sensitivities to all in-channels
  void sensitize_static_inchannel(inChannel* ic); // make statically sensitive to an in-channel
  void desensitize_static_inchannels(); // remove static sensitivities to all in-channels

  void hold_for(VirtualTime delay); // suspend the process for some time
  void hold_until(VirtualTime time); // suspend the process until the specified time

  void execute_process(); // continue running this process
  void activate_procedure(); // activate start procedure or the one on top of stack
  void suspend_process(); // block this process from execution
  void terminate_process(); // terminate this process

  void clear_pending_wait(); // clear the waiting state of the process (called by timeline)
  void clear_process_context(); // clear process context information (timeout flag and active channel)
}; /*class Process*/

// assocate inchannel and its waiting process
class WaitNode {
 public:
  Process* p; // points to the process
  inChannel* ic; // points to the inchannel
  WaitNode* next_p; // next inchannel the process is waiting on
  WaitNode* prev_c; // previous process that is also sensitive to the inchannel
  WaitNode* next_c; // next process that is also sensitive to the inchannel
}; /*class WaitNode*/

}; /*namespace minissf*/

#include "ssfapi/entity.h"
#include "ssfapi/inchannel.h"

#define PROPER_PROCEDURE(p) \
  if(!p || !p->in_procedure_context()) \
    SSF_THROW("improper procedure call")

#endif /*__MINISSF_PROCESS_H__*/

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
