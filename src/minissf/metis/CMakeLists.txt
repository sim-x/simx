include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB METIS_SOURCES 
  balance.c
  bucketsort.c
  ccgraph.c
  coarsen.c
  compress.c
  debug.c
  estmem.c
  fm.c
  fortran.c
  frename.c
  graph.c
  initpart.c
  kmetis.c
  kvmetis.c
  kwayfm.c
  kwayrefine.c
  kwayvolfm.c
  kwayvolrefine.c
  match.c
  mbalance2.c
  mbalance.c
  mcoarsen.c
  memory.c
  mesh.c
  meshpart.c
  mfm2.c
  mfm.c
  mincover.c
  minitpart2.c
  minitpart.c
  mkmetis.c
  mkwayfmh.c
  mkwayrefine.c
  mmatch.c
  mmd.c
  mpmetis.c
  mrefine2.c
  mrefine.c
  mutil.c
  myqsort.c
  myrng.c
  ometis.c
  parmetis.c
  pmetis.c
  pqueue.c
  refine.c
  separator.c
  sfm.c
  srefine.c
  stat.c
  subdomains.c
  timing.c
  util.c )

list(APPEND SIMX_SOURCES ${METIS_SOURCES} )
set(SIMX_SOURCES "${SIMX_SOURCES}" PARENT_SCOPE )
