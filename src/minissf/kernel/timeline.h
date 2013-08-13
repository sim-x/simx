// timeline is a logical process which maintains its own eventlist and
// simulation clock

#ifndef __MINISSF_TIMELINE_H__
#define __MINISSF_TIMELINE_H__

#include "ssfapi/ssf_common.h"
#include "kernel/kernel_event.h"

namespace minissf {

typedef BinaryHeapNode<VirtualTime> TimelineQueueNode;

// another name for logical process
class Timeline : public TimelineQueueNode {
 public:
  Timeline();
  virtual ~Timeline();

  // return the current simulation clock
  inline VirtualTime now() const { return simclock; }

  // managing entities in a timeline
  void add_entity(Entity* entity);
  void delete_entity(Entity* entity);
  void merge_timeline(Timeline* timeline);
  inline const SET(Entity*)& get_entities() const { return entities; }

  // all entities and timelines must be uniquely identified; the
  // timelines are uniquely identified because they are used to
  // identify cross-time traffic, the entities are uniquely identified
  // because they are used for tie-breaking simultaneous events
  //inline int get_serialno() const { return serialno; }
  inline int get_serialno_space() const { return entities.size(); }
  int get_portno_space() const;
  inline void settle_universe(Universe* univ) { universe = univ; }
  int settle_serialno(int tmlnid, int startentid); // for both timelines and entities
  int settle_portno(int id); // for all outchannels

  // this timeline is emulated if one of its entity is
  bool is_emulated();

  // insert and cancel events in the event list
  void insert_event(KernelEvent* evt);
  void cancel_event(KernelEvent* evt);

  // called by entity to insert an emulation event
  void insert_emulated_event(EmulatedEvent* evt);

  // for getting the next event from the event list
  bool no_more_events();
  KernelEvent* peek_next_event();
  void pop_next_event(KernelEvent* evt);

  // processing all events until the event horizon, although it's
  // possible to stop processing events before the horizon is reached,
  // for example, due to flow control reasons; return how far the
  // timeline has progressed
  VirtualTime run();

  // return the process that's currently running
  inline Process* current_process() { return active_processes.front(); }

  // insert a process onto the ready list
  void activate_process(Process* p);

  // return the first process that has been activated
  Process* get_active_process();

  // remove running process from ready queue and assign a new state
  void deactivate_process(int newstate);

  // add ingress and outgress edges
  void add_inbound_stargate(Stargate* sg);
  void add_outbound_stargate(Stargate* sg);
  void classify_async(); // called to pick out those async stargates

  void retrieve_incoming_and_calculate_lowerbound();
  void update_subsequent_timelines();
  VirtualTime next_emulation_due_time();

 public:
  // collecting statistics
  inline void record_stats_processed_events() { stats_processed_events++; } 
  inline void record_stats_process_context_switches() { stats_process_context_switches++; }
  inline void record_stats_procedure_calls() { stats_procedure_calls++; }
  inline void record_stats_remote_messages() { stats_remote_messages++; }
  inline void record_stats_shmem_messages() { stats_shmem_messages++; }
  inline void record_stats_local_messages() { stats_local_messages++; }
  inline void record_stats_remote_null_messages() { stats_remote_null_messages++; }
  inline void record_stats_shmem_null_messages() { stats_shmem_null_messages++; }
  inline void record_stats_local_null_messages() { stats_local_null_messages++; }
  inline void record_stats_lbts_calculations() { stats_lbts_calculations++; }
  inline void record_stats_subsequent_updates() { stats_subsequent_updates++; }

 private:
  enum { STATE_START, STATE_RUNNING, STATE_PACING, STATE_WAITING, STATE_ROUND, STATE_DONE };

  Universe* universe; // where does this timeline reside?
  int serialno; // each timeline is uniquely identified
  int state; // which queue is this timeline currently located?
  bool emulated; // whether this timeline needs to be pinned down in real time
  bool emulated_set; // true if this timeline is emulated has been determined
  bool emulated_timer_set; // true if the emulated timer has been scheduled
  VirtualTime responsiveness; // min responsiveness of all emulated entities
  VirtualTime lbts; // lower bound on timestamp
  VirtualTime simclock; // current simulation time increases monotonically

  KernelEventList evtlist; // eventlist containing all future events (simulated and emulated)
  //KernelEventList simlist; // eventlist containing all future simulation events
  //KernelEventList emulist; // eventlist containing all future emulation events
  SET(Entity*) entities; // list of entities defined in this timeline
  DEQUE(Process*) active_processes; // list of processes ready to run
  VECTOR(Stargate*) inbound; // incoming portals to receive events from other timelines
  VECTOR(Stargate*) outbound; // /outgoing portals to send events to other timelines
  VECTOR(Stargate*) inbound_async; // incoming portals to receive async events from other timelines
  VECTOR(Stargate*) outbound_async; // /outgoing portals to send aync events to other timelines

 public:
  // statistics to be collected for performance tuning
  unsigned long stats_processed_events;
  unsigned long stats_process_context_switches;
  unsigned long stats_procedure_calls;
  unsigned long stats_remote_messages;
  unsigned long stats_shmem_messages;
  unsigned long stats_local_messages;
  unsigned long stats_remote_null_messages;
  unsigned long stats_shmem_null_messages;
  unsigned long stats_local_null_messages;
  unsigned long stats_lbts_calculations;
  unsigned long stats_subsequent_updates;

  friend class Entity;
  friend class outChannel;
  friend class Universe;
  friend class Stargate;
  friend class TickEvent;
  friend class EmulatedTimerEvent;
}; /*class Timeline*/

}; /*namespace minissf*/

#include "kernel/universe.h"
#include "kernel/stargate.h"

#endif /*__MINISSF_TIMELINE_H__*/

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
