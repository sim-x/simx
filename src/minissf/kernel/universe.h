// one universe per processor on a machine; universes communicate via
// shared memory; remote machines communicate through mpi

#ifndef __MINISSF_UNIVERSE_H__
#define __MINISSF_UNIVERSE_H__

#include <stdio.h>
#include "ssfapi/ssf_common.h"
#include "kernel/ssfmachine.h"
#include "evtlist/simevent.h"
#include "kernel/binque.h"

namespace minissf {

typedef BinaryHeap<VirtualTime> TimelineQueue;

class Universe {
  friend class Timeline;
  friend class Stargate;

  /****** command-line options: universe_cmdline.cc ******/

 protected:
  enum CommandLineOption {
    OPTION_NONE = 0,  // unused
    OPTION_ENDOFOPT,
    OPTION_ALIGN,
    OPTION_DEBUG,
    OPTION_INTV,
    OPTION_NPROCS,
    OPTION_SET_NPROCS,
    OPTION_OFILE,
    OPTION_SEED,
    OPTION_GT,
    OPTION_LT,
    OPTION_SET_LOCAL_THRESH,
    OPTION_SET_TRAINING_LEN,
    OPTION_TOTAL // total number of options
  };
  struct CommandLineOptionStruct {
    CommandLineOption opt;
    const char* str;
    const char* info;
  };
  static CommandLineOptionStruct command_option_array[];

 public: 
  // debug levels
  enum {
    DEBUG_FLAG_SILENT  = 0x00000000,
    DEBUG_FLAG_BRIEF   = 0x00000001,
    DEBUG_FLAG_REPORT  = 0x00000002,
    DEBUG_FLAG_LPSCHED = 0x00000004,
    DEBUG_FLAG_TMSCHED = 0x00000008,
    DEBUG_FLAG_MPIMSG  = 0x00000010
  };

  // command-line arguments
  static int args_nmachs;
  static int args_rank;
  static int* args_mach_nprocs;
  //static STRING* args_mach_names;
  static int args_nprocs;
  static int args_seed;
  static int args_naligns;
  static unsigned int args_debug_mask;
  static STRING args_outfile;
  static VirtualTime args_progress_interval;
  static VirtualTime args_global_thresh;
  static VirtualTime args_local_thresh;
  static VirtualTime args_training_len;
  static bool args_global_thresh_set;
  static bool args_local_thresh_set;
  static VirtualTime args_endtime; // set by ssf_start()
  static double args_speedup; // simtime/realtime; set by ssf_start()

  static int total_num_procs; // this is to cache the total number of processors for all machines

  // parse and remove ssf command-line arguments
  static int parse_command_line(int& argc, char**& argv);

  // print command line options for user
  static void print_command_line(FILE* outfile);

  /****** quick memory: quick_memory.cc ******/

 public:
  char* qmem_chunklist;
  long qmem_poolsize; // the watermark of quick memory
  char* qmem_poolbegin;
  char* qmem_poolend;
  void** qmem_freelist;

  /****** mapping and alignment: universe_mapping.cc ******/

 protected:
  class MapRequest {
   public:
    outChannel* oc;
    inChannel* ic;
    STRING icname;
    VirtualTime delay;
    MapRequest* next;
    MapRequest(outChannel* myoc, inChannel* myic, VirtualTime mydelay) :
      oc(myoc), ic(myic), delay(mydelay), next(0) {}
    MapRequest(outChannel* myoc, STRING myicname, VirtualTime mydelay) :
      oc(myoc), ic(0), icname(myicname), delay(mydelay), next(0) {}
  };

  SET(Timeline*) timelines; // timelines belonging to this universe

  static SET(Timeline*) created_timelines; // all timelines that have already been created
  static MAP(PAIR(int,int),Stargate*) created_stargates; // all stargates that have already been created
  static SET(Entity*) orphan_entities; // entities that haven't been aligned
  static VECTOR(Entity*) created_entities; // entities created during the last round
  static MAP(STRING,inChannel*) local_icmap; // public named inchannel on this machine
  static MAP(STRING,int) remote_icmap; // public named inchannel from remote machine
  static VECTOR(inChannel*) named_inchannels_vector; // used temporarily 
  static int* timeline_scans; // ranges of timeline serial numbers
  static MapRequest* mapreq_head; // outChannel::mapto requests are stored here
  static MapRequest* mapreq_tail; // as a linked list
  static MAP(int,VECTOR(PAIR(MapInport*,Stargate*))*) starmap; // a mapping from outport id to receiving elements

 public:
  void assign_timeline(Timeline* tmln); // add a timeline to this universe

  static Timeline* create_timeline(); // create a timeline
  static void delete_timeline(Timeline* timeline); // remove the timeline
  static void register_stargate(Stargate* sg); // add the stargate to the list
  static Stargate* find_stargate(int srcid, int tgtid); // return the stargate already created if there is
  static void register_orphan_entity(Entity* ent); // entities are created as orphan
  static void deregister_orphan_entity(Entity* ent); // no longer orphan when the entity is aligned
  static void register_named_inchannel(inChannel* ic); // all named inchannel must register
  static void add_mapping(outChannel* oc, inChannel* ic, VirtualTime delay);
  static void add_mapping(outChannel* oc, STRING icname, VirtualTime delay);
  static void synchronize_inchannel_names(); // distribute public inchannel names
  static void wire_up_channels(); // settle all channel mapping requests

  static int timeline_to_machine(int sno); // map from timeline serial number to where it's located
  static void map_local_local(outChannel* oc, inChannel* ic, VirtualTime delay);
  static void map_local_remote(outChannel* oc, int timelineno, VirtualTime& delay); // the delay becomes extra delay afterwards
  static void map_remote_local(int tmlnid, int outportno, inChannel* ic, VirtualTime xdelay);

  /******* parallel universe: universe.cc ******/

 public:
  enum { 
    SIM_STATE_UNINITIALIZED = 0, 
    SIM_STATE_INITIALIZING  = 1, 
    SIM_STATE_RUNNING       = 2, 
    SIM_STATE_FINALIZING    = 3,
    SIM_STATE_FINISHED      = 4
  };

  static Universe** parallel_universe; // one universe for each processor
  static int sim_state;  // state of simulation (0, 1, or 2)
  int processor_id; // which process is this universe in charge of?

  // the constructor and destructor
  Universe(int id);
  virtual ~Universe();

  static void global_init();
  static void global_wrapup();
  void local_init();
  void local_wrapup();
  void run(VirtualTime t, double s);

  // return the stage of the simulation
  static bool is_uninitialized() { return sim_state == SIM_STATE_UNINITIALIZED; }
  static bool is_initializing() { return sim_state == SIM_STATE_INITIALIZING; }
  static bool is_running() { return sim_state == SIM_STATE_RUNNING; }
  static bool is_finalizing() { return sim_state == SIM_STATE_FINALIZING; }
  static bool is_finished() { return sim_state == SIM_STATE_FINISHED; }

  /****** timeline scheduling: universe_sched.cc ******/

 protected:
  static double start_wallclock_time;
  static double time0, time1, time2;

  static VirtualTime epoch_length; // global synchronization window size
  static VirtualTime decade_length; // local synchronization window size
  static ChannelEvent*** switch_board; // for exchanging synchronous events between universes
  VirtualTime synpoint; // lower bound of the current synchronization window
  VirtualTime next_decade, next_epoch; // upper bounds of the current local and global synchronization windows
  bool decade_sync, epoch_sync; // indicate whether the window is in a sync boundary

  // all events beyond the current synchronization window are put here
  // (for composite synchronization) so that they get redistributed at
  // the synchronization barrier
  BinQueue* global_binque;
  BinQueue* local_binque;

  TimelineQueue runnable_timelines;
  TimelineQueue paced_timelines;
  SET(Timeline*) blocked_timelines;
  
  // each processor (other than processor 0) maintains a mailbox to
  // store the channel events sent from remote machines, which are
  // forwarded by handle_io_events() at processor 0; processor 0 also
  // maintains a mailbox, for storing events forwarded by other
  // processors on the same machine to be sent to remote machines
  ssf_thread_mutex_t mailbox_mutex;
  ssf_thread_cond_t mailbox_cond;
  ChannelEvent* mailbox;
  ChannelEvent* mailbox_tail;

#ifdef HAVE_MPI_H
  static ssf_thread_mutex_t remote_mailbox_mutex;
  static ssf_thread_cond_t remote_mailbox_cond;
  static ChannelEvent* remote_mailbox;
  static ChannelEvent* remote_mailbox_tail;

  static char** sendbuf;
  static int* sendpos;
  //static bool irecv_posted;
  //static MPI_Request irecv_request;
  static char* recvbuf;
#endif

 public:
  // the main timeline dispatching loop
  void run();
  void run_continuation();
  void run_end();

  // for dividing sync vs. async channels
  static int classify_global_channels(VirtualTime t, VirtualTime now, bool training);
  static int classify_local_channels(VirtualTime t, VirtualTime now, bool training);
  void classify_timeline_channels();

  // for automatic threshold assignment
  static VECTOR(VirtualTime) global_training_thresholds;
  static VECTOR(VirtualTime) local_training_thresholds;
  static VirtualTime global_training_length;
  static VirtualTime local_training_length;
  static VirtualTime global_training_round;
  static VirtualTime local_training_round;
  static VirtualTime global_roundup;
  static VirtualTime local_roundup;
  static bool global_channel_reclassified;
  static bool local_channel_reclassified;
  static bool training_finished;
  static void enumerate_channel_delays();
  static void trim_channel_delays(MAP(VirtualTime,int)&, VECTOR(VirtualTime)&, int);

  // get the wallclock time relative to the beginning of simulation
  VirtualTime get_wallclock_time();

  // add a timeline to the runnable list (with correct priority)
  void make_timeline_runnable(Timeline* timeline);

  // block the timeline until updated by its predecessor
  void block_timeline(Timeline* timeline);

  // the timeline needs to put on hold until the given time; if the
  // time has already been reached, this method will return false; if
  // the timeline indeed needs to be put on hold, return true
  bool pace_timeline(Timeline* timeline, VirtualTime until);

#ifdef HAVE_MPI_H
  void transport_message(ChannelEvent* evt); // transport event from one machine to another; get it to writer thread
  void transport_reduce_message(); // wait until all transient messages are done with
  void transport_barrier_message(); // do a global barrier
  void transport_terminal_message(); // send special event to inform the writer thread to terminate

  static int* rscnt; // used by reduce scatter
  static int64** sndcnt; // sndcnt[i][j]: the number of channel events sent from processor i to machine j
  static int64 rcvcnt; // the total number of channel events received by the reader thread
  static int64 rcvcnt_target; // the number of channel events to be received before signaling the conditional variable
  static ssf_thread_mutex_t rcvcnt_mutex;
  static ssf_thread_cond_t rcvcnt_cond;
  static ssf_thread_cond_t bar_cond;
  static bool scatter_reduce_carry_on;
  static bool barrier_carry_on;

  // either one thread for both sending and receiving events; or a
  // separate thread for sending and receiving events
  static int mpi_thread_support;
  static ssf_thread_t rw_thread_id;
  static ssf_thread_t writer_thread_id;
  static ssf_thread_t reader_thread_id;
  static void rw_thread();
  static void reader_thread();
  static void writer_thread();

  static void handle_incoming_events(int rbfsz);
  static bool handle_outgoing_events(ChannelEvent* evt);
#endif

  // send and receive external events (emulation events or events from
  // other processors or machines)
  void handle_io_events(bool blocking);

  // called within main sync loop
  void synchronize_events();

 public:
  //  collect statistics
  inline void record_stats_timeline_context_switches() { stats_timeline_context_switches++; }
  inline void record_stats_handle_io_events() { stats_handle_io_events++; }
  inline static void record_stats_mpi_sent_messages(unsigned long bytes) { 
    stats_mpi_sent_messages++; stats_mpi_sent_bytes += bytes; }
  inline static void record_stats_mpi_rcvd_messages(unsigned long bytes) { 
    stats_mpi_rcvd_messages++; stats_mpi_rcvd_bytes += bytes; }

  unsigned long stats_timeline_context_switches;
  unsigned long stats_handle_io_events;
  static unsigned long stats_mpi_sent_messages;
  static unsigned long stats_mpi_sent_bytes;
  static unsigned long stats_mpi_rcvd_messages;
  static unsigned long stats_mpi_rcvd_bytes;

  SSF_CACHE_LINE_PADDING;
}; /*class Universe*/

// used internally for auto-alignment
class metis_node_t {
 public:
  metis_node_t(int id, int size, Timeline* tm);
  ~metis_node_t();
  void add_link(metis_node_t* node, double delay);

  int id;
  int size;
  Timeline* timeline; // maintains a context pointer for referencing
  int part;
  MAP(metis_node_t*,double) links;
};

class metis_graph_t {
 public:
  metis_graph_t();
  ~metis_graph_t();

  int add_node(int size, Timeline* tm);
  void add_link(int idfrom, int idto, double delay);
  void partition(int numparts);
  void minmax_delays();
  int fx(double delay);

  int nlinks;
  double min_delay, max_delay;
  VECTOR(metis_node_t*) nodes;
};

}; /*namespace minissf*/

#include "kernel/timeline.h"
#include "kernel/stargate.h"

#endif /*__MINISSF_UNIVERSE_H__*/

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
