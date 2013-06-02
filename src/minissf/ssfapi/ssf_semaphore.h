/**
 * \file ssf_semaphore.h
 * \brief Header file for the semaphore class.
 *
 * This header file contains the definition of the semaphore class;
 * Users do not need to include this header file directly; it is
 * included from ssf.h.
 */

#ifndef __MINISSF_SEMAPHORE_H__
#define __MINISSF_SEMAPHORE_H__

#include "ssfapi/ssf_common.h"

namespace minissf {

/** \brief Semaphores used for synchronizing processes with an entity.
 *
 * It is frequently the case that an SSF process needs to signal
 * another process about its progress without passing data. Normally,
 * this can be achieved by using two channels: one outchannel and one
 * inchannel. The channels are mapped together and events are sent as
 * the signal from one process to the other. This situation becomes
 * more complicated when there are more processes involved. The
 * semaphores can be used here to ease this situation. A semaphore
 * must be an entity state (that is, it's a member variable of the
 * user-defined entity class). Processes belonging to the same entity
 * can operate on this semaphore in a way similar to an operating
 * system semaphore for inter-process communications (such as in the
 * typical producer-consumer scenario).
 */
class Semaphore { 
 public:
  /** \brief The constructor of a semaphore.
   *
   * A semaphore must be owned by an entity. One can provide an
   * initial value of the semaphore. A wait call will decrement the
   * semaphore value; if the value goes below zero, the caller process
   * will be suspended. A signal call will increment the semaphore
   * value; if there are processes waiting on the semaphore, the first
   * process at the front of the queue will be unblocked.
   *
   * \param owner points to the entity that owns this semaphore
   * \param initval the initial value of the semaphore (default is zero if omitted)
   */
  Semaphore(Entity* owner, int initval = 0);

  /** \brief The destructor of the semaphore.
   *
   * A semaphore can only be destroyed during the simulation
   * finalization (when ssf_finalize() is called). That is, if the
   * user wants to explicitly delete a semaphore, it should be done
   * either in the entity's wrapup() method or in the entity's
   * destrctor.
   */
  virtual ~Semaphore();

  /** \brief Decrement the semaphore.
   * 
   * The function is a wait function. That is, it can only be called
   * within a procedure (of a process). It decrements the semaphore,
   * and if the semaphore value becomes negative, ther caller process
   * will be blocked.
   */
  void wait();
  
  /** \brief Increment the semaphore.
   *
   * The function increments the semaphore. If there are processes
   * waiting on the semaphore, the first of such processes will be
   * unblocked. The processes blocked on a semaphore are
   * first-come-first-serve (FCFS).
   */
  void signal();

  /** \brief Return the current semaphore value.
   *
   * The semaphore value is initialized when it is created. The value
   * increases when the signal() method is called, and decreases when
   * the wait() method is called.
   */
  inline int value() { return sem_count; }

 protected:
  // the owner entity; the ownership is immutable
  Entity* entity_owner;

  // the current semaphore value
  int sem_count;

  // the data structure we use for keeping the waiting processes is a
  // double ended queue
  DEQUE(Process*) blocked_processes;
}; /*class Semaphore*/

}; /*namespace minissf*/

#endif /*__MINISSF_SEMAPHORE_H__*/

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
