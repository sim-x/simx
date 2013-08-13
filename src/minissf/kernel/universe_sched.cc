#include <string.h>
#include <sys/time.h>
#include "kernel/universe.h"
#include "ssf.h"

/* MPIBUF_SIZE is the size the preallocated buffers for mpi messages
 (both at the sending side and at the receiving side). We make a
 best-effort to pack the events sent from the same source rank to the
 same destination rank into a larger mpi mpi message. We stop packing
 when the packing size is larger than MPIBUF_THRESOLD, which is set to
 be half of MPIBUF_SIZE. IMPORTANT: AN MPI MESSAGE MUST NOT BE LARGER
 THAN MPIBUF_THRESHOLD, OR WE'LL HAVE BUFFER OVERFLOW!!! */
#define MPIBUF_THRESHOLD 10240
#define MPIBUF_SIZE (2*MPIBUF_THRESHOLD)
#define MPIBUF_ATTACHED (64*1024*1024)

/* This is the mpi tag used to send and receive events. IMPORTANT: THE
   USER WHO ALSO USES MPI FOR COMMUNICATION MUST NOT USE THIS TAG. */
#define CHANNEL_EVENT_TAG 100

// namespace simx
// {
//   extern void simx_restore_py_main_thread_state();
// }



namespace minissf {

/* We record the wall clock time when the simulation starts, so that
   we can pace simulation relative to real time. */
int64 Universe::start_wallclock_time;

/* We record these times for reporting simulation init and run time. */
int64 Universe::time0;
int64 Universe::time1;
int64 Universe::time2;

/* This is the mininum of all synchronous channel link delays. */
VirtualTime Universe::epoch_length;
VirtualTime Universe::decade_length;

/* This is a 2D array for universes on the same machine to exchange
   their synchronous events for the next window. */
ChannelEvent*** Universe::switch_board = 0;

#if HAVE_MPI_H
/* remote_mailbox is the head of a linked list containing the channel
   events to be sent out from this machine to all remote machines.
   remote_mailbox_tail points to the end of the linked list. Channel
   events are deposited here by a universe via transport_message
   (producer), and they are retrieved and sent by the writer thread
   (consumer). We use conditional variables to synchronize the
   producer(s) and the consumer. */
ssf_thread_mutex_t Universe::remote_mailbox_mutex;
ssf_thread_cond_t Universe::remote_mailbox_cond;
ChainedEvent* Universe::remote_mailbox = 0;
ChainedEvent* Universe::remote_mailbox_tail = 0;

/* There is one send buffer (sendbuf) of size MPIBUF_SIZE created for
   each remote machine by the writer thread only when there is a
   message targeting the remote machine.  sendpos indicates the last
   packing position. There is only one receive buffer (recvbuf) to
   store mpi messages from all remote machines. */
char** Universe::sendbuf = 0;
int* Universe::sendpos = 0;
//bool Universe::irecv_posted = false;
//MPI_Request Universe::irecv_request;
char* Universe::recvbuf = 0;

int* Universe::rscnt = 0;
int64** Universe::sndcnt = 0;
int64 Universe::rcvcnt = 0;
int64 Universe::rcvcnt_target = 0;
ssf_thread_mutex_t Universe::rcvcnt_mutex;
ssf_thread_cond_t Universe::rcvcnt_cond;
ssf_thread_cond_t Universe::bar_cond;
bool Universe::scatter_reduce_carry_on;
bool Universe::barrier_carry_on;

/* These are simple functions used to create the reader and writer
   thread with. They call the corresponding static methods of the
   universe class. */
int Universe::mpi_thread_support;
ssf_thread_t Universe::rw_thread_id;
ssf_thread_t Universe::writer_thread_id;
ssf_thread_t Universe::reader_thread_id;

extern pthread_key_t thread_id;

static void rw_thread_start(void* data) 
{ 
  // the new thread should become thread 0
  Universe::rw_thread(); 
  
  // Universe* univ = (Universe*)data;
  // pthread_setspecific(thread_id, univ);
  // //restore simx python thread
  // simx::simx_restore_py_main_thread_state();
  // univ->run_continuation();
}

static void reader_thread_start(void* data) { Universe::reader_thread(); }
static void writer_thread_start(void* data) { Universe::writer_thread(); }
#endif

void Universe::synchronize_events()
{
  // if there are more than one processor, we do need to do local
  // barrier synchronization since the current synchronization point
  // is right at the local window edge
  if((decade_sync || epoch_sync) && args_nprocs > 1) {
    assert(local_binque);
    assert(switch_board);
    // dispatch local events
    /*
    printf(">> [%d:%d] %lg: retrieve events next_decade=%lg\n", 
	   args_rank, processor_id, synpoint.second(), next_decade.second());
    */
    ChannelEvent* local_evts = local_binque->retrieve_events(next_decade);
    while(local_evts) {
      ChannelEvent* e = local_evts;
      local_evts = (ChannelEvent*)e->get_next_event();
      int pid = e->stargate->target_timeline->universe->processor_id;
      assert(processor_id != pid);
      e->stargate->source_timeline->record_stats_shmem_messages();
      e->get_next_event() = switch_board[processor_id][pid];
      switch_board[processor_id][pid] = e;
    }
    ssf_barrier();
    for(int i=0; i<args_nprocs; i++) {
      local_evts = switch_board[i][processor_id];
      switch_board[i][processor_id] = 0;
      while(local_evts) {
	ChannelEvent* e = local_evts;
	local_evts = (ChannelEvent*)e->get_next_event();
	//e->get_next_event() = 0;
	/*
	printf("[%d:%d]   e=%lg from p%d\n", args_rank, processor_id, 
	       VirtualTime(e->time()).second(), i);
	*/
	e->stargate->target_timeline->insert_event(e);
      }
    }
  }

#ifdef HAVE_MPI_H
  // if there are more than one machine, we do need to do global
  // barrier synchronization if the current synchronization point is
  // right at the global window edge
  if(epoch_sync && args_nmachs > 1) { 
    assert(global_binque);
    // dispatch global events
    ChannelEvent* evts = global_binque->retrieve_events(next_epoch);
    //printf("%d:%d: HERE 1\n", args_rank, processor_id);
    
    // sent the event through the writer thread, which records the
    // number of messages being sent
    while(evts) {
      ChannelEvent* e = evts;
      evts = (ChannelEvent*)e->get_next_event();
      assert(e->stargate && e->stargate->source_timeline);
      assert(!e->stargate->target_timeline);
      transport_message(e);
    }
    //printf("%d:%d: HERE 2\n", args_rank, processor_id);
    ssf_barrier();

    if(!processor_id) {
      // a global reduction so that each machine knows the number of
      // messages expected to be received (by the reader thread)
      // before it can continue
      //printf("%d:%d: >> %lld %lld\n", args_rank, processor_id, sndcnt[0][0], sndcnt[0][1]);
      transport_reduce_message();
    }

    // it's important to use the barrier here, since we need to make
    // sure all the receiving events by the reader thread have been
    // delivered to the other processors mailboxes before they can
    // be retrieved using handle_io_events() next
    ssf_barrier(); 
    handle_io_events(false); // so that all synchronous events are inserted into the corresponding timelines

    // it's important to use the mpi barrier here too, since we
    // don't want to mix up event delivery at different rounds
    if(!processor_id) {
      transport_barrier_message();
    }
    ssf_barrier(); // necessary?
  }
#endif
}


void Universe::run()
{
  // wait for ALL universes to complete the initialization phrase
  // (using the barrier) and then we can move onto the running phase
  ssf_barrier(); 
  if(!processor_id) {
    sim_state = SIM_STATE_RUNNING; 

#ifdef HAVE_MPI_H
    // initialize the variables, set up the send and receive buffers,
    // and spawn the reader and writer threads (or one r/w thread)
    // only when we detect there are more than one mpi processes
    
    if(args_nmachs > 1) {
      ssf_thread_mutex_init(&remote_mailbox_mutex);
      ssf_thread_cond_init(&remote_mailbox_cond);
      char* attachbuf = new char[MPIBUF_ATTACHED]; assert(attachbuf);
      ssf_mpi_buffer_attach(attachbuf, MPIBUF_ATTACHED);
      sendbuf = new char*[args_nmachs]; assert(sendbuf);
      sendpos = new int[args_nmachs]; assert(sendpos);
      memset(sendbuf, 0, sizeof(char*)*args_nmachs); // we will allocate each send buffer on demand
      recvbuf = new char[MPIBUF_SIZE]; assert(recvbuf);

      rscnt = new int[args_nmachs]; assert(rscnt);
      for(int i=0; i<args_nmachs; i++) rscnt[i] = 1;

      sndcnt = new int64*[args_nprocs]; assert(sndcnt);
      for(int i=0; i<args_nprocs; i++) {
	sndcnt[i] = new int64[args_nmachs]; 
	assert(sndcnt[i]);
	memset(sndcnt[i], 0, args_nmachs*sizeof(int64));
      }
      ssf_thread_mutex_init(&rcvcnt_mutex);
      ssf_thread_cond_init(&rcvcnt_cond);
      ssf_thread_cond_init(&bar_cond);

      if(mpi_thread_support == MPI_THREAD_MULTIPLE) {
	ssf_thread_create(&reader_thread_id, &reader_thread_start, this);
	ssf_thread_create(&writer_thread_id, &writer_thread_start, this);
      } else {
	ssf_thread_create(&rw_thread_id, &rw_thread_start, this); // which will assume the role of thread 0
	//rw_thread(); // the old thread 0 becomes the i/o thread
	//run_end(); // then, it switches back to thread 0
	run_continuation();
	run_end();
	return;
	//return; 
      }
    }
#endif
  }
  run_continuation();
}

void Universe::run_continuation()
{
  // if there are more than one machine or more than one processor on
  // this machine, we'd use binque to redistribute events during
  // composite synchronization barrier
  if(args_nmachs > 1) {
    global_binque = new BinQueue(); 
    assert(global_binque);
  }
  if(args_nprocs > 1) {
    local_binque = new BinQueue(); 
    assert(local_binque);
  }

  // now we can schedule all the init events
  for(SET(Timeline*)::iterator tmln_iter = timelines.begin();
      tmln_iter != timelines.end(); tmln_iter++) {
    Timeline* tmln = *tmln_iter;
    for(SET(Entity*)::iterator iter = tmln->entities.begin();
	iter != tmln->entities.end(); iter++) 
      (*iter)->schedule_init_events();
  }
  ssf_barrier();

  // wait for every universe to reach this point and we start simulation!
  if(!processor_id) {
    // update channel time
    /*
    for(MAP(PAIR(int,int),Stargate*)::iterator sg_iter = created_stargates.begin();
	sg_iter != created_stargates.end(); sg_iter++) 
      (*sg_iter).second->time = (*sg_iter).second->min_delay;
    */
    
    // record the starting wall clock time
    time1 = start_wallclock_time = ssf_wallclock_in_nanoseconds();
  }
  ssf_barrier();

  // set up local and global training (only used by processor 0)
  VirtualTime l_opt, g_opt;
  int l_idx, g_idx;
  int64 t0, t_l_opt = 0, t_g_opt = 0;
  if(!processor_id) {
    training_finished = global_channel_reclassified = 
      local_channel_reclassified = false;
    if(0 >= local_training_length || local_training_length >= args_endtime) {
      if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	printf("[%d] no one's doing local thresh, set default local thresh=%lg\n", 
	       args_rank, args_local_thresh.second());
      }
      classify_local_channels(args_local_thresh, 0, false);
      local_roundup = VirtualTime::INFINITY;
      l_opt = args_local_thresh;
    
      if(0 >= global_training_length ||
	 local_training_length+global_training_length >= args_endtime) {
	if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	  printf("[%d] no one's doing global thresh, set default global thresh=%lg\n", 
		 args_rank, args_global_thresh.second());
	}
	classify_global_channels(args_global_thresh, 0, false);
	global_roundup = VirtualTime::INFINITY;
	g_opt = args_global_thresh;
	training_finished = true;
      } else {
	assert(global_training_thresholds.size() > 0);
	if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	  printf("[%d] start global thresh, set global thresh[0]=%lg\n", 
		 args_rank, global_training_thresholds[0].second());
	}
	classify_global_channels(global_training_thresholds[0], 0, true);
	g_idx = 0;
	global_roundup = global_training_round;
	t0 = ssf_wallclock_in_nanoseconds();
      }
    } else if(local_training_thresholds.size() <= 0) {
      if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	printf("[%d] done local thresh, set default local thresh=%lg\n",
	       args_rank, args_local_thresh.second());
      }
      classify_global_channels(local_training_length, 0, true);
      classify_local_channels(args_local_thresh, 0, false);
      local_roundup = VirtualTime::INFINITY;
      l_opt = args_local_thresh;
      global_roundup = local_training_length;
    } else {
      if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	printf("[%d] start local thresh, set local thresh[0]=%lg\n", 
	       args_rank, local_training_thresholds[0].second());
      }
      classify_global_channels(local_training_length, 0, true);
      classify_local_channels(local_training_thresholds[0], 0, true);
      l_idx = 0;
      local_roundup = local_training_round;
      global_roundup = local_training_length;
      t0 = ssf_wallclock_in_nanoseconds();
    }
  }

  ssf_barrier();
  /*
  printf("<><><> %d: local_training_length=%lg, local_roundup=%lg, global_roundup=%lg, epoch_length=%lg, decade_length=%lg\n",
	 processor_id, local_training_length.second(), local_roundup.second(), global_roundup.second(), epoch_length.second(), decade_length.second());
  */

  classify_timeline_channels();
  if(training_finished) {
    if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
      printf("[%d:%d] training finished, resetting binques\n", 
	     args_rank, processor_id);
    }
    if(global_binque) {
      int n;
      if(args_endtime/1024 > epoch_length) n = 1024;
      else n = int(args_endtime/epoch_length)+1;
      global_binque->settle(epoch_length, n, 0);
    }
    if(local_binque) {
      int n;
      if(args_endtime/1024 > decade_length) n = 1024;
      else n = int(args_endtime/decade_length)+1;
      local_binque->settle(decade_length, n, 0); 
    }
  }

  // entering the main LP scheduling loop
  synpoint = 0; 
  next_decade = decade_length; 
  next_epoch = epoch_length;
  if(next_decade > args_endtime) next_decade = args_endtime;
  if(next_decade > local_roundup) next_decade = local_roundup;
  if(next_epoch > args_endtime) next_epoch = args_endtime;
  if(next_epoch > global_roundup) next_epoch = global_roundup;
  decade_sync = epoch_sync = false;

  VirtualTime next_window = next_decade;
  if(next_epoch < next_window) next_window = next_epoch;

  while(synpoint < args_endtime) { // the simulation hasn't finished
    ssf_barrier();

    synchronize_events();

    if(!processor_id) {
      local_channel_reclassified = 
	global_channel_reclassified = false;
    }

    if(decade_sync) {
      if(synpoint == local_roundup && !processor_id) {
	int64 t1 = ssf_wallclock_in_nanoseconds();
	int64 dt = t1-t0; t0 = t1;
	if(!t_l_opt || dt < t_l_opt) {
	  t_l_opt = dt;
	  l_opt = local_training_thresholds[l_idx];
	}
	l_idx++;
	if(l_idx < (int)local_training_thresholds.size()) {
	  if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	    printf("[%d] %lg: continue local thresh, set local thresh[%d]=%lg\n", 
		   args_rank, synpoint.second(), l_idx, local_training_thresholds[l_idx].second());
	  }
	  classify_local_channels(local_training_thresholds[l_idx], synpoint, true);
	  local_roundup += local_training_round;
	  //printf("local_roundup now is %lg\n", local_roundup.second());
	} else {
	  /* ***** DEBUG *****
	  LehmerRandom rng(args_rank);
	  l_opt = local_training_thresholds[rng.equilikely(0,local_training_thresholds.size()-1)]; 
	  if(args_rank == 0) l_opt = local_training_thresholds[1];
	  else l_opt = local_training_thresholds[0];
	  */
	  if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	    printf("[%d] %lg: done local thresh, set opt local thresh=%lg\n", 
		   args_rank, synpoint.second(), l_opt.second());
	  }
	  classify_local_channels(l_opt, synpoint, false);
	  local_roundup = VirtualTime::INFINITY;
	}
	local_channel_reclassified = true; 
      }
    }

#ifdef HAVE_MPI_H
    if(epoch_sync) { 
      if(synpoint == global_roundup && !processor_id) {
	if(global_roundup == local_training_length) { // meaning it's a start
	  if(0 >= global_training_length || 
	     local_training_length+global_training_length >= args_endtime) {
	    if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	      printf("[%d] %lg: done global thresh, set default global thresh=%lg\n", 
		     args_rank, synpoint.second(), args_global_thresh.second());
	    }
	    classify_global_channels(args_global_thresh, synpoint, false);
	    global_roundup = VirtualTime::INFINITY;;
	    g_opt = args_global_thresh;
	    training_finished = true;
	  } else {
	    assert(global_training_thresholds.size() > 0);
	    if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	      printf("[%d] %lg: start global thresh, set global thresh[0]=%lg\n", 
		     args_rank, synpoint.second(), global_training_thresholds[0].second());
	    }
	    classify_global_channels(global_training_thresholds[0], synpoint, true);
	    g_idx = 0;
	    global_roundup += global_training_round;
	    t0 = ssf_wallclock_in_nanoseconds();
	  }
	} else {
	  int64 t1 = ssf_wallclock_in_nanoseconds();
	  int64 dt = t1-t0;  t0 = t1;
	  if(!t_g_opt || dt < t_g_opt) {
	    t_g_opt = dt;
	    g_opt = global_training_thresholds[g_idx];
	  }
	  g_idx++;
	  if(g_idx < (int)global_training_thresholds.size()) {
	    if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	      printf("[%d] %lg: continue global thresh, set global thresh[%d]=%lg\n", 
		     args_rank, synpoint.second(), g_idx, global_training_thresholds[g_idx].second());
	    }
	    classify_global_channels(global_training_thresholds[g_idx], synpoint, true);
	    global_roundup += global_training_round;
	  } else {
	    //g_opt = global_training_thresholds[0]; // DEBUG
	    int64 myopt = g_opt.get_ticks(), youropt;
	    ssf_mpi_allreduce(&myopt, &youropt, 1, MPI_LONG_LONG_INT, MPI_MIN, MPI_COMM_WORLD);
	    g_opt.set_ticks(youropt);
	    if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	      printf("[%d] %lg: done global thresh, set opt global thresh=%lg\n", 
		     args_rank, synpoint.second(), g_opt.second());
	    }
	    classify_global_channels(g_opt, synpoint, false);
	    global_roundup = VirtualTime::INFINITY;
	    training_finished = true;
	  }
	}
	global_channel_reclassified = true;
      }
    }
#endif

    ssf_barrier();
    if(global_channel_reclassified || local_channel_reclassified) {
      if(local_channel_reclassified || training_finished) {
	next_decade = synpoint+decade_length; 
	if(next_decade > args_endtime) next_decade = args_endtime;
	if(next_decade > local_roundup) next_decade = local_roundup;
      }
      if(global_channel_reclassified || training_finished) {
	next_epoch = synpoint+epoch_length;
	if(next_epoch > args_endtime) next_epoch = args_endtime;
	if(next_epoch > global_roundup) next_epoch = global_roundup;
      }
      next_window = next_decade;
      if(next_epoch < next_window) next_window = next_epoch;
      classify_timeline_channels();

      if(training_finished) {
	/*
	printf("[%d:%d] %lg: training finished: g_reclass=%d, epoch_length=%lg, next_epoch=%lg, "
	       "l_reclass=%d, decade_length=%lg, next_decade=%lg, next_window=%lg\n",
	       args_rank, processor_id, synpoint.second(), 
	       global_channel_reclassified, epoch_length.second(), next_epoch.second(),
	       local_channel_reclassified, decade_length.second(), next_decade.second(),
	       next_window.second());
	*/
	if(global_binque) {
	  int n;
	  if(args_endtime/1024 > epoch_length) n = 1024;
	  else n = int(args_endtime/epoch_length)+1;
	  if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	    printf("[%d:%d] %lg: settle global binque (epoch_length=%lg, n=%d)\n", 
		   args_rank, processor_id, synpoint.second(), epoch_length.second(), n);
	  }
	  global_binque->settle(epoch_length, n, synpoint);
	}
	if(local_binque) {
	  int n;
	  if(args_endtime/1024 > decade_length) n = 1024;
	  else n = int(args_endtime/decade_length)+1;
	  if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	    printf("[%d:%d] %lg: settle local binque (decade_length=%lg, n=%d)\n", 
		   args_rank, processor_id, synpoint.second(), decade_length.second(), n);
	  }
	  local_binque->settle(decade_length, n, synpoint); 
	}
      }
    }
    ssf_barrier(); // safer to have it

    if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
      printf(">> [%d:%d] composite window: synpoint=%lg, "
	     "next_decade=%lg, next_epoch=%lg (decade=%lg, epoch=%lg)\n", 
	     args_rank, processor_id, synpoint.second(), next_decade.second(), 
	     next_epoch.second(), decade_length.second(), epoch_length.second());
    }

    //synchronize_events();
    
    // we calculate initial lbts for all timelines on this universe
    // and move them onto runnable_timelines (i.e., the ready queue)
    for(SET(Timeline*)::iterator tmln_iter = timelines.begin();
	tmln_iter != timelines.end(); tmln_iter++) {
      Timeline* tmln = *tmln_iter;
      tmln->retrieve_incoming_and_calculate_lowerbound();
      make_timeline_runnable(tmln);
    }

    for(;;) { // loop for the round (the current synchronization)
      // as long as there are timelines waiting to run, we pick the one
      // with the highest priority (minimum simulation clock) and run it
      while(!runnable_timelines.empty()) {
	Timeline* tmln = pace_out_timeline();
	if(!tmln) tmln = (Timeline*)runnable_timelines.deleteMin();
	tmln->retrieve_incoming_and_calculate_lowerbound();
	record_stats_timeline_context_switches();

	VirtualTime clock = tmln->simclock;
	//tmln->retrieve_incoming_and_calculate_lowerbound(); // no need
	while(clock < tmln->lbts) { // there's room for processing events
	  if((args_debug_mask&DEBUG_FLAG_TMSCHED) != 0) {
	    printf(">> [%d:%d] timeline [%d] run (simclock=%lg => lbts=%lg)\n",
		   args_rank, processor_id, tmln->serialno, 
		   tmln->simclock.second(), tmln->lbts.second());
	  }

	  // process events on the timeline until lbts (if it's not paced or interrupted)
	  VirtualTime newclock = tmln->run();
	  assert(newclock == tmln->simclock);
	  if((args_debug_mask&DEBUG_FLAG_TMSCHED) != 0) {
	    printf(">> [%d:%d] timeline [%d] done running (simclock=%lg => lbts=%lg)\n",
		   args_rank, processor_id, tmln->serialno,
		   newclock.second(), tmln->lbts.second());
	  }

	  if(clock < newclock) // if time has advanced, we propagate the new time
	    tmln->update_subsequent_timelines();
	  clock = newclock;
	  if(clock < tmln->lbts) break; // if the timeline is being paced or interrupted, we break out

	  handle_io_events(false); // handle i/o events, no blocking
	  tmln->retrieve_incoming_and_calculate_lowerbound();
	}

	if(clock == tmln->lbts) { // the timeline is not paced or interrupted
	  if(clock < next_window) block_timeline(tmln); // if it's not done, we put it in the wait queue (waiting for lbts to advance)
	  else { // it is done for this window
	    if(next_window >= args_endtime) { // if there's no tomorrow (no more windows)
	      tmln->state = Timeline::STATE_DONE;
	      if((args_debug_mask&DEBUG_FLAG_TMSCHED) != 0) {
		printf(">> [%d:%d] timeline [%d] DONE (simclock=%lg, lbts=%lg)\n",
		       args_rank, processor_id, tmln->serialno, 
		       tmln->simclock.second(), tmln->lbts.second());
	      }
	    } else {
	      tmln->state = Timeline::STATE_ROUND; // done with this round
	      if((args_debug_mask&DEBUG_FLAG_TMSCHED) != 0) {
		printf(">> [%d:%d] timeline [%d] ROUND (simclock=%lg, lbts=%lg)\n",
		       args_rank, processor_id, tmln->serialno, 
		       tmln->simclock.second(), tmln->lbts.second());
	      }
	    }
	  }
	}
      }

      // at this point, we don't have any runnable timeline
      if(!blocked_timelines.empty() || !paced_timelines.empty()) {
	// if there are still blocked or paced timelines
	while(runnable_timelines.empty()) 
	  handle_io_events(true); // handle i/o events, blocking
      } else {
	synpoint = next_window;
	if(synpoint == next_decade) {
	  next_decade += decade_length; 
	  if(next_decade > args_endtime) next_decade = args_endtime;
	  if(next_decade > local_roundup) next_decade = local_roundup;
	  decade_sync = true;
	} else decade_sync = false;
	if(synpoint == next_epoch) {
	  next_epoch += epoch_length; 
	  if(next_epoch > args_endtime) next_epoch = args_endtime;
	  if(next_epoch > global_roundup) next_epoch = global_roundup;
	  epoch_sync = true;
	} else epoch_sync = false;
	next_window = next_decade;
	if(next_epoch < next_window) next_window = next_epoch;
	break;
      }
    }
  }

  // wait for ALL universes to complete their running phrase and then
  // we can move onto the finalizing phase
  ssf_barrier();

  if(global_binque) delete global_binque;
  if(local_binque) delete local_binque;

  if(!processor_id) {
    sim_state = SIM_STATE_FINALIZING; 

#ifdef HAVE_MPI_H
    if(args_nmachs > 1) {
      // send a special event to terminate the writer thread (the
      // writer thread will terminate the reader thread subsequently)
      transport_terminal_message();

      // wait until the threads are finished
      if(mpi_thread_support == MPI_THREAD_MULTIPLE) {
	ssf_thread_join(&reader_thread_id);
	ssf_thread_join(&writer_thread_id);
      } else return; // this is end of the new thread
    }
#endif
  }
  run_end();
}

void Universe::run_end()
{
#ifdef HAVE_MPI_H
  if(args_nmachs > 1 && !processor_id) {
    if(mpi_thread_support != MPI_THREAD_MULTIPLE) 
      ssf_thread_join(&rw_thread_id);
    for(int i=0; i<args_nprocs; i++) delete[] sndcnt[i];
    delete[] sndcnt;
    delete[] rscnt; // used by mpi reduce scatter
  }
#endif
  ssf_barrier();
}

VirtualTime Universe::get_wallclock_time() 
{
  // convert from wall clock time to simulation time (with appropriate
  // speedup ratio); if the speedup is zero (which means infinity),
  // the simulation time ought to be infinity
  if(args_speedup > 0) {
    return VirtualTime((ssf_wallclock_in_nanoseconds()-start_wallclock_time)*args_speedup);
  } else return VirtualTime::INFINITY;
}

void Universe::make_timeline_runnable(Timeline* tmln) 
{
  assert(tmln->state == Timeline::STATE_START || 
	 tmln->state == Timeline::STATE_RUNNING || // this is possible due to time slicing
	 tmln->state == Timeline::STATE_ROUND || 
	 tmln->state == Timeline::STATE_WAITING);
  if(tmln->simclock < tmln->lbts) {
    if(tmln->state == Timeline::STATE_WAITING)
      blocked_timelines.erase(tmln);
    tmln->state = Timeline::STATE_RUNNING;
    tmln->setTime(tmln->next_emulation_due_time()); // set the priority here!
    runnable_timelines.insert(tmln);
    if((args_debug_mask&DEBUG_FLAG_TMSCHED) != 0) {
      printf(">> [%d:%d] timeline [%d] ready (simclock=%lg, lbts=%lg)\n",
	     args_rank, processor_id, tmln->serialno, 
	     tmln->simclock.second(), tmln->lbts.second());
    }
  } else if(tmln->state != Timeline::STATE_WAITING) {
    tmln->state = Timeline::STATE_WAITING;
    blocked_timelines.insert(tmln);
    if((args_debug_mask&DEBUG_FLAG_TMSCHED) != 0) {
      printf(">> [%d:%d] timeline [%d] (readily) blocked (simclock=%lg, lbts=%lg)\n",
	     args_rank, processor_id, tmln->serialno, 
	     tmln->simclock.second(), tmln->lbts.second());
    }
  }
}

void Universe::block_timeline(Timeline* tmln) 
{
  assert(tmln->state == Timeline::STATE_RUNNING);
  tmln->state = Timeline::STATE_WAITING;
  blocked_timelines.insert(tmln);
  if((args_debug_mask&DEBUG_FLAG_TMSCHED) != 0) {
    printf(">> [%d:%d] timeline [%d] blocked (simclock=%lg, lbts=%lg)\n",
	   args_rank, processor_id, tmln->serialno, 
	   tmln->simclock.second(), tmln->lbts.second());
  }
}

bool Universe::pace_in_timeline(Timeline* tmln, VirtualTime untiltime)
{
  assert(tmln->state == Timeline::STATE_RUNNING);
  VirtualTime t = get_wallclock_time();
  if(t < untiltime) {
    tmln->state = Timeline::STATE_PACING;
    tmln->setTime(untiltime); // set the priority here!
    paced_timelines.insert(tmln);
    if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
      printf(">> [%d:%d] timeline [%d] paced (simclock=%lg, wallclock=%lg, lbts=%lg, until=%lg)\n",
	     args_rank, processor_id, tmln->serialno, tmln->simclock.second(),
	     t.second(), tmln->lbts.second(), untiltime.second());
    }
    record_stats_timeline_pacing();
    return true; // meaning the timeline is paced
  } else return false; // meaning it is not paced
}

Timeline* Universe::pace_out_timeline()
{
  if(!paced_timelines.empty()) {
    Timeline* tmln = (Timeline*)paced_timelines.getMin();
    VirtualTime t = get_wallclock_time();
    if(tmln->time() <= t) {
      paced_timelines.deleteMin();
      tmln->state = Timeline::STATE_RUNNING;
      return tmln;
    }
  }
  return 0;
}

#ifdef HAVE_MPI_H
void Universe::transport_message(ChannelEvent* evt)
{
  // an event needs to be sent out from a universe on this machine; we
  // deposit the event into the remote mailbox and notify the write
  // thread (if necessary)
  assert(evt && !evt->inport);
  assert(evt->stargate);
  // if the event is a null message, or if the event is not a null
  // message and the time is within the simulation end time, we count it
  if(!evt->event || (evt->event && evt->time() < args_endtime)) {
    int rank = timeline_to_machine(evt->stargate->target_timeline_id);
    sndcnt[processor_id][rank]++;
    /*
    if(evt->event && evt->stargate->in_sync) 
      printf("%d [%lg] => send sync event at %lg\n", args_rank, synpoint.second(), VirtualTime(evt->time()).second());
    else if(evt->event && !evt->stargate->in_sync) 
      printf("%d [%lg] => send async regular event at %lg\n", args_rank, synpoint.second(), VirtualTime(evt->time()).second());
    else printf("%d [%lg] => send null event at %lg\n", args_rank, synpoint.second(), VirtualTime(evt->time()).second());
    */
  }

  ssf_thread_mutex_lock(&remote_mailbox_mutex);
  if(!remote_mailbox) ssf_thread_cond_signal(&remote_mailbox_cond);
  evt->append_to_list(&remote_mailbox, &remote_mailbox_tail);
  ssf_thread_mutex_unlock(&remote_mailbox_mutex);
}

void Universe::transport_terminal_message()
{
  ChannelEvent* evt = new ChannelEvent(Timestamp(0,0,0), 0, (int)0); // terminal
  ssf_thread_mutex_lock(&remote_mailbox_mutex);
  if(!remote_mailbox) ssf_thread_cond_signal(&remote_mailbox_cond);
  evt->append_to_list(&remote_mailbox, &remote_mailbox_tail);
  ssf_thread_mutex_unlock(&remote_mailbox_mutex);
}

void Universe::transport_reduce_message()
{
  assert(!processor_id);

  // processor 0 adds up the number of messages sent to each remote machine
  for(int i=1; i<args_nprocs; i++) {
    for(int j=0; j<args_nmachs; j++) {
      sndcnt[0][j] += sndcnt[i][j];
      sndcnt[i][j] = 0;
    }
  }

  if(mpi_thread_support != MPI_THREAD_MULTIPLE) {
    //printf("before %d: rcvcnt=%lld, rcvcnt_target=%lld\n", args_rank, rcvcnt, rcvcnt_target); fflush(0);
    ChannelEvent* evt = new ChannelEvent(Timestamp(0,0,0), 0, (int)1); // reduce
    ssf_thread_mutex_lock(&remote_mailbox_mutex);
    if(!remote_mailbox) ssf_thread_cond_signal(&remote_mailbox_cond);
    evt->append_to_list(&remote_mailbox, &remote_mailbox_tail);
    scatter_reduce_carry_on = false;
    ssf_thread_mutex_unlock(&remote_mailbox_mutex);
  
    ssf_thread_mutex_lock(&rcvcnt_mutex);
    while(!scatter_reduce_carry_on) 
      ssf_thread_cond_wait(&rcvcnt_cond, &rcvcnt_mutex); //XXX
    ssf_thread_mutex_unlock(&rcvcnt_mutex);
    //printf("after %d: rcvcnt=%lld, rcvcnt_target=%lld\n", args_rank, rcvcnt, rcvcnt_target); fflush(0);
  } else {
    int64 to_recv;
    ssf_mpi_reduce_scatter(sndcnt[0], &to_recv, rscnt, MPI_LONG_LONG_INT, MPI_SUM, MPI_COMM_WORLD);
  
    // if the reader thread has not received the expected number
    // of events, processor 0 needs to wait
    ssf_thread_mutex_lock(&rcvcnt_mutex);
    rcvcnt_target = to_recv;
    //printf("%d:%d: rcvcnt=%lld, rcvcnt_target=%lld\n", args_rank, processor_id, rcvcnt, rcvcnt_target);
    while(rcvcnt < rcvcnt_target) //XXX
      ssf_thread_cond_wait(&rcvcnt_cond, &rcvcnt_mutex);
    ssf_thread_mutex_unlock(&rcvcnt_mutex);
  }
}

void Universe::transport_barrier_message()
{
  assert(!processor_id);
  
  if(mpi_thread_support != MPI_THREAD_MULTIPLE) {
    ChannelEvent* evt = new ChannelEvent(Timestamp(0,0,0), 0, (int)2); // barrier
    ssf_thread_mutex_lock(&remote_mailbox_mutex);
    if(!remote_mailbox) ssf_thread_cond_signal(&remote_mailbox_cond);
    evt->append_to_list(&remote_mailbox, &remote_mailbox_tail);
    barrier_carry_on = false;
    ssf_thread_mutex_unlock(&remote_mailbox_mutex);

    ssf_thread_mutex_lock(&rcvcnt_mutex);
    while(!barrier_carry_on)
      ssf_thread_cond_wait(&bar_cond, &rcvcnt_mutex); //XXX
    ssf_thread_mutex_unlock(&rcvcnt_mutex);
  } else {
    ssf_mpi_barrier(MPI_COMM_WORLD);
  }
}

void Universe::handle_incoming_events(int rbfsz)
{
  // unpack the channel events and deliver them, one at a time
  int pos = 0;
  while(pos < rbfsz) {
    ChannelEvent* evt = ChannelEvent::unpack(MPI_COMM_WORLD, recvbuf, pos, rbfsz);
    assert(evt);

    MAP(int,VECTOR(PAIR(MapInport*,Stargate*))*)::iterator iter = 
      starmap.find(evt->outportno);
    assert(iter != starmap.end());
      
    // deliver a copy of the event to each timeline target
    VECTOR(PAIR(MapInport*,Stargate*))* vec = (*iter).second;
    assert(vec->size() > 0);
    for(int i=0; i<(int)vec->size(); i++) {
      // all except the last one uses a cloned event
      ChannelEvent* myevt;
      if(i+1 < (int)vec->size()) myevt = new ChannelEvent
	   (evt->time(), evt->event?evt->event->clone():0, evt->outportno);
      else myevt = evt;

      myevt->inport = vec->at(i).first;
      myevt->stargate = vec->at(i).second;

      // if the event is a regular channel event that goes through
      // asynchronous channel, we send it to the mailbox at the
      // corresponding stargate (via send_message); if this is a
      // null message or if the channel is synchronous, we send it
      // to the mailbox at the target universe
      assert(myevt->stargate);
      if(myevt->event && !myevt->stargate->in_sync) {
	//printf("%d => async regular event\n", args_rank);
	myevt->stargate->send_message(myevt);
      } else {
	//if(!myevt->event) printf("%d => null event\n", args_rank); else printf("%d => sync event\n", args_rank);
	Universe* univ = myevt->stargate->target_timeline->universe;
	ssf_thread_mutex_lock(&univ->mailbox_mutex);
	if(!univ->mailbox) ssf_thread_cond_signal(&univ->mailbox_cond);
	myevt->append_to_list(&univ->mailbox, &univ->mailbox_tail);
	ssf_thread_mutex_unlock(&univ->mailbox_mutex);
      }
    }

    ssf_thread_mutex_lock(&rcvcnt_mutex);
    //printf("%d => [%lld..%lld] recv evt at %lg\n", args_rank, rcvcnt, rcvcnt_target, VirtualTime(evt->time()).second());
    rcvcnt++;
    if(rcvcnt == rcvcnt_target) {
      scatter_reduce_carry_on = true;
      ssf_thread_cond_signal(&rcvcnt_cond);
    }
    ssf_thread_mutex_unlock(&rcvcnt_mutex);
  }
  assert(pos == rbfsz);
}

void Universe::insert_emulated_event(Timeline* tmln, EmulatedEvent* myevt)
{
  Universe* univ = tmln->universe;
  ssf_thread_mutex_lock(&univ->mailbox_mutex);
  if(!univ->mailbox) ssf_thread_cond_signal(&univ->mailbox_cond);
  myevt->append_to_list(&univ->mailbox, &univ->mailbox_tail);
  ssf_thread_mutex_unlock(&univ->mailbox_mutex);
}

bool Universe::handle_outgoing_events(ChannelEvent* evt)
{
  SET(int) rankset; // contains the ranks of remote machines that we'll send messages to

  // if the batch of events contain a terminating event (with a null
  // 'stargate' pointer), this boolean will set to true; nullevt and
  // nullevt_tail store all the null events in this batch of events
  // as a linked list
  bool finished = false;

  ChainedEvent* nullevt = 0;
  ChainedEvent* nullevt_tail = 0;
  bool do_reduce_scatter = false;
  bool do_barrier = false;
  while(evt) {
    if(!evt->stargate) {
      if(evt->outportno == 0) { // terminal
	// if the main thread wants the writer thread to quit, we flag
	// it for the moment
	finished = true;
      } else if(evt->outportno == 1) { // reduce scatter
	do_reduce_scatter = true;
      } else { // barrier
	do_barrier = true;
      }
      ChannelEvent* nxt = (ChannelEvent*)evt->get_next_event();
      delete evt;
      evt = nxt;
    } else if(!evt->event) { 
      //printf("%d => null event!\n", args_rank); fflush(0);
      // if this is a null event, we put it in the linked list for
      // now; we'll send them later
      ChannelEvent* nxt = (ChannelEvent*)evt->get_next_event();
      evt->append_to_list(&nullevt, &nullevt_tail);
      evt = nxt;
    } else if(args_endtime <= evt->time()) { 
      //printf("%d => regular event beyond end time!\n", args_rank); fflush(0);
      // if a regular event is beyond the simulation end time,
      // there's no need to send it, we simply delete it
      ChannelEvent* nxt = (ChannelEvent*)evt->get_next_event();
      delete evt;
      evt = nxt;
    } else {
      //printf("%d => regular event!\n", args_rank); fflush(0);
      // for any other regular event, we find the target machine
      // rank, and pack the event into the send buffer
      assert(!evt->stargate->target_timeline);
      int rank = timeline_to_machine(evt->stargate->target_timeline_id);
      assert(rank != args_rank);
      rankset.insert(rank);
      if(!sendbuf[rank]) { // create send buffer on demand
	sendbuf[rank] = new char[MPIBUF_SIZE]; assert(sendbuf[rank]);
	sendpos[rank] = 0;
      }
      evt->pack(MPI_COMM_WORLD, sendbuf[rank], sendpos[rank], MPIBUF_SIZE);

      ChannelEvent* nxt = (ChannelEvent*)evt->get_next_event();
      delete evt;
      evt = nxt;

      // if the buffer is more than half full, we send the mpi message
      if(sendpos[rank] > MPIBUF_THRESHOLD) {
	record_stats_mpi_sent_messages(sendpos[rank]);
	if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
	  printf(">> [%d] send %d bytes to %d [c=%lu,b=%lu]\n", args_rank, 
		 sendpos[rank], rank, stats_mpi_sent_messages, stats_mpi_sent_bytes);
	}
	ssf_mpi_bsend(sendbuf[rank], sendpos[rank], MPI_PACKED, rank, 
		      CHANNEL_EVENT_TAG, MPI_COMM_WORLD);
	sendpos[rank] = 0; // reset to the beginning of the buffer
	rankset.erase(rank);
      }
    }
  }

  while(nullevt) {
    // for each null event, we find its target machine rank, and
    // pack the event into the send buffer
    assert(!((ChannelEvent*)nullevt)->stargate->target_timeline);
    int rank = timeline_to_machine(((ChannelEvent*)nullevt)->stargate->target_timeline_id);
    assert(rank != args_rank);
    rankset.insert(rank);
    if(!sendbuf[rank]) {
      sendbuf[rank] = new char[MPIBUF_SIZE]; assert(sendbuf[rank]);
      sendpos[rank] = 0;
    }
    //printf("%d: PACKING for sending null event to rank %d\n", args_rank, rank);
    ((ChannelEvent*)nullevt)->pack(MPI_COMM_WORLD, sendbuf[rank], sendpos[rank], MPIBUF_SIZE);

    ChannelEvent* nxt = (ChannelEvent*)nullevt->get_next_event();
    delete nullevt;
    nullevt = nxt;

    // if the buffer is more than half full, we send the mpi message
    if(sendpos[rank] > MPIBUF_THRESHOLD) {
      record_stats_mpi_sent_messages(sendpos[rank]);
      if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
	printf(">> [%d] send %d bytes to %d [c=%lu,b=%lu]\n", args_rank, 
	       sendpos[rank], rank, stats_mpi_sent_messages, stats_mpi_sent_bytes);
      }
      ssf_mpi_bsend(sendbuf[rank], sendpos[rank], MPI_PACKED, rank, 
		    CHANNEL_EVENT_TAG, MPI_COMM_WORLD);
      sendpos[rank] = 0; // reset to the beginning of the buffer
      rankset.erase(rank);
    }
  }

  // for each send buffer with packed events, we send the mpi message
  for(SET(int)::iterator iter = rankset.begin(); 
      iter != rankset.end(); iter++) {
    int rank = *iter;
    assert(sendpos[rank] > 0);
    record_stats_mpi_sent_messages(sendpos[rank]);
    if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
      printf(">> [%d] send %d bytes to %d [c=%lu,b=%lu]\n", args_rank, 
	     sendpos[rank], rank, stats_mpi_sent_messages, stats_mpi_sent_bytes);
    }
    ssf_mpi_bsend(sendbuf[rank], sendpos[rank], MPI_PACKED, rank, 
		  CHANNEL_EVENT_TAG, MPI_COMM_WORLD);
    sendpos[rank] = 0; // reset to the beginning of the buffer
  }
  rankset.clear();

  if(do_reduce_scatter) {
    // if the main thread wants to do a reduce scatter (this only
    // happens when we use a single r/w thread)
    //printf("%d => about to reduce scatter!\n", args_rank); fflush(0);
    int64 to_recv;
    ssf_mpi_reduce_scatter(sndcnt[0], &to_recv, rscnt, MPI_LONG_LONG_INT, 
			   MPI_SUM, MPI_COMM_WORLD);
    
    ssf_thread_mutex_lock(&rcvcnt_mutex);
    rcvcnt_target = to_recv; assert(rcvcnt <= rcvcnt_target);
    if(rcvcnt == rcvcnt_target) {
      //printf("SIGNALED!!!\n"); fflush(0);
      scatter_reduce_carry_on = true;
      ssf_thread_cond_signal(&rcvcnt_cond); //XXX
    }
    ssf_thread_mutex_unlock(&rcvcnt_mutex);
    //printf("%d => [%lld..%lld] done reduce scatter at %lg\n", args_rank, rcvcnt, rcvcnt_target, VirtualTime(evt->time()).second()); fflush(0);
  }

  if(do_barrier) {
    //printf("%d => about to barrier!\n", args_rank); fflush(0);
    ssf_mpi_barrier(MPI_COMM_WORLD);
	
    ssf_thread_mutex_lock(&rcvcnt_mutex);
    barrier_carry_on = true;
    ssf_thread_cond_signal(&bar_cond); //XXX
    ssf_thread_mutex_unlock(&rcvcnt_mutex);
    //printf("%d => done barrier!\n", args_rank); fflush(0);
  }

  return finished;
}

void Universe::reader_thread()
{
  // the receive buffer has been created when this thread is running
  for(;;) {
    // blocking receive
    MPI_Status status;
    ssf_mpi_recv(recvbuf, MPIBUF_SIZE, MPI_PACKED, MPI_ANY_SOURCE, 
		 CHANNEL_EVENT_TAG, MPI_COMM_WORLD, &status);

    // if we receive a message from the same machine, it indicates
    // that the reader thread should terminate by now (it's a hack)
    if(status.MPI_SOURCE == args_rank) {
      if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
	printf(">> [%d] reader thread has been instructed to end\n", args_rank);
      }
      break;
    }

    // check incoming message size
    int rbfsz;
    ssf_mpi_get_count(&status, MPI_PACKED, &rbfsz);

    record_stats_mpi_rcvd_messages(rbfsz);
    if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
      printf(">> [%d] receive %d bytes from %d [c=%lu,b=%lu]\n", args_rank,
	     rbfsz, status.MPI_SOURCE, stats_mpi_rcvd_messages, stats_mpi_rcvd_bytes);
    }
    handle_incoming_events(rbfsz);
  }

  // reclaim the receive buffer
  delete[] recvbuf;
}

void Universe::writer_thread()
{
  SET(int) rankset; // contains the ranks of remote machines that we'll send messages to
  for(;;) {
    // wait on the conditional variable, until there are one or more
    // events have been deposited in the remote mailbox
    ssf_thread_mutex_lock(&remote_mailbox_mutex);
    while(!remote_mailbox)
      ssf_thread_cond_wait(&remote_mailbox_cond, &remote_mailbox_mutex);
    ChannelEvent* evt = (ChannelEvent*)remote_mailbox; 
    remote_mailbox = remote_mailbox_tail = 0;
    ssf_thread_mutex_unlock(&remote_mailbox_mutex);

    bool finished = handle_outgoing_events(evt);
    if(finished) {
      // if the main thread wants to terminate, first inform the
      // reader thread to finish by sending an mpi message (a packed
      // dummy integer) to the same machine, and then break out
      char mybuf[128]; int dummy = 0; int pos = 0;
      ssf_mpi_pack(&dummy, 1, MPI_INT, mybuf, 128, &pos, MPI_COMM_WORLD);
      ssf_mpi_bsend(mybuf, pos, MPI_PACKED, args_rank, CHANNEL_EVENT_TAG, MPI_COMM_WORLD);
      if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
	printf(">> [%d] writer thread informing reader thread to end (size=%d)\n", 
	       args_rank, pos);
      }
      break; // we are done!
    }
  }

  // reclaim the send buffers and write positions
  for(int i=0; i<args_nmachs; i++) 
    if(sendbuf[i]) delete[] sendbuf[i];
  delete[] sendbuf;
  delete[] sendpos;
  sendbuf = 0;
  int attachsiz; char* attachbuf;
  ssf_mpi_buffer_detach(&attachbuf, &attachsiz);
  assert(attachsiz==MPIBUF_ATTACHED);
  delete[] attachbuf;
}

void Universe::rw_thread()
{
  SET(int) rankset; // contains the ranks of remote machines that we'll send messages to

  /*
  MPI_Request request;
  if(MPI_Irecv(recvbuf, MPIBUF_SIZE, MPI_PACKED, MPI_ANY_SOURCE, 
	       CHANNEL_EVENT_TAG, MPI_COMM_WORLD, &request))
    SSF_THROW("unable to post irecv");
  */

  bool finished = false;
  while(!finished) {
    //printf("*"); fflush(0);
    //ssf_thread_yield();

    int flag;
    MPI_Status status;
    ssf_mpi_iprobe(MPI_ANY_SOURCE, CHANNEL_EVENT_TAG, MPI_COMM_WORLD, &flag, &status);
    /*
    if(MPI_Test(&request, &flag, &status)) SSF_THROW("unable to mpi test");
    */
    if(flag) {
      // check incoming message size
      int rbfsz;
      ssf_mpi_get_count(&status, MPI_PACKED, &rbfsz);
      if(rbfsz > MPIBUF_SIZE) SSF_THROW("increase MPIBUF_SIZ to receive jumbo message: msgsiz=" << rbfsz);

      ssf_mpi_recv(recvbuf, rbfsz/*MPIBUF_SIZE*/, MPI_PACKED, status.MPI_SOURCE/*MPI_ANY_SOURCE*/, 
		   status.MPI_TAG/*CHANNEL_EVENT_TAG*/, MPI_COMM_WORLD, &status);
      // if we receive a message from the same machine, it indicates
      // that the reader thread should terminate by now (it's a hack)
      if(status.MPI_SOURCE == args_rank) {
	if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
	  printf(">> [%d] reader thread has been instructed to end\n", args_rank);
	}
	break;
      }
#ifndef NDEBUG
      // WE ASSUME (ACCORDING TO MANPAGE) THE RECEIVED MESSAGE IS THE ONE PROBED
      int t; ssf_mpi_get_count(&status, MPI_PACKED, &t); 
      if(t != rbfsz) SSF_THROW("internal error: invalid iprobe/recv sequence");
#endif

      record_stats_mpi_rcvd_messages(rbfsz);
      if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
	printf(">> [%d] receive %d bytes from %d [c=%lu,b=%lu]\n", args_rank,
	       rbfsz, status.MPI_SOURCE, stats_mpi_rcvd_messages, stats_mpi_rcvd_bytes);
      }
      handle_incoming_events(rbfsz);
      
      /*
      if(MPI_Irecv(recvbuf, MPIBUF_SIZE, MPI_PACKED, MPI_ANY_SOURCE, 
		   CHANNEL_EVENT_TAG, MPI_COMM_WORLD, &request))
	SSF_THROW("unable to post irecv");
      */
    } else { 
      // wait on the conditional variable, until there are one or more
      // events have been deposited in the remote mailbox
      struct timeval tv;
      gettimeofday(&tv, 0);
      struct timespec ts;
      ts.tv_sec = tv.tv_sec;
      ts.tv_nsec = tv.tv_usec*1000+100000; // 0.1 ms
      if(ts.tv_nsec > 1000000000) {
	ts.tv_nsec -= 1000000000;
	ts.tv_sec++;
      }
      ssf_thread_mutex_lock(&remote_mailbox_mutex);
      int rc = 0;
      while(!remote_mailbox && !rc)
	rc = ssf_thread_cond_timedwait(&remote_mailbox_cond, &remote_mailbox_mutex, &ts);
      ChannelEvent* evt = (ChannelEvent*)remote_mailbox; 
      if(evt) remote_mailbox = remote_mailbox_tail = 0;
      ssf_thread_mutex_unlock(&remote_mailbox_mutex);

      if(evt) finished = handle_outgoing_events(evt);
    }
  }

  // reclaim the send buffers and write positions, and the receive buffer
  for(int i=0; i<args_nmachs; i++) 
    if(sendbuf[i]) delete[] sendbuf[i];
  delete[] sendbuf;
  delete[] sendpos;
  sendbuf = 0;
  int attachsiz; char* attachbuf;
  ssf_mpi_buffer_detach(&attachbuf, &attachsiz);
  assert(attachsiz==MPIBUF_ATTACHED);
  delete[] attachbuf;
  delete[] recvbuf;
}
#endif

void Universe::handle_io_events(bool blocking)
{
  // if blocking is true, we'll wait on the conditional variable (that
  // is, until someone deposits one or more events into the mailbox);
  // if blocking is false, we simply 'poll' the events from the
  // mailbox (which could be empty)
  if((args_debug_mask&DEBUG_FLAG_TMSCHED) != 0) {
    printf(">> [%d:%d] handle_io_events(blocking=%s)\n", args_rank, 
	   processor_id, blocking?"true":"false");
  }

  record_stats_handle_io_events();
  if(blocking) {
    if(paced_timelines.empty()) {
      //printf("empty\n");
      ssf_thread_mutex_lock(&mailbox_mutex);
      while(!mailbox) 
	ssf_thread_cond_wait(&mailbox_cond, &mailbox_mutex);
    } else {
      Timeline* tmln = (Timeline*)paced_timelines.getMin();
      VirtualTime t2 = tmln->time();
      VirtualTime t1 = get_wallclock_time();
      //printf("paced timeline real=%lld, now=%lld\n", t1.get_ticks(), t2.get_ticks());
      if(t2 <= t1) { // a paced timeline is ready
	//printf("ready\n");
	paced_timelines.deleteMin();
	tmln->state = Timeline::STATE_RUNNING;
	tmln->setTime(tmln->next_emulation_due_time()); // set the priority here!
	runnable_timelines.insert(tmln);
	ssf_thread_mutex_lock(&mailbox_mutex);
      } else {
	//struct timeval tv;
	//gettimeofday(&tv, 0);
	//printf("wait now   %ld %09d\n", tv.tv_sec, tv.tv_usec*1000);

	int64 ticks = start_wallclock_time+t2.get_ticks();
	struct timespec ts;
	ts.tv_sec = ticks/1000000000;
	ts.tv_nsec = ticks-ts.tv_sec*1000000000;
	//printf("wait until %ld %09ld %lld\n", ts.tv_sec, ts.tv_nsec, ticks);
	ssf_thread_mutex_lock(&mailbox_mutex);
	int rc = 0;
	while(!mailbox && !rc)
	  rc = ssf_thread_cond_timedwait(&mailbox_cond, &mailbox_mutex, &ts);
      }
    }
  } else ssf_thread_mutex_lock(&mailbox_mutex);
  ChainedEvent* evt = mailbox; 
  mailbox = mailbox_tail = 0;
  ssf_thread_mutex_unlock(&mailbox_mutex);

  // handle the null events
  while(evt) {
    ChainedEvent* nxt = evt->get_next_event();
    if(evt->is_channel_event()) {
      ChannelEvent* chevt = (ChannelEvent*)evt;
      assert(chevt->stargate); 
      assert(chevt->stargate->target_timeline->universe == this);
      //assert(!chevt->event || chevt->stargate->in_sync); // must be null message or the event is supposed going through synchronous channel
      if(!chevt->event) {
	chevt->stargate->set_time(chevt->time(), false);
	delete chevt;
      } else {
	assert(!chevt->stargate->source_timeline);
	chevt->stargate->target_timeline->insert_event(chevt);
      }
    } else {
      EmulatedEvent* eevt = (EmulatedEvent*)evt;
      Timeline* tmln = eevt->entity->timeline;
      VirtualTime herenow = get_wallclock_time();
      // this means out of order arrival of emulated events; in other
      // words, simulation is getting ahead of the real time since
      // simulated events are not pinned down to real time; to avoid
      // this, one has to set a pacing timer with a good enough
      // resolution resolution (that'll be the max time an emulated
      // event could be artificially delayed for this)
      if(herenow < tmln->simclock) herenow = tmln->simclock; 
      eevt->setTime(Timestamp(herenow, eevt->entity->serialno, 
			      eevt->entity->get_next_event_id()));
      tmln->insert_event(eevt);
      tmln->setTime(tmln->next_emulation_due_time());
      /* will be paced out in the next round if it is */
    }
    evt = nxt;
  }
}

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
