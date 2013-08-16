#include <stdlib.h>
#include "ssf.h"
#include "kernel/universe.h"

/*
***SIM_STATE_UNINITIALIZED***
main()
  ssf_init()
    MPI_Init()
    U:parse_command_line()
    U:global_init()
      ***SIM_STATE_INITIALIZING***
      ssf_barrier_init()
    end
    ssf_fork_children()
      ssf_child_main()
        new Universe() -> pthread_setspecific()
	  ssf_quickmem_init()
	U->local_init()
	<<<<< pid != 0 >>>>>
	if(go) U->run()
	U->local_wrapup()
	delete Universe
	  ssf_quickmem_wrapup();
	<<<<<<<<<<>>>>>>>>>>
      end
    end
  end

  <<<<< pid == 0 >>>>>
  USER CREATES ENTITIES
  ssf_start()
    go=1; U->run()
      ***SIM_STATE_RUNNING***
      ...
      ***SIM_STATE_FINALIZING***
    end
  end

  ssf_finalize()
    if ssf_start() is skipped:
    U->local_wrapup()
    delete Universe
      ssf_quickmem_wrapup();
    ssf_join_children()    
    U:global_wrapup()
      ssf_barrier_wrapup();
      SIM_STATE_FINISHED
    end
    delete misc data blocks
    MPI_Finalize()
  end
  <<<<<<<<<<>>>>>>>>>>
end
*/

namespace minissf {

pthread_key_t thread_id;

static void ssf_child_main(int myid)
{
  Universe* univ = new Universe(myid); assert(univ);
  pthread_setspecific(thread_id, univ);

  univ->local_init();

  if(myid > 0) {
    int go = ssf_min_reduction((int)1);
    if(go > 0) univ->run();
    univ->local_wrapup();
    delete univ; //(Universe*)pthread_getspecific(thread_id);
  }
}

void ssf_init(int argc, char** argv)
{
#ifdef HAVE_MPI_H
  ssf_mpi_init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &Universe::mpi_thread_support);
  const char* mpi_thread_info[] = {"MPI_THREAD_SINGLE",
				   "MPI_THREAD_FUNNELED",
				   "MPI_THREAD_SERIALIZED",
				   "MPI_THREAD_MULTIPLE"};
  std::cerr << "MPI thread support: " << 
    mpi_thread_info[Universe::mpi_thread_support] << std::endl;
#endif
  
  if(Universe::parse_command_line(argc, argv))
    SSF_THROW("invalid command line arguments");

  Universe::global_init();
  pthread_key_create(&thread_id, 0);
  ssf_fork_children(Universe::args_nprocs, ssf_child_main);
}

void ssf_start(VirtualTime t, double s)
{
  if(Universe::is_uninitialized()) { SSF_THROW("ssf has not been initialized"); }
  else if(!Universe::is_initializing())
    SSF_THROW("must be called at the end of simulation initialization"); 
  if(ssf_processor_index()) 
    SSF_THROW("only main thread is allowed to call this function");

  if(t <= 0) SSF_THROW("invalid simulation end time: " << t);
  if(s < 0) SSF_THROW("invalid simulation speedup: " << s);

  Universe* univ = (Universe*)pthread_getspecific(thread_id);
  ssf_min_reduction((int)1);
  univ->run(t, s);
}

void ssf_finalize() 
{
  if(Universe::is_uninitialized()) { SSF_THROW("ssf has not been initialized"); }
  if(ssf_processor_index()) 
    SSF_THROW("only main thread is allowed to call this function");

  Universe* univ = (Universe*)pthread_getspecific(thread_id);
  if(Universe::is_initializing()) { // ssf_start() was skipped
    ssf_min_reduction((int)0); // skip run()
  }

  univ->local_wrapup();
  delete univ;
  ssf_join_children();
  Universe::global_wrapup();

 // not supposed to be used afterwards!!!
  if(Event::registered_event_names) {
    delete Event::registered_event_names;
    delete Event::registered_event_factories;
  }

#ifdef HAVE_MPI_H
  ssf_mpi_finalize();
#endif
}

void ssf_abort(int err) 
{
  ssf_finalize();
  exit(err);
}

void ssf_print_options(FILE* fptr)
{
  if(!fptr) { SSF_THROW("invalid file descriptor"); }
  else Universe::print_command_line(fptr);
}

int ssf_num_machines() { 
  if(Universe::is_uninitialized()) SSF_THROW("ssf has not been initialized");
  if(Universe::is_finished()) SSF_THROW("ssf has finished");
  return Universe::args_nmachs; 
}

int ssf_machine_index() { 
  if(Universe::is_uninitialized()) SSF_THROW("ssf has not been initialized");
  if(Universe::is_finished()) SSF_THROW("ssf has finished");
  return Universe::args_rank;
}

#ifdef HAVE_MPI_H
MPI_Comm ssf_machine_communicator() { 
  if(Universe::is_uninitialized()) SSF_THROW("ssf has not been initialized");
  if(Universe::is_finished()) SSF_THROW("ssf has finished");
  return MPI_COMM_WORLD;
}
#endif

int ssf_num_processors() {
  if(Universe::is_uninitialized()) SSF_THROW("ssf has not been initialized");
  if(Universe::is_finished()) SSF_THROW("ssf has finished");
  return Universe::args_nprocs;
}

int ssf_num_processors(int machid) {
  if(Universe::is_uninitialized()) SSF_THROW("ssf has not been initialized");
  if(Universe::is_finished()) SSF_THROW("ssf has finished");
  if(0 > machid || machid >= Universe::args_nmachs)
    SSF_THROW("machine id out of range: " << machid << 
	      " (needs to be between 0 and " << Universe::args_nmachs-1 << ")");
  return Universe::args_mach_nprocs[machid];
}

int ssf_total_num_processors() {
  if(Universe::is_uninitialized()) SSF_THROW("ssf has not been initialized");
  if(Universe::is_finished()) SSF_THROW("ssf has finished");
  if(!Universe::total_num_procs) {
    int np = 0;
    for(int i=0; i<Universe::args_nmachs; i++)
      np += Universe::args_mach_nprocs[i];
    Universe::total_num_procs = np;
  }
  return Universe::total_num_procs;
}

int ssf_processor_index() {
  if(Universe::is_uninitialized()) SSF_THROW("ssf has not been initialized");
  if(Universe::is_finished()) SSF_THROW("ssf has finished");
  return ((Universe*)pthread_getspecific(thread_id))->processor_id;
}

int ssf_total_processor_index() {
  if(Universe::is_uninitialized()) SSF_THROW("ssf has not been initialized");
  if(Universe::is_finished()) SSF_THROW("ssf has finished");
  int np = 0;
  for(int i=0; i<Universe::args_rank; i++)
    np += Universe::args_mach_nprocs[i];
  return np+ssf_processor_index();
}

int ssf_processor_range(int& startidx, int& endidx) {
  if(Universe::is_uninitialized()) SSF_THROW("ssf has not been initialized");
  if(Universe::is_finished()) SSF_THROW("ssf has finished");
  int np = 0;
  for(int i=0; i<Universe::args_rank; i++)
    np += Universe::args_mach_nprocs[i];
  startidx = np;
  endidx = np+Universe::args_nprocs-1;
  return np+ssf_processor_index();
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
