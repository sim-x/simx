# Author: Sunil Thulasidasan
# Date Created: May 7 2013

project(minissf)
cmake_minimum_required(VERSION 2.6)
enable_language(C)
enable_language(CXX)

add_subdirectory(metis)

include(ssf-configure-check.txt)

set(CMAKE_CXX_FLAGS "-O2")

#for MPI
find_package(MPI REQUIRED)
if (MPI_CXX_FOUND)
  include_directories(${MPI_CXX_INCLUDE_PATH})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} metis)

file(GLOB SSF_RANDOM_SOURCES 	
  # random sources
  random/random.cc 
  random/mersenne_twister.cc 
  random/lecuyer.cc 
  random/sprng.cc 
  random/sprng_checkid.c 
  random/sprng_cmrg.c 
  random/sprng_lcg.c 
  random/sprng_lcg64.c 
  random/sprng_lfg.c 
  random/sprng_makeseed.c 
  random/sprng_mlfg.c 
  random/sprng_primes_32.c 
  random/sprng_primes_64.c 
  random/sprng_sprng.c 
  random/sprng_store.c
  )

file(GLOB SSF_API_SOURCES
  ssfapi/virtual_time.cc 
  ssfapi/compact_datatype.cc 
  ssfapi/quick_memory.cc 
  ssfapi/ssf_timer.cc 
  ssfapi/ssf_semaphore.cc 
  ssfapi/event.cc 
  ssfapi/procedure.cc 
  ssfapi/process.cc 
  ssfapi/entity.cc 
  ssfapi/inchannel.cc 
  ssfapi/outchannel.cc
  )

file(GLOB SSF_EVTLIST_SOURCES )

file(GLOB SSF_KERNEL_SOURCES
  kernel/ssfmachine.cc 
  kernel/kernel_event.cc 
  kernel/binque.cc 
  kernel/timeline.cc 
  kernel/stargate.cc 
  kernel/universe.cc 
  kernel/universe_cmdline.cc 
  kernel/universe_mapping.cc 
  kernel/universe_sched.cc 
  kernel/universe_align.cc 
  kernel/ssf.cc
  )

list(APPEND MINISSF_SOURCES ${SSF_RANDOM_SOURCES} 
  ${SSF_API_SOURCES} ${SSF_EVTLIST_SOURCES} 
  ${SSF_KERNEL_SOURCES})

add_library(ssf SHARED ${MINISSF_SOURCES})

#link_directories("/metis/build")
target_link_libraries(ssf ${MPI_CXX_LIBRARIES})
