/**
 * \file entity.h
 * \brief Header file for the entity class.
 *
 * This header file contains the definition of the entity class;
 * Users do not need to include this header file directly; it is
 * included from ssf.h.
 */

#ifndef __MINISSF_ENTITY_H__
#define __MINISSF_ENTITY_H__

#include "ssfapi/procedure.h"
#include "kernel/kernel_event.h"
#include "kernel/timeline.h"

namespace minissf {

/**
 * \brief Container for state variables.
 *
 * An entity is a container for state variables. For example, a router
 * or a host in a network model can be represented using an entity. An
 * entity is also a container for processes, inchannels, and
 * outchannels. A user-defined entity must derive from this base
 * entity class.
 */
class Entity : public ProcedureContainer {
 public:
  /**
   * \brief The entity contructor.
   *
   * Entities are static objects. That is, the user can only create
   * entities at the simulation init phase (before ssf_start() is
   * called or within Entity::init() methods). Once created, the
   * entities should never be reclaimed by the user. Instead, they are
   * reclaimed by the simulator after we finish the simulation (after
   * ssf_finalize() is called).
   *
   * When an entity is created, it is independent. That is, it
   * maintains its own timeline, with its own simulation clock that
   * advances independently from other timelines, unless the user
   * align the entity to another entity, in which case the coaligned
   * entities shares the same timeline and the same simulation
   * clock. Since coaligned entities are time synchronized, they can
   * directly access each other's state variables, processes,
   * inchannels and outchannels.
   *
   * We added an extension to this entity constructor for real-time
   * simulation: the user can make this entity an "emulated" entity,
   * which means that events associated with this entity will be
   * 'pinned down' to real time; for an entity that does not handle
   * emulation events, nor does it care about real time issues, this
   * flag should be false (as the default).
   *
   * \param emulated whether this entity is an emulated entity
   */
  Entity(bool emulated = false);

  /**
   * \brief The entity destructor.
   *
   * The user should never delete an entity directly. Entities are
   * static objects. The simulator reclaims all entities after the
   * simulation is finished.
   */
  virtual ~Entity();

  /** \brief Return whether this entity is an emulated entity. */
  bool isEmulated() const { return emulated; }

 /**
   * \brief Initialize a newly created entity.
   *
   * The init method is called by the simulator immediately after the
   * entity is created. The method in this base class is virtual and
   * does nothing by default; the user is expected to override this
   * method in the derived class.
   */
  virtual void init() {}

  /**
   * \brief Wrap up the entity before the simulator reclaims it.
   *
   * The wrapup method is called by the simulation before the entity
   * is about to be detroyed (i.e., before the entity's destructor is
   * invoked). The method in the base class is virtual and does
   * nothing by default; the user may override this method in the
   * derived class. Note that all living entities will be destroyed
   * automatically at the end of the simulation and their wrapup
   * methods will be called one by one then.
   */
  virtual void wrapup() {}


  /**
   * \brief Return the current simulation time of this entity.
   *
   * There is no global simulation clock in parallel simulation. Only
   * co-aligned entities share the same simulation clock. That is,
   * entities in the same alignment group (called a timeline) is
   * guaranteed to advance synchronously in simulation time. Entities
   * not co-aligned may experience different simulation time.
   * Therefore, it is incorrect for an entity to access the state
   * variables (as well as processes, inchannels and outchannels) of
   * another entity on a different timeline. Time advancement of the
   * entities is dictated by the underlying parallel simulation
   * protocol. The return value of this method is the current
   * simulation time of this entity and its co-aligned entities.
   */
  VirtualTime now() const;
  
  /**
   * \brief Set the alignment of the entity.
   *
   * It is important to know that entities are born independent unless
   * they are aligned to another entity. All coaligned entities share
   * the same timeline and advance in time synchronously. Entity
   * (re-)alignment can only take place during the simulation init
   * phase. Alignment is cumulative. Suppose entity A is aligned with
   * entity B, and entity C is aligned with entity D, and if the user
   * want to align B to C, then all four entities should be
   * coaligned. Once two or more entities are aligned together, they
   * cannot back out.
   */
  void alignto(Entity* entity);

  /** \brief Return the set of coaligned entities (including this entity). */
  const SET(Entity*)& coalignedEntities();

  /** \brief Return the set of processes defined for this entity. */
  inline const SET(Process*)& getProcesses() { return processes; }

  /** \brief Return the set of inchannels of this entity. */  
  inline const SET(inChannel*)& getInChannels() { return inchannels; }

  /** \brief Return the set of outchannels of this entity. */ 
  inline const SET(outChannel*)& getOutChannels() { return outchannels; }

 private:
  friend class ent;
  friend class Process;
  friend class inChannel;
  friend class outChannel;
  friend class Timeline;
  friend class Timer;
  friend class Semaphore;
  friend class KernelEvent;
  friend class TimerEvent;
  friend class HoldEvent;
  friend class ProcessEvent;
  friend class Universe;

  Timeline* timeline; // the owning timeline
  bool emulated; // indicate whether this entity is emulated
  int serialno; // globally unique serial number
  int nxtevtid; // all events from this entity are numbered sequentially
 
  SET(inChannel*) inchannels; // set of inchannels defined for this entity
  SET(outChannel*) outchannels; // set of outchannels defined for this entity
  SET(Process*) processes; // set of processes defined for this entity
  SET(KernelEvent*) init_events; // temporarily store events during initialization
  
  class InitWrite {
   public:
    Event* evt;
    VirtualTime wdelay;
    outChannel* oc;
    InitWrite(Event* e, VirtualTime d, outChannel* o) :
      evt(e), wdelay(d), oc(o) {}
  };
  SET(InitWrite*) init_writes;

 protected:
  //inline Timeline* get_timeline() const { return timeline; }
  //inline void set_timeline(Timeline* t) { timeline = t; }
  //inline int get_serialno() { return serialno; }
  //inline void set_serialno(int s) { serialno = s; }
  inline int get_next_event_id() { return nxtevtid++; }

  void add_inchannel(inChannel* ic);
  void delete_inchannel(inChannel* ic);
  void add_outchannel(outChannel* oc);
  void delete_outchannel(outChannel* oc);
  void add_process(Process* p);
  void delete_process(Process* p);

  void insert_event(KernelEvent*);
  void cancel_event(KernelEvent*);

  //XXX temporarily store events before the entity is wired correctly
  void init_write_event(Event* evt, VirtualTime write_delay, outChannel* oc);
  void schedule_init_events();
}; /*class Entity*/

}; /*namespace minissf*/

#include "ssfapi/process.h"
#include "ssfapi/inchannel.h"
#include "ssfapi/outchannel.h"
#include "ssfapi/event.h"

#endif /*__MINISSF_ENTITY_H__*/

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
