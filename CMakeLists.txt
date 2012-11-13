# CMake config file for building simx

project(simx)

cmake_minimum_required(VERSION 2.6)
enable_language(CXX)
#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "-O2")

# for mpich
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

# simx	
include_directories(${CMAKE_SOURCE_DIR} src src/simx) 
#					${MPI_INCLUDE_PATH}
#					${Boost_INCLUDE_DIRS}
#					${PYTHON_INCLUDE_DIR})


file(GLOB_RECURSE SIMX_SOURCES src/simx/*.C)
file(GLOB_RECURSE NOT_SIMX_SOURCES src/simx/main*.C)
list(REMOVE_ITEM SIMX_SOURCES ${NOT_SIMX_SOURCES})

add_library(simx SHARED
		    ${SIMX_SOURCES})

set_target_properties(simx PROPERTIES PREFIX "" SUFFIX ".so")

target_link_libraries(simx
        ${Boost_LIBRARIES}
        ${PYTHON_LIBRARIES}
	${MPI_CXX_LIBRARIES})
