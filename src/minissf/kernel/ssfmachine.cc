#include <assert.h>
#include <sched.h>
#include <sys/time.h>
#include <unistd.h>
#include "kernel/ssfmachine.h"
#include "ssf.h"

namespace minissf {

struct pthread_child_struct {
  void (*child)(int);
  int pid;
};

static int num_children = 0;
static pthread_t* child_ids = 0;
static pthread_child_struct* child_structs = 0;

static void* pthread_child_starter(void* data)
{
  pthread_child_struct* pcs = (pthread_child_struct*)data;
  (*pcs->child)(pcs->pid);
  return 0;
}

void ssf_fork_children(int num, void (*child)(int))
{
  if(num_children > 0) SSF_THROW("can be called only once");
  if(num <= 0) { SSF_THROW("negative number of threads: " << num); }
  else if(num > 1) {
    num_children = num;
    child_ids = new pthread_t[num-1];
    assert(child_ids);
    child_structs = new pthread_child_struct[num-1];
    assert(child_structs);
  }

  for(int i=0; i<num-1; i++) {
    child_structs[i].child = child; 
    child_structs[i].pid = i+1;
    if(pthread_create(&child_ids[i], 0, pthread_child_starter, &child_structs[i]))
      SSF_THROW("failed to create pthread");
  }
  (*child)(0);
}

void ssf_join_children()
{
  // parent now waits for all children to complete
  for(int i=0; i<num_children-1; i++) {
    if(pthread_join(child_ids[i], 0))
      SSF_THROW("failed to join pthread");
  }
  if(child_ids) {
    assert(num_children > 0 && child_structs);
    delete[] child_ids; child_ids = 0;
    delete[] child_structs; child_structs = 0;
    num_children = 0;
  }
}

struct ssf_thread_child_struct {
  void (*child)(void*);
  void* data;
};

static void* ssf_thread_child(void* data)
{
  int tmp;
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &tmp);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &tmp);
  ssf_thread_child_struct* cs = (ssf_thread_child_struct*)data;
  (*cs->child)(cs->data);
  delete cs;
  return 0;
}

void ssf_thread_create(ssf_thread_t* tid, void (*child)(void*), void* data)
{
  ssf_thread_child_struct* cs = new ssf_thread_child_struct;
  cs->child = child; cs->data = data;
  if(pthread_create(tid, 0, ssf_thread_child, cs))
    SSF_THROW("failed to create pthread");
}

void ssf_thread_yield()
{
#ifdef _POSIX_PRIORITY_SCHEDULING
  sched_yield();
#else
  usleep(0); // force it to sleep if possible
#endif
}

#if defined(HAVE_GETHRTIME)
// wall time clock is available in nanosecond resolution; it's as good
// as it gets for most platforms
double ssf_wallclock_in_nanoseconds() { return (double)gethrtime(); }
#elif defined(HAVE_GETTIMEOFDAY)
// wall time clock is only in microseconds; it's the best precision we
// can get from the gettimeofday function
double ssf_wallclock_in_nanoseconds() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec*1e9+tv.tv_usec*1e3;
}
#else
#error "ERROR: missing timing support!"
#endif

static int ssf_thread_barrier_counter = 0;
static pthread_mutex_t ssf_thread_barrier_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ssf_thread_barrier_release = PTHREAD_COND_INITIALIZER;
static char** ssf_reduction_array;

void ssf_barrier_init()
{
  int nprocs = ssf_num_processors();
  ssf_reduction_array = new char*[nprocs]; assert(ssf_reduction_array);
  for(int i=0; i<nprocs; i++) {
    ssf_reduction_array[i] = new char[SSF_CACHE_LINE_SIZE];
    assert(ssf_reduction_array[i]);
  }
}

void ssf_barrier_wrapup()
{
  int nprocs = ssf_num_processors();
  for(int i=0; i<nprocs; i++) delete[] ssf_reduction_array[i];
  delete[] ssf_reduction_array;
}

void ssf_barrier()
{
  if(ssf_num_processors() == 1) return;
  pthread_mutex_lock(&ssf_thread_barrier_mutex);
  ssf_thread_barrier_counter++;
  if(ssf_thread_barrier_counter == ssf_num_processors()) {
    ssf_thread_barrier_counter = 0;
    pthread_cond_broadcast(&ssf_thread_barrier_release);
  } else {
    pthread_cond_wait(&ssf_thread_barrier_release, &ssf_thread_barrier_mutex);
  }
  pthread_mutex_unlock(&ssf_thread_barrier_mutex);
}

char ssf_min_reduction(char mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  char min = mymin;
  *(char*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    char tmp = *(char*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

char ssf_sum_reduction(char myval)
{
  if(ssf_num_processors() == 1) return myval;
  char sum = 0;
  *(char*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(char*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}

short ssf_min_reduction(short mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  short min = mymin;
  *(short*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    short tmp = *(short*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

short ssf_sum_reduction(short myval)
{
  if(ssf_num_processors() == 1) return myval;
  short sum = 0;
  *(short*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(short*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}

int ssf_min_reduction(int mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  int min = mymin;
  *(int*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    int tmp = *(int*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

int ssf_sum_reduction(int myval)
{
  if(ssf_num_processors() == 1) return myval;
  int sum = 0;
  *(int*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(int*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}

long ssf_min_reduction(long mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  long min = mymin;
  *(long*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    long tmp = *(long*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

long ssf_sum_reduction(long myval)
{
  if(ssf_num_processors() == 1) return myval;
  long sum = 0;
  *(long*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(long*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}

#ifdef HAVE_LONG_LONG_INT
long long ssf_min_reduction(long long mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  long long min = mymin;
  *(long long*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    long long tmp = *(long long*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

long long ssf_sum_reduction(long long myval)
{
  if(ssf_num_processors() == 1) return myval;
  long long sum = 0;
  *(long long*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(long long*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}
#endif

unsigned char ssf_min_reduction(unsigned char mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  unsigned char min = mymin;
  *(unsigned char*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    unsigned char tmp = *(unsigned char*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

unsigned char ssf_sum_reduction(unsigned char myval)
{
  if(ssf_num_processors() == 1) return myval;
  unsigned char sum = 0;
  *(unsigned char*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(unsigned char*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}

unsigned short ssf_min_reduction(unsigned short mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  unsigned short min = mymin;
  *(unsigned short*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    unsigned short tmp = *(unsigned short*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

unsigned short ssf_sum_reduction(unsigned short myval)
{
  if(ssf_num_processors() == 1) return myval;
  unsigned short sum = 0;
  *(unsigned short*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(unsigned short*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}

unsigned int ssf_min_reduction(unsigned int mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  unsigned int min = mymin;
  *(unsigned int*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    unsigned int tmp = *(unsigned int*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

unsigned int ssf_sum_reduction(unsigned int myval)
{
  if(ssf_num_processors() == 1) return myval;
  unsigned int sum = 0;
  *(unsigned int*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(unsigned int*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}

unsigned long ssf_min_reduction(unsigned long mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  unsigned long min = mymin;
  *(unsigned long*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    unsigned long tmp = *(unsigned long*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

unsigned long ssf_sum_reduction(unsigned long myval)
{
  if(ssf_num_processors() == 1) return myval;
  unsigned long sum = 0;
  *(unsigned long*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(unsigned long*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}

#ifdef HAVE_LONG_LONG_INT
unsigned long long ssf_min_reduction(unsigned long long mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  unsigned long long min = mymin;
  *(unsigned long long*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    unsigned long long tmp = *(unsigned long long*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

unsigned long long ssf_sum_reduction(unsigned long long myval)
{
  if(ssf_num_processors() == 1) return myval;
  unsigned long long sum = 0;
  *(unsigned long long*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(unsigned long long*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}

#endif

float ssf_min_reduction(float mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  float min = mymin;
  *(float*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    float tmp = *(float*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

float ssf_sum_reduction(float myval)
{
  if(ssf_num_processors() == 1) return myval;
  float sum = 0;
  *(float*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(float*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}

double ssf_min_reduction(double mymin)
{
  if(ssf_num_processors() == 1) return mymin;
  double min = mymin;
  *(double*)ssf_reduction_array[ssf_processor_index()] = mymin;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) {
    double tmp = *(double*)ssf_reduction_array[i];
    if(tmp < min) min = tmp;
  }
  ssf_barrier();
  return min;
}

double ssf_sum_reduction(double myval)
{
  if(ssf_num_processors() == 1) return myval;
  double sum = 0;
  *(double*)ssf_reduction_array[ssf_processor_index()] = myval;
  ssf_barrier();
  for(int i=0; i<ssf_num_processors(); i++) 
    sum += *(double*)ssf_reduction_array[i];
  ssf_barrier();
  return sum;
}

#ifdef HAVE_MPI_H
//#define DEBUG_SSF_MPI

#ifdef DEBUG_SSF_MPI
static const char* print_mpi_thread_level(int level)
{
  if(level == MPI_THREAD_SINGLE) return "MPI_THREAD_SINGLE";
  else if(level == MPI_THREAD_FUNNELED) return "MPI_THREAD_FUNNELED";
  else if(level == MPI_THREAD_SERIALIZED) return "MPI_THREAD_SERIALIZED";
  else if(level == MPI_THREAD_MULTIPLE) return "MPI_THREAD_MULTIPLE";
  else return "MPI_THREAD_UNKNOWN";
}

static const char* print_mpi_datatype(MPI_Datatype type)
{
  if(type == MPI_BYTE) return "MPI_BYTE";
  else if(type == MPI_CHAR) return "MPI_CHAR";
  else if(type == MPI_UNSIGNED_CHAR) return "MPI_UNSIGNED_CHAR";
  else if(type == MPI_SHORT) return "MPI_SHORT";
  else if(type == MPI_UNSIGNED_SHORT) return "MPI_UNSIGNED_SHORT";
  else if(type == MPI_INT) return "MPI_INT";
  else if(type == MPI_UNSIGNED) return "MPI_UNSIGNED";
  else if(type == MPI_LONG) return "MPI_LONG";
  else if(type == MPI_UNSIGNED_LONG) return "MPI_UNSIGNED_LONG";
  else if(type == MPI_LONG_LONG) return "MPI_LONG_LONG";
  else if(type == MPI_UNSIGNED_LONG_LONG) return "MPI_UNSIGNED_LONG_LONG";
  else if(type == MPI_FLOAT) return "MPI_FLOAT";
  else if(type == MPI_DOUBLE) return "MPI_DOUBLE";
  else if(type == MPI_LONG_DOUBLE) return "MPI_LONG_DOUBLE";
  else if(type == MPI_PACKED) return "MPI_PACKED";
  else return "MPI_DATATYPE_UNKNOWN";
    /*
      MPI_WCHAR:
      MPI_C_COMPLEX
      MPI_C_FLOAT_COMPLEX
      MPI_C_DOUBLE_COMPLEX
      MPI_C_LONG_DOUBLE_COMPLEX
      MPI_C_BOOL
      MPI_C_LONG_DOUBLE_COMPLEX
      MPI_INT8_T 
      MPI_INT16_T
      MPI_INT32_T 
      MPI_INT64_T
      MPI_UINT8_T 
      MPI_UINT16_T 
      MPI_UINT32_T 
      MPI_UINT64_T
    */
}
#endif

void ssf_mpi_init_thread(int* argc, char*** argv, int required, int* provided)
{
  if(MPI_Init_thread(argc, argv, required, provided) != MPI_SUCCESS)
    SSF_THROW("MPI_Init_thread failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("MPI_Init_thread: required=%s, provided=%s\n", 
	 print_mpi_thread_level(required), print_mpi_thread_level(*provided));
#endif
}

void ssf_mpi_finalize()
{
  if(MPI_Finalize() != MPI_SUCCESS)
    SSF_THROW("MPI_Finalize failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Finalize\n", Universe::args_rank);
#endif
}

void ssf_mpi_comm_size(MPI_Comm comm, int* size)
{
  if(MPI_Comm_size(comm, size) != MPI_SUCCESS)
    SSF_THROW("MPI_Comm_size failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("MPI_Comm_Size: size=%d\n", *size);
#endif
}

void ssf_mpi_comm_rank(MPI_Comm comm, int* rank)
{
  if(MPI_Comm_rank(comm, rank) != MPI_SUCCESS)
    SSF_THROW("MPI_Comm_rank failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("MPI_Comm_Rank: rank=%d\n", *rank);
#endif
}

void ssf_mpi_pack(void* inbuf, int incount, MPI_Datatype datatype,
		  void* outbuf, int outcount, int* position, MPI_Comm comm)
{
#ifdef DEBUG_SSF_MPI
  //int oldpos = *position;
#endif
  int inc; 
  if(MPI_Type_size(datatype, &inc) != MPI_SUCCESS) 
    SSF_THROW("MPI_Type_size failed: " << MPI_Error_string);
  if(*position + inc*incount > outcount) SSF_THROW("MPI packing buffer overflow");
  if(MPI_Pack(inbuf, incount, datatype, outbuf, outcount, position, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Pack failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  //printf("MPI_Pack: %d %s => buf[%d: %d:%d]\n", 
  //	 incount, print_mpi_datatype(datatype), outcount, oldpos, *position);
#endif
}

void ssf_mpi_unpack(void* inbuf, int insize, int* position,
		    void* outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm)
{
#ifdef DEBUG_SSF_MPI
  //int oldpos = *position;
#endif
  int inc; 
  if(MPI_Type_size(datatype, &inc) != MPI_SUCCESS) 
    SSF_THROW("MPI_Type_size failed: " << MPI_Error_string);
  if(*position + inc*outcount > insize) SSF_THROW("MPI unpacking buffer overflow");
  if(MPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Unpack failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  //printf("MPI_Unpack: buf[%d: %d:%d] => %d %s\n", 
  //	 insize, oldpos, *position, outcount, print_mpi_datatype(datatype));
#endif
}

void ssf_mpi_send(void* buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
#ifdef DEBUG_SSF_MPI
  printf("[%d] about to MPI_Send: %d %s to [%d], tag=%d\n", Universe::args_rank,
	 count, print_mpi_datatype(datatype), dest, tag);
#endif
  if(MPI_Send(buf, count, datatype, dest, tag, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Send failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] done MPI_Send\n", Universe::args_rank);
#endif
}

void ssf_mpi_recv(void* buf, int count, MPI_Datatype datatype, int source, int tag,
		  MPI_Comm comm, MPI_Status* status)
{
#ifdef DEBUG_SSF_MPI
  printf("[%d] about to MPI_Recv: %d %s from [%d], tag=%d\n", Universe::args_rank,
	 count, print_mpi_datatype(datatype), source, tag);
#endif
  if(MPI_Recv(buf, count, datatype, source, tag, comm, status) != MPI_SUCCESS)
    SSF_THROW("MPI_Recv failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] done MPI_Recv\n", Universe::args_rank);
#endif
}

void ssf_mpi_get_count(MPI_Status* status, MPI_Datatype datatype, int* count)
{
  if(MPI_Get_count(status, datatype, count) != MPI_SUCCESS)
    SSF_THROW("MPI_Get_count failed: " << MPI_Error_string);
  if(*count <= 0) SSF_THROW("invalid message count: " << *count);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Get_count: %d %s\n", Universe::args_rank, *count, 
	 print_mpi_datatype(datatype));
#endif
}

void ssf_mpi_iprobe(int source, int tag, MPI_Comm comm, int* flag, MPI_Status* status)
{
  if(MPI_Iprobe(source, tag, comm, flag, status) != MPI_SUCCESS)
    SSF_THROW("MPI_Iprobe failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Iprobe: src=%d, tag=%d => %d\n", Universe::args_rank, 
	 source, tag, *flag);
#endif
}

void ssf_mpi_buffer_attach(void* buffer, int size)
{
  if(MPI_Buffer_attach(buffer, size) != MPI_SUCCESS)
    SSF_THROW("MPI_Buffer_attach failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Buffer_attach: buffer size=%d\n", Universe::args_rank, size);
#endif
}

void ssf_mpi_buffer_detach(void* buffer, int* size)
{
  if(MPI_Buffer_detach(buffer, size) != MPI_SUCCESS)
    SSF_THROW("MPI_Buffer_detach failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Buffer_detach: buffer size=%d\n", Universe::args_rank, *size);
#endif
}

void ssf_mpi_bsend(void* buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
#ifdef DEBUG_SSF_MPI
  printf("[%d] about to MPI_Bsend: %d %s to [%d], tag=%d\n", Universe::args_rank,
	 count, print_mpi_datatype(datatype), dest, tag);
#endif
  if(MPI_Bsend(buf, count, datatype, dest, tag, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Bsend failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] done MPI_Bsend\n", Universe::args_rank);
#endif
}

void ssf_mpi_barrier(MPI_Comm comm)
{
  if(MPI_Barrier(comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Barrier failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Barrier\n", Universe::args_rank);
#endif
}

void ssf_mpi_reduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype,
		    MPI_Op op, int root, MPI_Comm comm)
{
  if(MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Reduce failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Reduce: %d %s, root=%d\n", Universe::args_rank, count,
	 print_mpi_datatype(datatype), root);
#endif
}

void ssf_mpi_allreduce(void* sendbuf, void* recvbuf, int count,
		       MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
  if(MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Allreduce failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Allreduce: %d %s\n", Universe::args_rank, count,
	 print_mpi_datatype(datatype));
#endif
}

void ssf_mpi_reduce_scatter(void* sendbuf, void* recvbuf, int* recvcnts,
			    MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
  if(MPI_Reduce_scatter(sendbuf, recvbuf, recvcnts, datatype, op, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Reduce_scatter failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Reduce_scatter: datatype=%s\n", Universe::args_rank, print_mpi_datatype(datatype));
#endif
}

void ssf_mpi_gather(void* sendbuf, int sendcnt, MPI_Datatype sendtype, 
		    void* recvbuf, int recvcnt, MPI_Datatype recvtype, 
		    int root, MPI_Comm comm)
{
  if(MPI_Gather(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Gather failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Gather: %d %s=>%d %s\n", Universe::args_rank, sendcnt,
	 print_mpi_datatype(sendtype), recvcnt, print_mpi_datatype(recvtype));
#endif
}

void ssf_mpi_gatherv(void* sendbuf, int sendcnt, MPI_Datatype sendtype, 
		     void* recvbuf, int* recvcnts, int* displs, MPI_Datatype recvtype, 
		     int root, MPI_Comm comm)
{
  if(MPI_Gatherv(sendbuf, sendcnt, sendtype, recvbuf, 
		 recvcnts, displs, recvtype, root, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Gatherv failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Gatherv: %d %s=>%s\n", Universe::args_rank, sendcnt,
	 print_mpi_datatype(sendtype), print_mpi_datatype(recvtype));
#endif
}

void ssf_mpi_allgather(void* sendbuf, int sendcount, MPI_Datatype sendtype,
		       void* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
  if(MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Allgather failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Allgather: %d %s=>%d %s\n", Universe::args_rank, sendcount,
	 print_mpi_datatype(sendtype), recvcount, print_mpi_datatype(recvtype));
#endif
}

void ssf_mpi_allgatherv(void* sendbuf, int sendcount, MPI_Datatype sendtype,
			void* recvbuf, int* recvcounts, int* displs, 
			MPI_Datatype recvtype, MPI_Comm comm)
{
  if(MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, 
		    recvcounts, displs, recvtype, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Allgatherv failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Allgatherv: %d %s=>%s\n", Universe::args_rank, sendcount,
	 print_mpi_datatype(sendtype), print_mpi_datatype(recvtype));
#endif
}

void ssf_mpi_alltoall(void* sendbuf, int sendcount, MPI_Datatype sendtype,
		      void* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
  if(MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Alltoall failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Alltoall: %d %s=>%d %s\n", Universe::args_rank, sendcount,
	 print_mpi_datatype(sendtype), recvcount, print_mpi_datatype(recvtype));
#endif
}

void ssf_mpi_alltoallv(void* sendbuf, int* sendcnts, int* sdispls, MPI_Datatype sendtype, 
		       void* recvbuf, int* recvcnts, int* rdispls, MPI_Datatype recvtype,
		       MPI_Comm comm)
{
  if(MPI_Alltoallv(sendbuf, sendcnts, sdispls, sendtype, 
		   recvbuf, recvcnts, rdispls, recvtype, comm) != MPI_SUCCESS)
    SSF_THROW("MPI_Alltoallv failed: " << MPI_Error_string);
#ifdef DEBUG_SSF_MPI
  printf("[%d] MPI_Alltoallv: %s=>%s\n", Universe::args_rank, 
	 print_mpi_datatype(sendtype), print_mpi_datatype(recvtype));
#endif
}

#endif

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
