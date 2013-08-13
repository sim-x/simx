#include <algorithm>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "kernel/universe.h"
#include "ssf.h"

namespace minissf {

unsigned long Universe::stats_mpi_sent_messages = 0;
unsigned long Universe::stats_mpi_sent_bytes = 0;
unsigned long Universe::stats_mpi_rcvd_messages = 0;
unsigned long Universe::stats_mpi_rcvd_bytes = 0;

VECTOR(VirtualTime) Universe::global_training_thresholds;
VECTOR(VirtualTime) Universe::local_training_thresholds;
VirtualTime Universe::global_training_length;
VirtualTime Universe::local_training_length;
VirtualTime Universe::global_training_round;
VirtualTime Universe::local_training_round;
VirtualTime Universe::global_roundup;
VirtualTime Universe::local_roundup;
bool Universe::global_channel_reclassified;
bool Universe::local_channel_reclassified;
bool Universe::training_finished;

#define MAX_GLOBAL_TRAINING_ROUNDS 10
#define MAX_LOCAL_TRAINING_ROUNDS 10

// block partitioning for timelines on each processor
#define BLOCK_OWNER(j,p,n) (((p)>(n))?(j):(((p)*((j)+1)-1)/(n)))

Universe** Universe::parallel_universe = 0;
int Universe::sim_state = Universe::SIM_STATE_UNINITIALIZED;

void Universe::global_init()
{
  time0 = time1 = ssf_wallclock_in_nanoseconds();

  sim_state = SIM_STATE_INITIALIZING;
  parallel_universe = new Universe*[args_nprocs];
  assert(parallel_universe);
  ssf_barrier_init();
  Random::global_seed = args_seed;

  if(!args_rank) { // only first machine print the copyright info
    if((args_debug_mask&DEBUG_FLAG_BRIEF) != 0) {
      printf("<><><> " PACKAGE_STRING " <><><>\n");
      printf("[] Modeling and Networking Systems Research Group\n");
      printf("[] School of Computing and Information Sciences\n");
      printf("[] Florida International University\n");
      printf("[] http://www.primessf.net/minissf/\n");
    }
  }

  if((args_debug_mask&DEBUG_FLAG_REPORT) != 0) {
    if(args_nmachs > 1) { // print machine info only if running on multiple machines
      char myhostname[256];
      if(!gethostname(myhostname, 255))
	printf("[ MACH #%d: HOSTNAME=%s, NPROCS=%d ]\n", args_rank, myhostname, args_nprocs);
    }
  }

  if(!args_rank && (args_debug_mask&DEBUG_FLAG_BRIEF) != 0) {
    printf("[ TOTAL MACHINES: %d ]\n", args_nmachs);
    printf("[ TOTAL PARALLELISM: %d ]\n", ssf_total_num_processors());
    if(args_global_thresh_set)
      printf("[ GLOBAL THRESHOLD: %lg (s) ]\n", args_global_thresh.second());
    if(args_local_thresh_set)
      printf("[ LOCAL THRESHOLD (on 1st machine): %lg (s) ]\n", args_local_thresh.second());
  }    

  /*
  if((args_debug_mask&DEBUG_FLAG_REPORT) != 0) {
    printf("[%d] global init:\n", args_rank);
    printf("[%d]   args_nmachs = %d\n", args_rank, args_nmachs);
    printf("[%d]   args_mach_nprocs =", args_rank); 
    for(int i=0; i<args_nmachs; i++) printf(" %d", args_mach_nprocs[i]); printf("\n");
    printf("[%d]   args_nprocs = %d\n", args_rank, args_nprocs);
    printf("[%d]   args_seed = %d\n", args_rank, args_seed);
    printf("[%d]   args_debug_mask = 0x%08x\n", args_rank, args_debug_mask);
    printf("[%d]   args_outfile = \"%s\"\n", args_rank, args_outfile.c_str());
    printf("[%d]   args_progress_interval = %lg\n", args_rank, args_progress_interval.second());
    printf("[%d]   args_global_thresh = %lg\n", args_rank, args_global_thresh.second());
    printf("[%d]   args_local_thresh = %lg\n", args_rank, args_local_thresh.second());
  }
  */

  // if there are more than one machine or more than one processor on
  // this machine, we'd use switch board to redistribute events during
  // composite synchronization barrier
  if(args_nprocs > 1) {
    switch_board = new ChannelEvent**[args_nprocs]; assert(switch_board);
    for(int i=0; i<args_nprocs; i++) {
      switch_board[i] = new ChannelEvent*[args_nprocs];
      assert(switch_board[i]);
      memset(switch_board[i], 0, sizeof(ChannelEvent*)*args_nprocs);
    }
  }
}

void Universe::local_init() 
{
  /*
  if((args_debug_mask&DEBUG_FLAG_REPORT) != 0) {
    ssf_barrier();
    for(int p=0; p<args_nprocs; p++) {
      if(p == processor_id) {
	printf("[%d:%d] local init (on processor %d):\n", args_rank, processor_id, processor_id);
	printf("[%d:%d]   ssf_num_machines() = %d\n", args_rank,  processor_id, ssf_num_machines());
	printf("[%d:%d]   ssf_machine_index() = %d\n", args_rank,  processor_id, ssf_machine_index());
	printf("[%d:%d]   ssf_num_processors() = %d\n", args_rank,  processor_id, ssf_num_processors());
	printf("[%d:%d]   ssf_num_processors(i) =", args_rank, processor_id);
	for(int i=0; i<ssf_num_machines(); i++) printf(" %d", ssf_num_processors(i)); printf("\n");
	printf("[%d:%d]   ssf_total_num_processors() = %d\n", args_rank,  processor_id, ssf_total_num_processors());
	printf("[%d:%d]   ssf_processor_index() = %d\n", args_rank,  processor_id, ssf_processor_index());
	printf("[%d:%d]   ssf_total_processor_index() = %d\n", args_rank,  processor_id, ssf_total_processor_index());
	int x, y, z; z = ssf_processor_range(x, y);
	printf("[%d:%d]   ssf_processor_range(%d,%d) = %d\n", args_rank,  processor_id, x, y, z);
      }
      ssf_barrier();
    }
  }
  */
}

#define REPORT_ARRAYSIZE_1 13
#define REPORT_ARRAYSIZE_2 7
#define REPORT_ARRAYSIZE 13 // larger of the two

void Universe::local_wrapup() 
{
  ssf_barrier(); // to prevent other processors to reclaim core structures prematurely
  if(!processor_id) {
    sim_state = SIM_STATE_FINALIZING; // run() may be skipped
    //if((args_debug_mask&DEBUG_FLAG_REPORT) != 0) printf(">>\n");
    time2 = ssf_wallclock_in_nanoseconds();
  }
  ssf_barrier();

  // wrap up all entities and processes of those entities for all the
  // timelines that belong to this universe
  for(SET(Timeline*)::iterator iter = timelines.begin();
      iter != timelines.end(); iter++) {
    for(SET(Entity*)::iterator e_iter = (*iter)->entities.begin();
	e_iter != (*iter)->entities.end(); e_iter++) {
      for(SET(Process*)::iterator p_iter = (*e_iter)->processes.begin();
	  p_iter != (*e_iter)->processes.end(); p_iter++) 
	(*p_iter)->wrapup();
      (*e_iter)->wrapup();
    }
  }
  ssf_barrier();

  if((args_debug_mask&DEBUG_FLAG_REPORT) != 0 || (args_debug_mask&DEBUG_FLAG_BRIEF) != 0) {
    /*
    if((args_debug_mask&DEBUG_FLAG_REPORT) != 0)
      printf("[%d:%d] local_wrapup(%d):\n", args_rank,  processor_id, processor_id);
    */

    unsigned long x[REPORT_ARRAYSIZE]; memset(x, 0, REPORT_ARRAYSIZE*sizeof(unsigned long));
    ssf_barrier();
    for(int p=0; p<args_nprocs; p++) {
      if(p == processor_id) {
	if(!p && (args_debug_mask&DEBUG_FLAG_REPORT) != 0) 
	  printf("[%d:0] TMLN      ENT       EVT       CTX       PCALL     RMSG      SMSG      LMSG      RNUL      SNUL      LNUL      LBTS      UPDAT\n", args_rank);
	for(SET(Timeline*)::iterator iter = timelines.begin();
	    iter != timelines.end(); iter++) {
	  Timeline* t = (*iter);
	  if((args_debug_mask&DEBUG_FLAG_REPORT) != 0) {
	    printf("[%d:%d] #%-8d %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu\n",
		   args_rank, processor_id, t->serialno, (unsigned long)t->entities.size(),
		   t->stats_processed_events, t->stats_process_context_switches,
		   t->stats_procedure_calls, t->stats_remote_messages, t->stats_shmem_messages, 
		   t->stats_local_messages, t->stats_remote_null_messages, 
		   t->stats_shmem_null_messages, t->stats_local_null_messages, 
		   t->stats_lbts_calculations, t->stats_subsequent_updates);
	  }
	  x[0] += t->stats_processed_events;
	  x[1] += t->stats_process_context_switches;
	  x[2] += t->stats_procedure_calls;
	  x[3] += t->stats_remote_messages; 
	  x[4] += t->stats_shmem_messages;
	  x[5] += t->stats_local_messages;
	  x[6] += t->stats_remote_null_messages;
	  x[7] += t->stats_shmem_null_messages;
	  x[8] += t->stats_local_null_messages;
	  x[9] += t->stats_lbts_calculations;
	  x[10] += t->stats_subsequent_updates;
	  x[11] += (unsigned long)t->entities.size();
	}
	x[12] = (unsigned long)timelines.size();
      }
      ssf_barrier();
    }

    x[0] = ssf_sum_reduction(x[0]);
    x[1] = ssf_sum_reduction(x[1]);
    x[2] = ssf_sum_reduction(x[2]);
    x[3] = ssf_sum_reduction(x[3]);
    x[4] = ssf_sum_reduction(x[4]);
    x[5] = ssf_sum_reduction(x[5]);
    x[6] = ssf_sum_reduction(x[6]);
    x[7] = ssf_sum_reduction(x[7]);
    x[8] = ssf_sum_reduction(x[8]);
    x[9] = ssf_sum_reduction(x[9]);
    x[10] = ssf_sum_reduction(x[10]);
    x[11] = ssf_sum_reduction(x[11]);
    x[12] = ssf_sum_reduction(x[12]);

#ifdef HAVE_MPI_H
    if(args_nmachs > 1 && !processor_id) {
      unsigned long y[REPORT_ARRAYSIZE_1];
      ssf_mpi_reduce(x, y, REPORT_ARRAYSIZE_1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
      if(!ssf_total_processor_index()) memcpy(x, y, REPORT_ARRAYSIZE_1*sizeof(unsigned long));
    }
#endif
    
    unsigned long nevts = x[0];
    if((args_debug_mask&DEBUG_FLAG_BRIEF) != 0 && !ssf_total_processor_index()) {
      printf("[ TOTAL TIMELINES: %lu ]\n", x[12]);
      printf("[ TOTAL EVENTS: %lu ]\n", x[0]);
    }

    if((args_debug_mask&DEBUG_FLAG_REPORT) != 0) {
      if(!ssf_total_processor_index() && x[12] > 1) { // if more than one timeline in total
	printf("[*:*] %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu\n",
	       x[12], x[11], x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7], x[8], x[9], x[10]);
      }

      ssf_barrier();
      for(int p=0; p<args_nprocs; p++) {
	if(p == processor_id) {
	  if(!p) printf("[%d:0] TLCTX     PACING    IOEVT     SMSG      SBYTE     RMSG      RBYTE\n", args_rank);
	  printf("[%d:%d] %-9lu %-9lu %-9lu", args_rank, processor_id, 
		 stats_timeline_context_switches, stats_timeline_pacing, stats_handle_io_events);
	  x[0] = stats_timeline_context_switches;
	  x[1] = stats_timeline_pacing;
	  x[2] = stats_handle_io_events;
	  if(!p) printf(" %-9lu %-9lu %-9lu %-9lu\n", stats_mpi_sent_messages,
			stats_mpi_sent_bytes, stats_mpi_rcvd_messages, stats_mpi_rcvd_bytes);
	  else printf(" *         *         *         *\n");
	}
	ssf_barrier();
      }
      x[0] = ssf_sum_reduction(x[0]);
      x[1] = ssf_sum_reduction(x[1]);
      x[2] = ssf_sum_reduction(x[2]);
      x[3] = stats_mpi_sent_messages;
      x[4] = stats_mpi_sent_bytes;
      x[5] = stats_mpi_rcvd_messages;
      x[6] = stats_mpi_rcvd_bytes;

#ifdef HAVE_MPI_H
      if(args_nmachs > 1 && !processor_id) {
	int y[REPORT_ARRAYSIZE_2];
	ssf_mpi_reduce(x, y, REPORT_ARRAYSIZE_2, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	if(!ssf_total_processor_index()) memcpy(x, y, REPORT_ARRAYSIZE_2*sizeof(unsigned long));
      }
#endif
      if(!ssf_total_processor_index()) {
	printf("[*:*] %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu %-9lu\n",
	       x[0], x[1], x[2], x[3], x[4], x[5], x[6]);
      }
    }

    if(!ssf_total_processor_index()) {
      if(args_nmachs > 1) 
	printf("[ EPOCH LENGTH: %lg (s) ]\n", epoch_length.second());
      if(args_nprocs > 1)
	printf("[ DECADE LENGTH (on 1st machine): %lg (s) ]\n", decade_length.second());
      printf("[ INIT TIME: %lg (s) ]\n", VirtualTime(time1-time0).second());
      printf("[ RUN TIME: %lg (s) ]\n", VirtualTime(time2-time1).second());
      printf("[ EVENT RATE: %lg (evts/s) ]\n", nevts/VirtualTime(time2-time0).second());
    }
  }
}

void Universe::global_wrapup()
{
  ssf_barrier_wrapup();
  /*
  if((args_debug_mask&DEBUG_FLAG_REPORT) != 0) {
    printf("[%d] global_wrapup:\n", args_rank);
  }
  */
  delete[] args_mach_nprocs; // not supposed to be used after ssf_finalize()!!!
  //assert(created_timelines.empty());
  for(SET(Timeline*)::iterator tmln_iter = created_timelines.begin();
      tmln_iter != created_timelines.end(); tmln_iter++)
    delete (*tmln_iter);
  created_timelines.clear();
  for(MAP(PAIR(int,int),Stargate*)::iterator sg_iter = created_stargates.begin();
      sg_iter != created_stargates.end(); sg_iter++)
    delete (*sg_iter).second;
  created_stargates.clear();
  //assert(orphan_entities.empty());
  for(SET(Entity*)::iterator ent_iter = orphan_entities.begin();
      ent_iter != orphan_entities.end(); ent_iter++)
    delete (*ent_iter);
  orphan_entities.clear();
  //assert(local_icmap.empty());
  local_icmap.clear();
  assert(remote_icmap.empty());
  //assert(named_inchannels_vector.empty());
  named_inchannels_vector.clear();
  delete[] timeline_scans; timeline_scans = 0;
  //assert(!mapreq_head && !mapreq_tail);
  MapRequest* node = mapreq_head;
  while(node) { MapRequest* req = node; node = req->next; delete req; }
  for(MAP(int,VECTOR(PAIR(MapInport*,Stargate*))*)::iterator sm_iter = starmap.begin();
      sm_iter != starmap.end(); sm_iter++) {
    VECTOR(PAIR(MapInport*,Stargate*))* vec = (*sm_iter).second; assert(vec);
    for(int i=0; i<(int)vec->size(); i++) delete vec->at(i).first;
    delete vec;
  }
  starmap.clear();
  delete[] parallel_universe;
  sim_state = SIM_STATE_FINISHED;

  if(switch_board) { // the 2D array should be empty (not checked)
    for(int i=0; i<args_nprocs; i++) delete[] switch_board[i];
    delete[] switch_board;
  }
}

void Universe::run(VirtualTime t, double s)
{
  // this function will be called by ssf_start() in main thread
  assert(!processor_id);

  args_endtime = t;
  args_speedup = s;

  // go for rounds until all entities are created, their init()
  // methods and their processes' init() methods will be called
  while(!created_entities.empty()) {
    // copy all the entities into a separate array
    int sz = created_entities.size(); assert(sz > 0);	
    Entity** ents = new Entity*[sz]; assert(ents);
    register int i;
    for(i=0; i<sz; i++) ents[i] = created_entities[i];
    created_entities.clear();
    for(i=0; i<sz; i++) ents[i]->init();
    delete[] ents;
  }

  // create timelines for orphan entities
  for(SET(Entity*)::iterator ent_iter = orphan_entities.begin();
      ent_iter != orphan_entities.end(); ent_iter++) {
    Timeline* tmln = create_timeline();
    tmln->add_entity(*ent_iter);
  }
  orphan_entities.clear();

  // auto-alignment kicks in
  if(0 < args_naligns && args_naligns < (int)created_timelines.size()) {
    metis_graph_t* graph = new metis_graph_t();

    // first create the graph nodes
    for(SET(Timeline*)::iterator tmln_iter = created_timelines.begin();
	tmln_iter != created_timelines.end(); tmln_iter++) {
      // this is temporary!!!
      (*tmln_iter)->serialno = 
	graph->add_node((*tmln_iter)->entities.size(), *tmln_iter);
    }

    // next connect the graph nodes
    MapRequest* req = mapreq_head;
    while(req) {
      if(req->ic) { // direct mapping
	if(req->oc->owner()->timeline != req->ic->owner()->timeline) {
	  graph->add_link(req->oc->owner()->timeline->serialno,
			  req->ic->owner()->timeline->serialno, 
			  req->oc->channel_delay+req->delay);
	}
      } else {
	MAP(STRING,inChannel*)::iterator iciter = local_icmap.find(req->icname);
	if(iciter != local_icmap.end() &&
	   req->oc->owner()->timeline != (*iciter).second->owner()->timeline) {
	  graph->add_link(req->oc->owner()->timeline->serialno,
			  (*iciter).second->owner()->timeline->serialno, 
			  req->oc->channel_delay+req->delay);
	}
      }
      req = req->next;
    }

    // partition using metis
    graph->partition(args_naligns);

    Timeline** tm = new Timeline*[args_naligns]; assert(tm);
    memset(tm, 0, args_naligns*sizeof(Timeline*));
    for(VECTOR(metis_node_t*)::iterator tmniter = graph->nodes.begin();
	tmniter != graph->nodes.end(); tmniter++) {
      int p = (*tmniter)->part;
      if(!tm[p]) tm[p] = (*tmniter)->timeline;
      else tm[p]->merge_timeline((*tmniter)->timeline);
    }
    delete[] tm;

    // clean up in the end
    delete graph;
  }

  // settle the unique timeline and entity serial numbers, and
  // outchannel port number, and assign timelines to universes in a
  // round robin fashion
  int ns[3]; 
  ns[0] = 0; // number of entities
  ns[1] = 0; // number of output ports
  ns[2] = created_timelines.size(); // number of timelines
  SET(Timeline*)::iterator tmln_iter;
  for(tmln_iter = created_timelines.begin();
      tmln_iter != created_timelines.end(); tmln_iter++) {
    ns[0] += (*tmln_iter)->get_serialno_space();
    ns[1] += (*tmln_iter)->get_portno_space();
  }
  int* scans = new int[args_nmachs*3]; assert(scans);
  timeline_scans = new int[args_nmachs]; assert(timeline_scans);
#ifdef HAVE_MPI_H
  ssf_mpi_allgather(ns, 3, MPI_INT, scans, 3, MPI_INT, MPI_COMM_WORLD);
#else
  scans[0] = ns[0]; 
  scans[1] = ns[1];
  scans[2] = ns[2];
#endif
  int partial_sum0 = 0, partial_sum1 = 0, partial_sum2 = 0;
  for(int i=0; i<args_nmachs; i++) {
    partial_sum0 += scans[i*3];
    partial_sum1 += scans[i*3+1];
    partial_sum2 += scans[i*3+2];
    scans[i*3] = partial_sum0;
    scans[i*3+1] = partial_sum1;
    timeline_scans[i] = scans[i*3+2] = partial_sum2;
  }
  int startne = scans[3*args_rank]-ns[0];
  int startnp = scans[3*args_rank+1]-ns[1];
  int startnt = scans[3*args_rank+2]-ns[2];
  //printf("[%d] ne=%d, np=%d, nt=%d\n", args_rank, startne, startnp, startnt);
  delete[] scans;

  int tid = 0;
  for(tmln_iter = created_timelines.begin();
      tmln_iter != created_timelines.end(); tmln_iter++, tid++) {
    int x = BLOCK_OWNER(tid,args_nprocs,ns[2]);
    parallel_universe[x]->assign_timeline(*tmln_iter);
    startne = (*tmln_iter)->settle_serialno(startnt+tid, startne);
    startnp = (*tmln_iter)->settle_portno(startnp);
    if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
      printf("[%d] assign timeline %d to universe %d (i=%d,p=%d,n=%d)\n",
	     args_rank, (*tmln_iter)->serialno, x, tid, args_nprocs, ns[2]);
    }
  }
  created_timelines.clear();

  // wire up the channel mappings
  synchronize_inchannel_names();
  wire_up_channels();

  // find the training thresholds
  enumerate_channel_delays();

  // now run in parallel with other processors
  run();
}

void Universe::enumerate_channel_delays()
{
  //printf("%d %d\n", args_global_thresh_set, args_local_thresh_set);

  // find all distinct global and local delays on this machine
  MAP(VirtualTime,int) global_distinct_delays, local_distinct_delays;

  if(!args_global_thresh_set || !args_local_thresh_set) {
    for(MAP(PAIR(int,int),Stargate*)::iterator sg_iter = created_stargates.begin();
	sg_iter != created_stargates.end(); sg_iter++) {
      VirtualTime mt = (*sg_iter).second->min_delay;
      if((*sg_iter).second->source_timeline && (*sg_iter).second->target_timeline) {
	// the stargate is connecting local timelines
	MAP(VirtualTime,int)::iterator iter = local_distinct_delays.find(mt);
	if(iter != local_distinct_delays.end()) (*iter).second++;
	else local_distinct_delays.insert(MAKE_PAIR(mt,1));
      } else if (!(*sg_iter).second->target_timeline) {
	// the stargate is connecting TO a remote machine
	MAP(VirtualTime,int)::iterator iter = global_distinct_delays.find(mt);
	if(iter != global_distinct_delays.end()) (*iter).second++;
	else global_distinct_delays.insert(MAKE_PAIR(mt,1));
	/*
	  printf("[%d] global %d->%d = %lg\n", args_rank, 
	     (*sg_iter).second->source_timeline_id,
	     (*sg_iter).second->target_timeline_id, mt.second());
	*/
      }
    }
  }						
  //printf("--->%d %d\n", (int)global_distinct_delays.size(), (int)local_distinct_delays.size());

#ifdef HAVE_MPI_H
  // find all distinct global delays among all machines
  if(!args_global_thresh_set && args_nmachs > 1) {
    // collect all sizes
    int nn = global_distinct_delays.size();
    int* nc = new int[args_nmachs]; assert(nc);
    ssf_mpi_allgather(&nn, 1, MPI_INT, nc, 1, MPI_INT, MPI_COMM_WORLD);
    int n = 0;
    for(int i=0; i<args_nmachs; i++) n += nc[i];
    //printf("total = %d\n", n);

    if(n > 0) { // only if there are globals
      // prepare my delays and counts
      int64* myts = 0; 
      int* myns = 0;
      if(nn > 0) { 
	myts = new int64[nn]; assert(myts);
	myns = new int[nn]; assert(myns);
	int i = 0;
	for(MAP(VirtualTime,int)::iterator iter = global_distinct_delays.begin();
	    iter != global_distinct_delays.end(); iter++) {
	  myts[i] = (*iter).first.get_ticks();
	  myns[i] = (*iter).second;
	  //printf("[%d] %lld %d\n", args_rank, myts[i], myns[i]);
	  i++;
	}
      }

      // gathering
      int64* ts = new int64[n]; assert(ts);
      int* ns = new int[n]; assert(ns);
      int* displs = new int[args_nmachs]; assert(displs);
      int acc = 0;
      for(int i=0; i<args_nmachs; i++) {
	displs[i] = acc;
	acc += nc[i];
      }
      ssf_mpi_allgatherv(myts, nn, MPI_LONG_LONG_INT, ts, nc, displs, MPI_LONG_LONG_INT, MPI_COMM_WORLD);
      ssf_mpi_allgatherv(myns, nn, MPI_INT, ns, nc, displs, MPI_INT, MPI_COMM_WORLD);

      // reconstruct the global distinct delays from gather results
      global_distinct_delays.clear();
      for(int k=0; k<n; k++) {
	//printf("--> k=%d, %lld, %d\n", k, ts[k], ns[k]);
	VirtualTime mt(ts[k]);
	MAP(VirtualTime,int)::iterator iter = global_distinct_delays.find(mt);
	if(iter != global_distinct_delays.end()) (*iter).second += ns[k];
	else global_distinct_delays.insert(MAKE_PAIR(mt,ns[k]));
      }

      if(myts) delete[] myts;
      if(myns) delete[] myns;
      delete[] nc;
      delete[] ts; 
      delete[] ns; 
      delete[] displs;
    }
  }
#endif

  // trimming down distinct delays
  if(!args_local_thresh_set && local_distinct_delays.size() > 0) 
    trim_channel_delays(local_distinct_delays, local_training_thresholds, MAX_LOCAL_TRAINING_ROUNDS);
  int sz = (int)local_training_thresholds.size();
  if(sz > 0) {
    local_training_round = local_training_thresholds[sz-1];
    local_training_length = local_training_round*sz;
  } else local_training_length = local_training_round = 0;
#ifdef HAVE_MPI_H
  if(args_nmachs > 1) {
    int64 x = local_training_length.get_ticks(), y;
    ssf_mpi_allreduce(&x, &y, 1, MPI_LONG_LONG_INT, MPI_MAX, MPI_COMM_WORLD);
    local_training_length.set_ticks(y);
  }
#endif
  if(!args_global_thresh_set && global_distinct_delays.size() > 0)
    trim_channel_delays(global_distinct_delays, global_training_thresholds, 
			MAX_GLOBAL_TRAINING_ROUNDS);
  sz = global_training_thresholds.size();
  if(sz > 0) {
    global_training_round = global_training_thresholds[sz-1];
    global_training_length = global_training_round*sz;
  } else global_training_length = global_training_round = 0;

  // we need to avoid the situation where the training round is too
  // small to be effective
  VirtualTime tt = local_training_length + global_training_length;
  if(tt > 0) {
    if(args_training_len == 0) 
      args_training_len = 0.05*args_endtime;
    double f = args_training_len/tt; // we allow 5% time for training
    if(f > 1) {
      local_training_round *= f;
      local_training_length *= f;
      global_training_round *= f;
      global_training_length *= f;
    }
  }
  if(!args_rank && (args_debug_mask&DEBUG_FLAG_BRIEF) != 0 &&
     local_training_length+global_training_length > 0) {
    printf("[ TRAINING LENGTH: %lg (s) ]\n",  
	   (local_training_length+global_training_length).second());
  }

  if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
    printf("[%d] global training length/round=%lg/%lg\n", args_rank,
	   global_training_length.second(), global_training_round.second());
    printf("[%d] local training length/round=%lg/%lg\n", args_rank,
	   local_training_length.second(), local_training_round.second());
    int i = 0;
    for(VECTOR(VirtualTime)::iterator iter = global_training_thresholds.begin();
	iter != global_training_thresholds.end(); iter++, i++)
      printf("[%d] global-thresh[%d]=%lg\n", args_rank, i, (*iter).second());
    i = 0;
    for(VECTOR(VirtualTime)::iterator iter = local_training_thresholds.begin();
	iter != local_training_thresholds.end(); iter++, i++)
      printf("[%d] local-thresh[%d]: %lg\n", args_rank, i, (*iter).second());
  }
}

struct edge_sort_item_t { int64 delay; double priority; };
struct edge_sort_less : public LESS(edge_sort_item_t) {
  bool operator()(const edge_sort_item_t& t1, const edge_sort_item_t& t2) {
    return t1.priority > t2.priority; 
  }
};
void Universe::trim_channel_delays(MAP(VirtualTime,int)& distinct_delays,
				   VECTOR(VirtualTime)& training_thresholds, 
				   int max_rounds)
{
  if((int)distinct_delays.size() > max_rounds) {
    VECTOR(edge_sort_item_t) sortvec;
    for(MAP(VirtualTime,int)::iterator iter = distinct_delays.begin();
	iter != distinct_delays.end(); iter++) {
      edge_sort_item_t item;
      item.delay = (*iter).first.get_ticks();
      item.priority = (*iter).second;
      sortvec.push_back(item);
      //printf("[%d] %lld=>%lf\n", args_rank, item.delay, item.priority);
    }
    int64 base = sortvec[0].delay;
    int acc = 0;
    for(VECTOR(edge_sort_item_t)::iterator siter = sortvec.begin();
	siter != sortvec.end(); siter++) {
      acc += (*siter).priority;
      (*siter).priority = ((double)(*siter).delay/base)/acc;
    }
    std::sort(sortvec.begin(), sortvec.end(), edge_sort_less());
    int i = 0;
    for(VECTOR(edge_sort_item_t)::iterator siter = sortvec.begin();
	siter != sortvec.end() && i < max_rounds; siter++, i++)
      training_thresholds.push_back(VirtualTime((*siter).delay));
  } else {
    for(MAP(VirtualTime,int)::iterator iter = distinct_delays.begin();
	iter != distinct_delays.end(); iter++) 
      training_thresholds.push_back((*iter).first);
  }
  std::sort(training_thresholds.begin(), training_thresholds.end());
  training_thresholds.push_back(training_thresholds[training_thresholds.size()-1]*3);
}

// return # sync global channels; set the new epoch length
int Universe::classify_global_channels(VirtualTime t, VirtualTime now, bool training)
{
  int nlinks_sync = 0;
  if(training) epoch_length = t;
  else epoch_length = VirtualTime::INFINITY;

#ifdef HAVE_MPI_H
  for(MAP(PAIR(int,int),Stargate*)::iterator sg_iter = created_stargates.begin();
      sg_iter != created_stargates.end(); sg_iter++) {
    VirtualTime mt = (*sg_iter).second->min_delay;
    if(!(*sg_iter).second->source_timeline || !(*sg_iter).second->target_timeline) {
      // the stargate is connecting with remote machine
      if(mt < t) {
	bool tmp = (*sg_iter).second->in_sync;
	(*sg_iter).second->in_sync = false; 
	if(tmp) {
	  // before this channel was synchronous, now it's
	  // asynchronous, we need to update time
	  (*sg_iter).second->time = now+(*sg_iter).second->min_delay;
	}
      } else {
	(*sg_iter).second->in_sync = true;
	nlinks_sync++;
	if(!training && mt < epoch_length) epoch_length = mt;
      }
      if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	printf("[%d] (global)stargate[%d=>%d,delay=%lg]: in_sync=%d, t=%lg\n", args_rank,
	       (*sg_iter).second->source_timeline_id, (*sg_iter).second->target_timeline_id, 
	       mt.second(), (*sg_iter).second->in_sync, t.second());
      }
    } else {
      // the stargate is connecting local timelines
    }
  }

  if(!training && args_nmachs > 1) {
    int64 myepoch = epoch_length.get_ticks(), yourepoch;
    ssf_mpi_allreduce(&myepoch, &yourepoch, 1, MPI_LONG_LONG_INT, MPI_MIN, MPI_COMM_WORLD);
    epoch_length.set_ticks(yourepoch);
    int new_nlinks_sync = nlinks_sync;
    ssf_mpi_allreduce(&new_nlinks_sync, &nlinks_sync, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  }
#endif

  if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
    printf("[%d] classify global(%lg): epoch_length=%lg, sync_links=%d\n", 
	   args_rank, t.second(), epoch_length.second(), nlinks_sync);
  }
  return nlinks_sync;
}

// return # sync local channels; set the new decade length
int Universe::classify_local_channels(VirtualTime t, VirtualTime now, bool training)
{
  int nlinks_sync = 0;
  if(training) decade_length = t;
  else decade_length = VirtualTime::INFINITY;

  for(MAP(PAIR(int,int),Stargate*)::iterator sg_iter = created_stargates.begin();
      sg_iter != created_stargates.end(); sg_iter++) {
    VirtualTime mt = (*sg_iter).second->min_delay;
    if(!(*sg_iter).second->source_timeline || !(*sg_iter).second->target_timeline) {
      // the stargate is connecting with remote machine
    } else {
      // the stargate is connecting local timelines
      if(mt < t) {
	bool tmp = (*sg_iter).second->in_sync;
	(*sg_iter).second->in_sync = false; 
	if(tmp) {
	  // before this channel was synchronous, now it's
	  // asynchronous, we need to update time
	  (*sg_iter).second->time = now+(*sg_iter).second->min_delay;
	}
      } else {
	(*sg_iter).second->in_sync = true;
	nlinks_sync++;
	if(!training && mt < decade_length) decade_length = mt;
      }
      if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
	printf("[%d] (local)stargate[%d=>%d,delay=%lg]: in_sync=%d, t=%lg\n", args_rank,
	       (*sg_iter).second->source_timeline_id, (*sg_iter).second->target_timeline_id, 
	       mt.second(), (*sg_iter).second->in_sync, t.second());
      }
    }
  }

  if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
    printf("[%d] classify local(%lg): decade_length=%lg, sync_links=%d\n", 
	   args_rank, t.second(), decade_length.second(), nlinks_sync);
  }
  return nlinks_sync;
}

void Universe::classify_timeline_channels()
{
  for(SET(Timeline*)::iterator tmln_iter = timelines.begin();
      tmln_iter != timelines.end(); tmln_iter++) {
    Timeline* tmln = *tmln_iter;
    tmln->classify_async();
  }
}

Universe::Universe(int id) : 
  qmem_chunklist(0), qmem_poolsize(0), qmem_poolbegin(0), 
  qmem_poolend(0), qmem_freelist(0), processor_id(id), 
  synpoint(0), next_decade(0), next_epoch(0), 
  global_binque(0), local_binque(0), 
  mailbox(0), mailbox_tail(0),
  stats_timeline_context_switches(0),
  stats_timeline_pacing(0),
  stats_handle_io_events(0)
{
  parallel_universe[processor_id] = this;
  ssf_thread_mutex_init(&mailbox_mutex);
  ssf_thread_cond_init(&mailbox_cond);

  ssf_quickmem_init(processor_id);
}

Universe::~Universe() 
{
  assert(this == parallel_universe[processor_id]);

  // remove all timelines that belong to this universe
  for(SET(Timeline*)::iterator iter = timelines.begin();
      iter != timelines.end(); iter++) delete (*iter);
  timelines.clear();

  //assert(!mailbox); // must have no left-over events
  while(mailbox) {
    ChainedEvent* evt = mailbox;
    mailbox = mailbox->get_next_event();
    delete evt;
  }
  mailbox_tail = 0;

  ssf_quickmem_wrapup(processor_id);
  parallel_universe[processor_id] = 0;
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
