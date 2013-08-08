# CMake config file for building simx

project(simx)

set(TARGET_NAME "core")

cmake_minimum_required(VERSION 2.6)
enable_language(CXX)

# if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)
#   #release comes with -O3 by default
#   set(CMAKE_BUILD_TYPE None CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
# endif(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CXX_FLAGS)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fpermissive" HAVE_FPERMISSIVE)
if(NOT HAVE_FPERMISSIVE)
  message(FATAL_ERROR "flag '-fpermissive' does not work")
endif(NOT HAVE_FPERMISSIVE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive" CACHE STRING "" FORCE)

#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS "-O2 -fpermissive")
set(CMAKE_CXX_FLAGS "-g -fpermissive")


# for MPI
find_package(MPI REQUIRED)
if (MPI_FOUND)
   include_directories(${MPI_INCLUDE_PATH} ${MPI_CXX_INCLUDE_PATH})
else()
  message(STATUS "mpi not found")
endif()


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
include_directories(${PYTHON_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIR} ${PYTHON_INCLUDE_PATH})
link_directories(${PYTHON_LIBRARIES})

file(GLOB_RECURSE SIMX_SOURCES src/simx/*.C)
file(GLOB_RECURSE NOT_SIMX_SOURCES src/simx/main*.C)
list(REMOVE_ITEM SIMX_SOURCES ${NOT_SIMX_SOURCES})

set(ALL_SOURCES "")
#set(ALL_SOURCES ${SIMX_SOURCES})


# for SSF
add_subdirectory(src/minissf)

list(APPEND ALL_SOURCES ${SIMX_SOURCES})

ADD_DEFINITIONS("-DSIMX_USE_PRIME")
include_directories("${CMAKE_SOURCE_DIR}/src/minissf")
include_directories("${CMAKE_SOURCE_DIR}/src/minissf/metis")
# below include to make sure ssf_config.h is visible
include_directories("${CMAKE_BINARY_DIR}/src/minissf")
#link_directories("${CMAKE_BINARY_DIR}/minissf")
#link_directories("${CMAKE_BINARY_DIR}/minissf/metis")

# simx	
include_directories(${CMAKE_SOURCE_DIR} src src/simx) 
#					${MPI_INCLUDE_PATH}
#					${Boost_INCLUDE_DIRS}
#					${PYTHON_INCLUDE_DIR})


#message(STATUS "ALL_SOURCES = ${ALL_SOURCES}")

add_library(${TARGET_NAME} SHARED
		    ${ALL_SOURCES})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/simx")
set_target_properties(${TARGET_NAME} PROPERTIES 
    PREFIX "" 
    SUFFIX ".so"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/simx )
    



target_link_libraries(${TARGET_NAME}
        ${Boost_LIBRARIES}
        ${PYTHON_LIBRARIES}
	${MPI_CXX_LIBRARIES} ${MPI_LIBRARIES})
#	ssf metis)

#add_library(simx_static ${SIMX_SOURCES} src/simx/Global/main_MPI.C)

include_directories(examples/HelloWorld/cpp)
#file(GLOB_RECURSE HELLOWORLD_SOURCES examples/HelloWorld/*.C src/simx/Global/main_MPI.C)
file(GLOB_RECURSE HELLOWORLD_SOURCES examples/HelloWorld/*.C src/simx/Global/main_dassf.C)
add_executable(HelloWorld ${HELLOWORLD_SOURCES})
target_link_libraries(HelloWorld ${TARGET_NAME})
