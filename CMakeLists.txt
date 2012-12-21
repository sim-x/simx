# CMake config file for building simx

project(simx)

cmake_minimum_required(VERSION 2.6)
enable_language(CXX)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)
  #release comes with -O3 by default
  set(CMAKE_BUILD_TYPE None CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fpermissive" HAVE_FPERMISSIVE)
if(NOT HAVE_FPERMISSIVE)
  message(FATAL_ERROR "flag '-fpermissive' does not work")
endif(NOT HAVE_FPERMISSIVE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive" CACHE STRING "" FORCE)

# for mpich
find_package(MPI REQUIRED)
if (NOT MPI_CXX_FOUND)
  message(FATAL_ERROR "No mpi c++ lib found")
endif()
include_directories(${MPI_CXX_INCLUDE_PATH})

# for boost
set(Boost_USE_STATIC_LIBS  OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS
		   python
		   REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ADD_DEFINITIONS( "-DHAS_BOOST" )
link_directories(${Boost_LIBRARY_DIRS})


# for python
find_package(PythonInterp REQUIRED VERSION 2)
find_package(PythonLibs REQUIRED VERSION 2)
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

#add_library(simx_static ${SIMX_SOURCES} src/simx/Global/main_MPI.C)

include_directories(examples/HelloWorld/cpp)
file(GLOB_RECURSE HELLOWORLD_SOURCES examples/HelloWorld/*.C src/simx/Global/main_MPI.C)
add_executable(HelloWorld ${HELLOWORLD_SOURCES})
target_link_libraries(HelloWorld simx)
