/**
 * \file ssf.h
 * \brief Header file for all SSF classes and functions.
 *
 * This is the only header file that the user needs to include to use
 * all SSF classes and functions.
 */

#ifndef __MINISSF_H__
#define __MINISSF_H__

#include <stdio.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "ssfapi/ssf_common.h"
#include "random/random.h"
#include "ssfapi/compact_datatype.h"
#include "ssfapi/quick_memory.h"
#include "ssfapi/entity.h"
#include "ssfapi/process.h"
#include "ssfapi/event.h"
#include "ssfapi/inchannel.h"
#include "ssfapi/outchannel.h"
#include "ssfapi/ssf_timer.h"
#include "ssfapi/ssf_semaphore.h"
#endif

/** \brief The namespace for all minissf classes and functions. 
 *
 * All classes and functions of minissf are implemented in this
 * namespace to avoid potential name conflict.
 */
namespace minissf {

/** \brief Initialize SSF.
 *
 * This function must be called at the beginning (at the main
 * function) before the user can process the command-line arguments.
 *
 * \param argc a reference to the number of command-line arguments
 * \param argv a reference to the argument vector
 *
 * minissf will initialize the parallel execution environment (such as
 * MPI and pthreads), filter out and parse the command-line arguments
 * that are meant for it.
 *
 * @see ssf_print_options() for the list of command-line arguments
 * that are acceptable by minissf. @see ssf_start() and @see
 * ssf_finalize().
 */
extern void ssf_init(int argc, char** argv);

/** \brief Finalize SSF.
 *
 * This function is expected to be called before exiting from the main
 * function. This function wil clean up all SSF data structures and
 * states, including entities, processes, channels, and events.
 * 
 * Unless there has been a call to ssf_abort(), it is important that
 * this function is called before existing the main function, or the
 * simulation may not finish correctly. After this function is
 * returned, the user must not call any SSF routines or accessing any
 * SSF objects (such as entities, processes, channels, and
 * events). For example, if the user wishes to print out the final
 * statistics of the simulation, she should do that between the
 * ssf_start() function and this function in the main function.
 *
 * @see ssf_init(), @see ssf_start() and @see ssf_abort().
 */
extern void ssf_finalize();

/** \brief Start simulation.
 *
 * This function starts the simulation for the given amount of time.
 *
 * \param endtime the total simulation time to run the simulation
 * \param speedup the expected ratio of progress in simulation time over real time.
 *
 * The function runs the simulation over the interval from time zero
 * up to the given endtime. This function is normalled called in the
 * main function after the simulation entities have been
 * created. speedup is used to indicate the emulation speed, which is
 * defined by the simulation time divided by the wall-clock time. If
 * ignored, speedup will be set to zero by default, which means that
 * the emulation speed is infinite. In this case, we run simulation
 * independent from the wall-clock time. That is, the simulation will
 * be run as fast as possible. If speedup is five, the simulation time
 * progresses five times as fast as the wall-clock time. Setting
 * speedup to be one means we run the simulation in real time.
 *
 * @see ssf_init(), @see ssf_finalize() and @see ssf_abort().
 */
extern void ssf_start(VirtualTime endtime, double speedup = 0);

/** \brief Terminate SSF in case of an error condition.
 * 
 * This function should be called at the main function to terminate
 * the SSF execution. minissf will clean up the execution environment
 * before exiting the program with the given error code.
 *
 * \param errorcode error code to return to invoking environment
 *
 * @see ssf_init() and @see ssf_finalize().
 */
extern void ssf_abort(int errorcode);

/** \brief Print SSF command-line options.
 *
 * This function outputs the list of command-line options that are
 * accepted by the SSF simulator. The ssf_init() method parses and
 * filters out these command-line options.
 *
 * \param fptr points to the file to output the list of command-line options.
 *
 * @see ssf_init().
 */
extern void ssf_print_options(FILE* fptr);

/** @name Runtime environment information. */
/** @{ */

/** \brief Return the number of distributed machines running minissf. */
extern int ssf_num_machines();

/** \brief Return the caller's machine index (MPI rank). */
extern int ssf_machine_index();

#ifdef HAVE_MPI_H
/** \brief Return the MPI communicator for all distributed machines running minissf. */
extern MPI_Comm ssf_machine_communicator();
#endif

/** \brief Return the number of processors available on the machine of the given index. */
extern int ssf_num_processors();

/** \brief Return the number of processors available on the current machine. */
extern int ssf_num_processors(int machid);

/** \brief Return the total number of processors used for the entire distributed simulation environment. */
extern int ssf_total_num_processors();

/** \brief Return the caller's processor index on the current machine. */
extern int ssf_processor_index();

/** \brief Return the global index of this processor. */
extern int ssf_total_processor_index();

/** 
 * \brief Get the processor index range of the current machine.
 *
 * The processor index range will be the global processor indices of
 the first processor and the last processor of the current machine.
 *
 * \param startid returns the global processor index of the first processor
 * \param endid returns the global processor index of the last processor
 * \returns the global index of the caller's processor.
 */
extern int ssf_processor_range(int& startid, int& endid);
/** @} */

}; /*namespace minissf*/

#endif /*__MINISSF_H__*/

/**
 * \mainpage Main Page
 *
 * MiniSSF is a streamlined implementation of the Scalable Simulation
 * Framework (SSF) that supports large-scale parallel discrete-event
 * simulation. MiniSSF implements a subset of the SSF API and modifies
 * it whenever necessary for greater usability. MiniSSF uses the
 * minimalistic design principle and as such features only a small
 * number of functions and classes for users to conveniently construct
 * large-scale discrete-event simulation models. MiniSSF supports
 * emulation by throttling the simulation time in relation to the
 * wall-clock time. MiniSSF also supports portable simulation on most
 * common parallel platforms.

 * <b>Copyright (c) 2011-2013 Florida International University.</b>
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
 *   Jason Liu\n
 *   Modeling and Networking Systems Research Group\n
 *   School of Computing and Information Sciences\n
 *   Florida International University\n
 *   Miami, Florida 33199, USA
 *
 * You can find our research at http://www.primessf.net/.
 *
 * This reference manual describes the classes and functions of the
 * MiniSSF implementation. It is generated automatically from the
 * source code using doxygen.
 */
