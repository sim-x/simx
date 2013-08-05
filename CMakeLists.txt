# CMake config file for building simx

project(simx)

cmake_minimum_required(VERSION 2.6)
enable_language(CXX)
#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS "-O2 -fpermissive")
set(CMAKE_CXX_FLAGS "-g -fpermissive")

# for MPI
find_package(MPI REQUIRED)
if (MPI_CXX_FOUND)
   include_directories(${MPI_CXX_INCLUDE_PATH})
endif()

# for boost
set(Boost_USE_STATIC_LIBS  OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS
		   python
		   REQUIRED)
IF (Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  ADD_DEFINITIONS( "-DHAS_BOOST" )
  link_directories(${Boost_LIBRARY_DIRS})
ENDIF()
 
#include_directories(${Boost_INCLUDE_DIRS})


# for python
find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${PYTHON_LIBRARIES})

file(GLOB_RECURSE SIMX_SOURCES src/simx/*.C)
file(GLOB_RECURSE NOT_SIMX_SOURCES src/simx/main*.C)
list(REMOVE_ITEM SIMX_SOURCES ${NOT_SIMX_SOURCES})

set(ALL_SOURCES ${SIMX_SOURCES})

# for SSF
add_subdirectory(src/minissf)
ADD_DEFINITIONS("-DSIMX_USE_PRIME")
include_directories("${CMAKE_SOURCE_DIR}/src/minissf")
# below include to make sure ssf_config.h is visible
include_directories("${CMAKE_BINARY_DIR}/src/minissf")
link_directories("${CMAKE_BINARY_DIR}/minissf")
link_directories("${CMAKE_BINARY_DIR}/minissf/metis")

# simx	
include_directories(${CMAKE_SOURCE_DIR} src src/simx) 
#					${MPI_INCLUDE_PATH}
#					${Boost_INCLUDE_DIRS}
#					${PYTHON_INCLUDE_DIR})


message(STATUS "ALL_SOURCES = ${ALL_SOURCES}")

add_library(simx SHARED
		    ${ALL_SOURCES})

set_target_properties(simx PROPERTIES PREFIX "" SUFFIX ".so")

target_link_libraries(simx
        ${Boost_LIBRARIES}
        ${PYTHON_LIBRARIES}
	${MPI_CXX_LIBRARIES})
#	ssf metis)

#add_library(simx_static ${SIMX_SOURCES} src/simx/Global/main_MPI.C)

include_directories(examples/HelloWorld/cpp)
#file(GLOB_RECURSE HELLOWORLD_SOURCES examples/HelloWorld/*.C src/simx/Global/main_MPI.C)
file(GLOB_RECURSE HELLOWORLD_SOURCES examples/HelloWorld/*.C src/simx/Global/main_dassf.C)
add_executable(HelloWorld ${HELLOWORLD_SOURCES})
target_link_libraries(HelloWorld simx)
