# Author: Christoph Junghans

project(simx)
set(TARGET_NAME "core")
cmake_minimum_required(VERSION 2.6)
enable_language(CXX)

# Cmake modules/macros are in a subdirectory to keep this file cleaner
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)
  #release comes with -O3 by default
  set(CMAKE_BUILD_TYPE None CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fpermissive" HAVE_FPERMISSIVE)
if(NOT HAVE_FPERMISSIVE)
  message(FATAL_ERROR "flag '-fpermissive' does not work")
endif(NOT HAVE_FPERMISSIVE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

#for MPI
option(SIMX_USE_MPI "Build Simx with MPI" ON)
option(SIMX_USE_PRIME "Build Simx with prime" ON)
if(SIMX_USE_MPI)
    find_package(MPI REQUIRED)
    if (MPI_FOUND)
	include_directories(${MPI_INCLUDE_PATH} ${MPI_CXX_INCLUDE_PATH})
	ADD_DEFINITIONS("-DHAVE_MPI_H")
    else()
	message(FATAL_ERROR "MPI not found. Install MPI or build without MPI (-DSIMX_USE_MPI=0)")
    endif()
else()
    #compiling without MPI, make sure we're using SSF
    if(NOT SIMX_USE_PRIME)
	message(FATAL_ERROR "non-MPI build must use SSF. Rebuild with SSF enabled (-DSIMX_USE_PRIME=1)")
    endif()
endif()

# for python
find_package(PythonInterp 2 REQUIRED)
find_package(PythonLibs 2 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIR} ${PYTHON_INCLUDE_PATH})
link_directories(${PYTHON_LIBRARY_DIRS})

execute_process(COMMAND ${PYTHON_EXECUTABLE} 
                -c "import distutils.sysconfig as cg; print cg.get_python_lib(1,0,prefix='${CMAKE_INSTALL_EXEC_PREFIX}')"
                OUTPUT_VARIABLE PYTHON_INSTDIR OUTPUT_STRIP_TRAILING_WHITESPACE)

# for boost
set(Boost_USE_STATIC_LIBS  OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS
		   python
		   REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ADD_DEFINITIONS( "-DHAS_BOOST" )
link_directories(${Boost_LIBRARY_DIRS})

# for simx sources
file(GLOB_RECURSE SIMX_SOURCES src/simx/*.C)
file(GLOB_RECURSE NOT_SIMX_SOURCES src/simx/main*.C)
list(REMOVE_ITEM SIMX_SOURCES ${NOT_SIMX_SOURCES})

# for SSF
if (SIMX_USE_PRIME)
  ADD_DEFINITIONS("-DSIMX_USE_PRIME")
  add_subdirectory(src/minissf)
  include_directories("${CMAKE_SOURCE_DIR}/src/minissf")
  include_directories("${CMAKE_SOURCE_DIR}/src/minissf/metis")
  # below include to make sure ssf_config.h is visible
  include_directories("${CMAKE_BINARY_DIR}/src/minissf")
endif()
 
include_directories(${CMAKE_SOURCE_DIR} src src/simx) 
add_library(${TARGET_NAME} SHARED ${SIMX_SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/simx/core"
    PREFIX "" 
    SUFFIX ".so")
  
list(APPEND SIMX_LINK_LIBRARIES ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
if(SIMX_USE_MPI)
    list(APPEND SIMX_LINK_LIBRARIES ${MPI_CXX_LIBRARIES} ${MPI_LIBRARIES})
endif()

target_link_libraries(${TARGET_NAME} ${SIMX_LINK_LIBRARIES})

install(TARGETS ${TARGET_NAME} LIBRARY DESTINATION ${PYTHON_INSTDIR}/simx/core)

#build SimX.rc
#CMAKE_BINARY_DIR for espresso, CMAKE_CURRENT_BINARY_DIR for for _espressos.so and contrib for MPI.so
set (PYTHON_DIR "${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/simx")
configure_file(${CMAKE_SOURCE_DIR}/simx/SimX.cmakein ${CMAKE_BINARY_DIR}/SimX.rc @ONLY)

file(GLOB_RECURSE PYTHON_SCRIPTS RELATIVE ${CMAKE_SOURCE_DIR} simx/*.py)
add_custom_target(scripts ALL COMMENT "Copying scripts to builddir: *.py")
foreach(FILENAME ${PYTHON_SCRIPTS})
  set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
  set(DST "${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}")
  add_custom_command(TARGET scripts COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC} ${DST})
  string(REGEX MATCH .*/ DIR "${FILENAME}")
  install(FILES ${DST} DESTINATION ${PYTHON_INSTDIR}/${DIR})
endforeach(FILENAME)

#######################
# Helloworld stuff. 
#######################
# include_directories(examples/HelloWorld/cpp)
# if (SIMX_USE_PRIME)
#   file(GLOB_RECURSE HELLOWORLD_SOURCES examples/HelloWorld/*.C src/simx/Global/main_dassf.C) 
# else()
#   file(GLOB_RECURSE HELLOWORLD_SOURCES examples/HelloWorld/*.C src/simx/Global/main_MPI.C)
# endif()
# add_executable(HelloWorld ${HELLOWORLD_SOURCES})
# target_link_libraries(HelloWorld ${TARGET_NAME})

## end Helloworld stuff
