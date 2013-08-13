// low-level machine-related functions

#ifndef __MINISSF_MACHINE_H__
#define __MINISSF_MACHINE_H__

#include "ssfapi/ssf_common.h"

#if defined(HAVE_PTHREAD_H)
#include <pthread.h>
#else
#error "ERROR: pthreads are required!"
#endif

namespace minissf {

// padding to avoid false sharing
#define SSF_CACHE_LINE_SIZE 128
#define SSF_CACHE_LINE_PADDING \
  char ssf_dummy_for_cache_line_padding[SSF_CACHE_LINE_SIZE]

typedef pthread_t ssf_thread_t;
typedef pthread_mutex_t ssf_thread_mutex_t;
typedef pthread_cond_t ssf_thread_cond_t;

extern void ssf_fork_children(int num, void (*child)(int));
extern void ssf_join_children();
extern void ssf_thread_create(ssf_thread_t* tid, void (*child)(void*), void* data);
extern void ssf_thread_yield();
extern int64 ssf_wallclock_in_nanoseconds();

inline void ssf_thread_join(ssf_thread_t* tid)
  { pthread_join(*tid, 0); }
//inline int ssf_thread_kill(ssf_thread_t tid, int sig)
//  { return pthread_kill(tid, sig); }
//inline void ssf_thread_cancel(ssf_thread_t* tid) 
//  { pthread_cancel(*tid); }
//inline ssf_thread_t ssf_thread_self()
//  { return pthread_self(); }

inline void ssf_thread_mutex_init(ssf_thread_mutex_t* lock)
  { pthread_mutex_init(lock, 0); }
inline void ssf_thread_mutex_lock(ssf_thread_mutex_t* lock) 
  { pthread_mutex_lock(lock); }
inline void ssf_thread_mutex_unlock(ssf_thread_mutex_t* lock)
  { pthread_mutex_unlock(lock); }
/*
inline int  ssf_thread_mutex_trylock(ssf_thread_mutex_t* lock) 
  { return  !pthread_mutex_trylock(lock); }
*/

inline void ssf_thread_cond_init(ssf_thread_cond_t* cond)
  { pthread_cond_init(cond, 0); }
inline void ssf_thread_cond_wait(ssf_thread_cond_t* cond, ssf_thread_mutex_t* lock)
  { pthread_cond_wait(cond, lock); }
inline int ssf_thread_cond_timedwait(ssf_thread_cond_t* cond, 
    ssf_thread_mutex_t* lock, const struct timespec* abstime)
  { return pthread_cond_timedwait(cond, lock, abstime); }
inline void ssf_thread_cond_signal(ssf_thread_cond_t* cond)
  { pthread_cond_signal(cond); }
inline void ssf_thread_cond_broadcast(ssf_thread_cond_t* cond)
  { pthread_cond_broadcast(cond); }

extern void ssf_barrier_init();
extern void ssf_barrier_wrapup();
extern void ssf_barrier();
extern char ssf_min_reduction(char myval);
extern short ssf_min_reduction(short myval);
extern int ssf_min_reduction(int myval);
extern long ssf_min_reduction(long myval);
extern unsigned char ssf_min_reduction(unsigned char myval);
extern unsigned short ssf_min_reduction(unsigned short myval);
extern unsigned int ssf_min_reduction(unsigned int myval);
extern unsigned long ssf_min_reduction(unsigned long myval);
extern float ssf_min_reduction(float  myval);
extern double ssf_min_reduction(double myval);
extern char ssf_sum_reduction(char myval);
extern short ssf_sum_reduction(short myval);
extern int ssf_sum_reduction(int myval);
extern long ssf_sum_reduction(long myval);
extern unsigned char ssf_sum_reduction(unsigned char myval);
extern unsigned short ssf_sum_reduction(unsigned short myval);
extern unsigned int ssf_sum_reduction(unsigned int myval);
extern unsigned long ssf_sum_reduction(unsigned long myval);
#ifdef HAVE_LONG_LONG_INT
extern long long ssf_min_reduction(long long myval);
extern unsigned long long ssf_min_reduction(unsigned long long myval);
extern long long ssf_sum_reduction(long long myval);
extern unsigned long long ssf_sum_reduction(unsigned long long myval);
#endif
extern float ssf_sum_reduction(float  myval);
extern double ssf_sum_reduction(double myval);

#ifdef HAVE_MPI_H
extern void ssf_mpi_init_thread(int* argc, char*** argv, int required, int* provided);
extern void ssf_mpi_finalize();
extern void ssf_mpi_comm_size(MPI_Comm comm, int* size);
extern void ssf_mpi_comm_rank(MPI_Comm comm, int* rank);
extern void ssf_mpi_pack(void* inbuf, int incount, MPI_Datatype datatype,
			 void* outbuf, int outcount, int* position, MPI_Comm comm);
extern void ssf_mpi_unpack(void* inbuf, int insize, int* position,
			   void* outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm);
extern void ssf_mpi_send(void* buf, int count, MPI_Datatype datatype, int dest, int tag,
			 MPI_Comm comm);
extern void ssf_mpi_recv(void* buf, int count, MPI_Datatype datatype, int source, int tag,
			 MPI_Comm comm, MPI_Status* status);
extern void ssf_mpi_get_count(MPI_Status* status, MPI_Datatype datatype, int* count);
extern void ssf_mpi_iprobe(int source, int tag, MPI_Comm comm, int* flag, MPI_Status* status);
extern void ssf_mpi_buffer_attach(void* buffer, int size);
extern void ssf_mpi_buffer_detach(void* buffer, int* size);
extern void ssf_mpi_bsend(void* buf, int count, MPI_Datatype datatype, int dest, int tag,
			  MPI_Comm comm);
extern void ssf_mpi_barrier(MPI_Comm comm);
extern void ssf_mpi_reduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
			   MPI_Op op, int root, MPI_Comm comm);
extern void ssf_mpi_allreduce(void* sendbuf, void* recvbuf, int count,
			      MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
extern void ssf_mpi_reduce_scatter(void* sendbuf, void* recvbuf, int* recvcnts,
				   MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
extern void ssf_mpi_gather(void* sendbuf, int sendcnt, MPI_Datatype sendtype, 
			   void* recvbuf, int recvcnt, MPI_Datatype recvtype, 
			   int root, MPI_Comm comm);
extern void ssf_mpi_gatherv(void* sendbuf, int sendcnt, MPI_Datatype sendtype, 
			    void* recvbuf, int* recvcnts, int* displs, MPI_Datatype recvtype, 
			    int root, MPI_Comm comm);
extern void ssf_mpi_allgather(void* sendbuf, int sendcount, MPI_Datatype sendtype,
			      void* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
extern void ssf_mpi_allgatherv(void* sendbuf, int sendcount, MPI_Datatype sendtype,
			       void* recvbuf, int* recvcounts, int* displs, 
			       MPI_Datatype recvtype, MPI_Comm comm);
extern void ssf_mpi_alltoall(void* sendbuf, int sendcount, MPI_Datatype sendtype,
			     void* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm);
extern void ssf_mpi_alltoallv(void* sendbuf, int* sendcnts, int* sdispls, MPI_Datatype sendtype, 
			      void* recvbuf, int* recvcnts, int* rdispls, MPI_Datatype recvtype,
			      MPI_Comm comm);
#endif

}; /*namespace minissf*/

#endif /*__MINISSF_MACHINE_H__*/

/*
 * Copyright (c) 2011-2013 Florida International University.
 *
 * Permission is hereby granted, free of charg,e to any individual or
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
