add_subdirectory(metis)

include(ssf-configure-check.txt)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} metis)

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

list(APPEND SIMX_SOURCES ${SSF_API_SOURCES} ${SSF_KERNEL_SOURCES} )

set(SIMX_SOURCES "${SIMX_SOURCES}" PARENT_SCOPE)
