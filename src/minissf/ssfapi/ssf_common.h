/**
 * \file ssf_common.h
 * \brief Header file for common definitions.
 *
 * This header file contains definitions of configuration macros,
 * primitive types, STL containers, forward declarations, virtual time
 * and exception handling, all commonly used by minissf.  Users do not
 * need to include this header file directly; it is included from
 * ssf.h.
 */

#ifndef __MINISSF_COMMON_H__
#define __MINISSF_COMMON_H__

#include "ssf_config.h"

/// We assume homogeneous machines to run minissf.
#define HOMOGENEOUS_ENVIRONMENT 1

#include <deque>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#ifdef HAVE_MPI_H
#include <mpi.h>
#endif

namespace minissf {

/*
 * We define these primitive types because C/C++ varies the size of
 * the primitive types on different platforms (different machine
 * architecture, different operating system, or even different
 * compiler options). The types defined here are explicit about their
 * size.
 */
  
/** \name Platform-Independent Primitive Data Types */
/** @{ */
typedef char int8; ///< 8-bit character type
typedef unsigned char uint8; ///< 8-bit unsigned character type
typedef unsigned char byte; ///< 8-bit unsigned character type
typedef short int16; ///< 16-bit integer type (short)
typedef unsigned short uint16; ///< 16-bit unsigned integer type (unsigned short)
typedef int int32; ///< 32-bit integer type
typedef unsigned int uint32; ///< 32-bit unsigned integer type
#ifdef HAVE_LONG_LONG_INT
typedef long long int int64; ///< 64-bit integer type
typedef unsigned long long int uint64; ///< 64-bit unsigned integer type
#else
typedef long int int64; ///< 64-bit integer type
typedef unsigned long int uint64; ///< 64-bit unsigned integer type
#endif
/** @} */

/*
 * Common STL containers used in the simulator; we define them with
 * alternative names so that we can later possibly replace them, say
 * to use a different memory allocator.
 */

/** \name Commonly Used Standard Template Library Types */
/** @{ */
#define LESS(T) std::less<T > ///< same as std::less
#define MAKE_PAIR(A,B) std::make_pair(A,B) ///< same as std::make_pair
#define PAIR(A,B) std::pair<A,B > ///< same as std::pair
#define PRIORITY_QUEUE(T,S,C) std::priority_queue<T,S,C > ///< same as std::priority_queue
#define DEQUE(T) std::deque<T > ///< same as std::deque
#define MAP(S,T) std::map<S,T > ///< same as std::map
#define MULTIMAP(S,T) std::multimap<S,T > ///< same as std::multimap
#define SET(T) std::set<T > ///< same as std::set
#define VECTOR(T) std::vector<T > ///< same as std::vector

typedef std::string STRING; ///< same as std::string
typedef std::ostream OSTREAM; ///< same as std::ostream
/** @} */

// forward definition of some ssf classes
class Entity;
class Procedure;
class Process;
class inChannel;
class outChannel;
class Event;
class Timer;
class Semaphore;
class Timeline;
class Universe;
class MapOutport;
class MapInport;
class Stargate;
class KernelEvent;
class TickEvent;
class TimerEvent;
class HoldEvent;
class ChannelEvent;
class ProcessEvent;
class SemaphoreEvent;

}; /*namespace minissf*/

#include "kernel/throwable.h"
#include "ssfapi/virtual_time.h"

#endif /*__MINISSF_COMMON_H__*/

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
