/**
 * \file ssf_timer.h
 * \brief Header file for the timer class.
 *
 * This header file contains the definition of the timer class; Users
 * do not need to include this header file directly; it is included
 * from ssf.h.
 */

#ifndef __MINISSF_TIMER_H__
#define __MINISSF_TIMER_H__

#include "ssfapi/ssf_common.h"
#include "kernel/kernel_event.h"

namespace minissf {

/** \brief Timers for scheduling actions in the simulated future.
 *
 * A timer encapsulates a callback function, which can either be a
 * member function of an entity class, a regular function with a
 * designated signature, or the callback() method of this class. A
 * timer must be owned by an entity (i.e., a timer is treated as a
 * state of an entity).  The timer can be scheduled to fire off at a
 * future simulation time. It can also be cancelled if necessary
 * before it goes off. If a timer goes off, minissf will call the
 * associated callback function with a pointer to the timer object as
 * the function argument. The user can design a derived class of this
 * class to carry meaningful user data so that when the timer goes
 * off, the callback function will be able to extract the data.
 */
class Timer {
 public:
  /** \brief The constructor of a timer using an entity method as the
   * callback function.
   *
   * A timer must be owned by an entity.  The constructor requires a
   * pointer to an entity with which the timer will be permanently
   * associated. The callback function is an entity method and must
   * take as argment a pointer to the timer.
   *
   * \param owner points to the owner entity of this timer
   * \param callback points to the callback method of the owner entity
   */
  Timer(Entity* owner, void (Entity::*callback)(Timer*));

  /** \brief The constructor of a timer using a regular function as
   * the callback function.
   *
   * A timer must be owned by an entity.  The constructor requires a
   * pointer to an entity with which the timer will be permanently
   * associated. The callback function is a function that takes as
   * argment a pointer to the timer.
   *
   * \param owner points to the owner entity of this timer
   * \param callback points to the callback method of the owner entity
   */
  Timer(Entity* owner, void (*callback)(Timer*));

  /** \brief The constructor of a timer using the callback() method as
   * the callback function.
   *
   * A timer must be owned by an entity.  The constructor requires a
   * pointer to an entity with which the timer will be permanently
   * associated. The callback function is the callback() method of
   * this class. The user is expected to create a class derived from
   * this class and override the callback() method to handle the
   * timeout.
   *
   * \param owner points to the owner entity of this timer
   */
  Timer(Entity* owner);

  /** \brief The destructor of the timer.
   *
   * If the timer is running, that is, the timer is scheduled to fire
   * off at a specified simulation time, the function will first
   * cancel the timer (by descheduling the timeout event) before
   * reclaiming the timer itself.
   */
  virtual ~Timer();

  /** \brief Return the entity owner of this timer.
   *
   * The entity owner is permanently set upon creation of the
   * timer. The ownership is immutable.
   */
  Entity* owner() const { return entity_owner; }

  /**
   * \brief Set the timer to go off after some delay.
   *
   * This method schedules a timer event to go off after the given
   * delay. When the timer expires, the associated callback function
   * will be invoked. Attempting to schedule a timer that has already
   * been scheduled (i.e., when the timer is running) is an error. The
   * user should use reschedule() in this case.
   *
   * \param delay the delay (from now) before the timer is scheduled
   * to go off
   */
  void schedule(VirtualTime delay);

  /**
   * \brief Reset the timer to go off after some delay.
   *
   * This method is similar to the schedule() method: it schedules the
   * timer to go off after the specified delay. The only difference is
   * that, if the timer is currently running, the timer is first
   * cancelled before it is reset with the new delay.

   * \param delay the delay (from now) before the timer is scheduled
   * to go off
   *
   */
  void reschedule(VirtualTime delay);

 /**
   * \brief Cancel the timer.
   *
   * If the timer is not running, the function is simply a no-op.
   */
  void cancel();
  
  /**
   * \brief Return true if the timer is running.
   *
   * If the timer has been scheduled to go off at a future simulation
   * time (i.e., the timer is running), this function will return
   * true. A timer has but two states: running or not running.
   */
  bool isRunning() const { return timer_event != 0; }

  /**
   * \brief Return the time the timer is scheduled to go off.
   *
   * If the timer is not running, the return value will be
   * undefined. So to use this function, it's important to know
   * whether the timer is running or not.
   * 
   * @see isRunning()
   */
  VirtualTime time() const { return fire_time; }

  /**
   * \brief This is the callback function if the constructor
   * Timer(Entity* owner) is used.
   *
   * The user is expected to create a class derived from this class
   * and override this method to handle timeout. In cases where the
   * user wants to pass user-specific data to the callback function,
   * creating a subclass of this timer class is the best way.
   */
  virtual void callback() {}

 protected:
  friend class TimerEvent;

  // a timer must be owned by an entity; the ownership is immutable
  Entity* entity_owner; 

  // this variable is used only if the callback function is set to be
  // an entity method (created using the first constructor)
  void (Entity::*timer_entity_callback)(Timer*); 

  // this variable is used only if the callback function is a regular
  // function (created using the second constructor)
  void (*timer_callback)(Timer*);

  // if the timer is running, this variable is set to be the
  // simulation time the timer is scheduled to go off; if the timer
  // is not running, this variable is undefined
  VirtualTime fire_time;

  // if the timer is running, this variable points to the kernel
  // event on the eventlist representing the timer
  TimerEvent* timer_event; 
}; /*class Timer*/

}; /*namespace minissf*/

#include "ssfapi/entity.h"

#endif /*__MINISSF_TIMER_H__*/

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
