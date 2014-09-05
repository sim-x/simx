// Copyright (c) 2012. Los Alamos National Security, LLC. 

// This material was produced under U.S. Government contract DE-AC52-06NA25396
// for Los Alamos National Laboratory (LANL), which is operated by Los Alamos 
// National Security, LLC for the U.S. Department of Energy. The U.S. Government 
// has rights to use, reproduce, and distribute this software.  

// NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, 
// EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  
// If software is modified to produce derivative works, such modified software should
// be clearly marked, so as not to confuse it with the version available from LANL.

// Additionally, this library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License v 2.1 as published by the 
// Free Software Foundation. Accordingly, this library is distributed in the hope that 
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See LICENSE.txt for more details.

//--------------------------------------------------------------------------
// File:    simEngine.C
// Module:  simx
// Author:  Lukas Kroc, Sunil Thulasidasan
// Created: Feb 25 2010
//
// @@
//
//--------------------------------------------------------------------------

#ifndef SIMX_USE_PRIME

#include <iostream>
#include <algorithm>

#ifdef HAVE_MPI_H
#include "mpi.h"
#endif

#include "pthread.h"

#include "simx/simEngine.h"
#include "simx/EventInfo.h"
#include "simx/logger.h"
#include "simx/PackedData.h"
#include "simx/EventQueue.h"
#include "simx/LP.h"
#include "simx/control.h"

#include <limits>
#include <assert.h>


using namespace std;

// private stuff 
namespace {

using namespace simx;


int g_num_proc;		// number of processes
int g_my_rank;		// rank of process

#ifdef HAVE_MPI_H
// MPI STUFF
MPI_Datatype g_mpi_time_type;	// type of simx time
const int g_eventinfo_tag = 1;	//< tag for EventInfos
MPI_Comm g_comm_events;	//< the communicator for events
MPI_Comm g_comm_sync;	//< the communicator for sync
#endif

// TIMING
Time g_time_start;	// when the sim starts
Time g_time_end;	// when the sim ends
Time g_time_now;	// CURRENT simulation time
Time g_time_next_sent = numeric_limits<Time>::max();	//< the estimate of the next event from all we sent out each epoch


// EVENT QUEUE
EventQueue		g_eq;		//< EVENT QUEUE

#ifdef HAVE_MPI_H
pthread_mutex_t 	g_eqlock;	// for for the event queue access
#endif


#ifdef HAVE_MPI_H
// THE LISTENING THREAD FUNCTION:
// (listens for any messages coming in, and puts them into Evetn Queue)
void* listeningThread(void*)
{
    int tmp;	//< various returns for asserts
    MPI_Status status;
    
    // buffer to receive messages into
    int buffer_size = 100;
    char* buffer = (char*)malloc( buffer_size );
    SMART_ASSERT( buffer );

    Logger::info() << "LISTENING THREAD START: listening thread started" << endl;

    // just listen until you get a command message
    bool done = false;
    while( !done )
    {
	// 1) listen to any any message from anybody:
	// probe it and find out its size:
//	Logger::debug3() << "[listening thread]: waiting for a message" << endl;
	MPI_Probe( MPI_ANY_SOURCE, g_eventinfo_tag, g_comm_events, &status );
	    int count;	//< size of the incoming message
	    MPI_Get_count( &status, MPI_BYTE, &count );
	    if( count == MPI_UNDEFINED )
		Logger::failure("Cannot get the size of incoming message");
	    SMART_ASSERT( count >= 1 )( count );

	// make sure the buffer is large enough
	if( count > buffer_size )
	{
	    buffer_size = max( count, 2*buffer_size );
	    buffer = (char*)realloc( buffer, buffer_size );
	    SMART_ASSERT( buffer );
	}
	// then actually receive it:
//	Logger::debug3() << "[listening thread]: receiving for a message" << endl;
	MPI_Recv( buffer, count, MPI_BYTE, MPI_ANY_SOURCE, g_eventinfo_tag, g_comm_events, &status );
	if( status.MPI_ERROR != MPI_SUCCESS )
	{
	    char error_msg[MPI_MAX_ERROR_STRING];
	    int len;
	    MPI_Error_string( status.MPI_ERROR, error_msg, &len );
	    Logger::error() << "simEngine.C: error in MPI_Recv: " << error_msg << endl;
	}

	// see if we got a special command
	if( count == 1 )
	{
	    switch( buffer[0] )
	    {
	    case 'q':
		// end listening
		done = true;
		continue;
		break;
	    default:
		Logger::error() << "simEngine.C: unknown command for listening thread received: " << buffer[0] << endl;
		continue;
	    }
	}
	
	// 2) unpack the message (of the correck kind):
	PackedData pd( buffer, count );
	EventInfo e;
	e.unpack( pd );
	
	// 3) put it into the event queue
	tmp = pthread_mutex_lock( &g_eqlock );
	SMART_ASSERT( tmp == 0 )(tmp);
	g_eq.push( e.getTime(), e );
	tmp = pthread_mutex_unlock( &g_eqlock );
	SMART_ASSERT( tmp == 0 )(tmp);
	
	if( e.getTime() < g_time_now )
	{
	    Logger::warn() << "simEngine.C: received a delayed message, with time=" << e.getTime() << endl;
	}
	
//	Logger::debug3() << "[listening thread]: message is unpacked and in the queue" << endl;
    }
    free( buffer );

    Logger::info() << "LISTENING THREAD DONE: listening thread done" << endl;
    pthread_exit(NULL);
}
#endif

} // unnamed namespace



namespace simx {

namespace SimEngine {

//=============================================================
//=============================================================


// initializes MPI, if enabled
void init()
{
#ifdef HAVE_MPI_H
    // MPI MUST BE RUNNING (done in framework/Global/main_MPI.C)

    // create our communicators:
    MPI_Comm_dup( MPI_COMM_WORLD, &g_comm_sync );	//< used ONLY for sync
    MPI_Comm_dup( MPI_COMM_WORLD, &g_comm_events );	//< used for sending and receiving infos
    
    // Find out process rank
    MPI_Comm_rank( g_comm_events, &g_my_rank );
    
    // Find out number of processes
    MPI_Comm_size( g_comm_events, &g_num_proc );
    
    if( typeid( Time ) == typeid(int) )
	g_mpi_time_type = MPI_INT;
    else if( typeid( Time ) == typeid(long) )
	g_mpi_time_type = MPI_LONG;
    else if( typeid( Time ) == typeid(float) )
	g_mpi_time_type = MPI_FLOAT;
    else if( typeid( Time ) == typeid(double) )
	g_mpi_time_type = MPI_DOUBLE;
    else if( typeid( Time ) == typeid(unsigned long long) )
        g_mpi_time_type = MPI_UNSIGNED_LONG_LONG;
    else Logger::failure("Unsupported simx::Time type in SimEngine::init()");
#else
    g_my_rank = 0;
    g_num_proc = 1;
#endif
    if( ! (sizeof( EventInfo ) > 1 ) ) 
    {
	Logger::failure("EventInfo must be at least 2 bytes large");
	// this is because simEngine uses size==1 messes for special purposes
	// as commands to listening thread
    }
}

// returns the number of machines in the MPI world; will be 1
// if MPI is not enabled
int getNumMachs()
{
    return g_num_proc;
}

// returns the rank of this machine; will be 0 if MPI is not enabled
int getRank()
{
    return g_my_rank;
}

// 'prepares' for the simulation to be started
void prepare(Time start, Time stop)
{
    g_time_start = start;
    g_time_end = stop;
#ifdef HAVE_MPI_H
    int lockRet =  pthread_mutex_init( &g_eqlock, NULL);
    SMART_VERIFY( lockRet == 0)( lockRet ).msg("Cannot initialize thread queue lock");
#endif
       
}

// runs the simulation, does not return untill the end
void run()
{
#ifdef HAVE_MPI_H
    // START A SEPARATE THREAD FOR LISTENING TO INCOMMING MESSAGES
    Logger::info() << "MAIN THREAD START: creating other threads" << endl;

    
    pthread_t ltId;
    int threadRet = pthread_create(&ltId, NULL, listeningThread, NULL);
    SMART_VERIFY( threadRet == 0)( threadRet ).msg("Cannot create a thread");

    int tmp;	//< various ret values for asserts
    Time base_time = g_time_start;	//< the time we last synchronized
    
    while( base_time <= g_time_end )
    {
	g_time_now = base_time;
	g_time_next_sent = numeric_limits<Time>::max();
	Time next_time = g_time_end+1;	//< the time for next event
    
	// 2) do something now, untill you are more than MINDELAY away from base_time, or have no more events
	Logger::info() << "A: working...." << endl;
	bool done = false;	//< done with this timestep?
	while( true )
	{
	    // 2a) see if there is an event for us to do this time unit
	    EventInfo e;
	    tmp = pthread_mutex_lock( &g_eqlock );
	    SMART_ASSERT( tmp == 0 )(tmp);
	    if( g_eq.empty() )
	    {
		done = true;
	    } else
	    {
		e = g_eq.top();
		if( e.getTime() >= base_time + LP::MINDELAY )
		{
		    next_time = e.getTime();
		    done = true;
		} else
		{
		    g_eq.pop();
		}
	    }
	    tmp = pthread_mutex_unlock( &g_eqlock );
	    SMART_ASSERT( tmp == 0 )(tmp);
	    if( done )
		break;

	    // 2b) if so, advance time:
	    if( e.getTime() < g_time_now )
	    {
		Logger::warn() << "simEngine.C: popped event in the past " << e.getTime() << endl;
		e.setTime( g_time_now );
	    }
	    g_time_now = e.getTime();
	    
	    // you might be beyond end_time due to MINDELAY
	    if( g_time_now > g_time_end )
		break;
	    
    	    Logger::debug2() << "Executing event: " << e << endl;

	    /// 2c) and execute the event
	    /// main try{} catch{} loop of simx
	    try {
    		e.execute();
	    }
	    catch(const Exception& ex)
	    {
		switch( ex.getLevel() )
		{
		    case Exception::kINFO:
			Logger::debug2() << "Exception: " << ex.getDescription() << endl;
			break;
		    case Exception::kWARN:
			Logger::warn() << "Exception: " << ex.getDescription() << endl;
			break;
		    case Exception::kERROR:
			Logger::error() << "Exception: " << ex.getDescription() << endl;
			break;
		    case Exception::kFATAL:
			Logger::error() << "FATAL Exception: " << ex.getDescription() << endl;
			Logger::failure( "FATAL Exception caught");
			break;
		    default:
			Logger::error() << "(UNKNOWN) Exception: " << ex.getDescription() << endl;
		}
	    }
	    catch(const std::exception& ex)
	    {
		SMART_ASSERT( ex.what() );
		Logger::error() << "simEngine.C: std::exception: " << ex.what() << endl;
	    }


	} // while( !g_eq.empty() ) or out of this time
	

	// 3) find out what the next base_time is (SYNC)
	Logger::info() << "B: waiting...." << endl;
	next_time = min( next_time, g_time_next_sent );	//< you must include the receive time of the sent events, to make sure pending events don't mess with the earliest time
	MPI_Allreduce( &next_time, &base_time, 1, g_mpi_time_type, MPI_MIN, g_comm_sync );
	
    } // while( base_time <= g_time_end )
    
    Logger::info() << "MAIN THREAD DONE: waiting for other threads" << endl;
    // send a quit command to the listening thread
    char command = 'q';
    MPI_Send( &command, 1, MPI_BYTE, g_my_rank, g_eventinfo_tag, g_comm_events );
    pthread_join(ltId, NULL);
//    pthread_cancel( ltId );

    pthread_mutex_destroy( &g_eqlock );

#else  // MPI not enabled

    Logger::info() << "SimEngine: Starting simulation" << endl;
    //Time base_time = g_time_start;
    g_time_now = g_time_start;
    while( g_time_now <= g_time_end)
      {
	//g_time_now = base_time;
	//Time next_tie = g_time_end + 1;
	//bool done = false;
	//while (true)
	//  {
	    EventInfo e;
	    if ( g_eq.empty() )
	      {
		//done = true;
		break;
	      }
	    else
	      {
		e = g_eq.top();
		g_eq.pop();
	      }
	    //if (done)
	    //  break;
	    SMART_ASSERT( e.getTime() >= g_time_now)(e.getTime())
	      (g_time_now);
	    g_time_now = e.getTime();
	    Logger::debug2() << "Executing event:" << e << endl;
	     /// 2c) and execute the event
	    /// main try{} catch{} loop of simx
	    try {
	      e.execute();
	    }
	    catch(const Exception& ex)
	      {
		switch( ex.getLevel() )
		  {
		  case Exception::kINFO:
		    Logger::debug2() << "Exception: " << ex.getDescription() << endl;
		    break;
		  case Exception::kWARN:
		    Logger::warn() << "Exception: " << ex.getDescription() << endl;
		    break;
		  case Exception::kERROR:
		    Logger::error() << "Exception: " << ex.getDescription() << endl;
		    break;
		  case Exception::kFATAL:
		    Logger::error() << "FATAL Exception: " << ex.getDescription() << endl;
		    Logger::failure( "FATAL Exception caught");
		    break;
		  default:
		    Logger::error() << "(UNKNOWN) Exception: " << ex.getDescription() << endl;
		  }
	      }
	    catch(const std::exception& ex)
	      {
		SMART_ASSERT( ex.what() );
		Logger::error() << "simEngine.C: std::exception: " << ex.what() << endl;
	      }
	    //	  }
      }
    Logger::info() << "SimEngine: Simulation Done" << endl;
    
#endif
}

// shuts down MPI
void finalize()
{
    // MPI is shut down in framework/Global/main_MPI.C
  
  // finalize event queue

  if ( !  g_eq.empty() )
    Logger::warn() << "SimEngine:  Unprocessed events in the event queue"
		   << " at end of simulation" << endl;
  // clear out events from event queue before python erases them.
  g_eq.finalize();
  
#ifdef HAVE_MPI_H
  MPI_Comm_free( &g_comm_events );
  MPI_Comm_free( &g_comm_sync );
#endif
}

//=============================================================
//=============================================================

Time getNow()
{
    return g_time_now;
}

// packs an event, and sends it off
// DO NOT PACK AND SEND EVENTS TO YOURSELF
void sendEventInfo( LPID destLP, const EventInfo& e )
{
    Logger::debug3() << "Sending EventInfo " << e << " for " << destLP << endl;

#ifdef HAVE_MPI_H
    if( g_my_rank == destLP )
    {
	Logger::debug3() << "    .... no need to pack it" << endl;

	int tmp = pthread_mutex_lock( &g_eqlock );
	SMART_ASSERT( tmp == 0 )(tmp);
	g_eq.push( e.getTime(), e );
	tmp = pthread_mutex_unlock( &g_eqlock );
	SMART_ASSERT( tmp == 0 )(tmp);
	  
    } else
    {
	Logger::debug3() << "    .... packing it" << endl;
	PackedData dp;
	e.pack( dp );
    
	// send it off! (might block, but when it finishes, I can delete dp()
	// tag==1 means EventInfo
	Logger::debug3() << "    .... sending it to " << destLP << endl;

	g_time_next_sent = min( g_time_next_sent, e.getTime() );
	
	// TODO: the synchronous send here slows things down a lot, but 
	// without it, some listeningThread on some LP can get bogged down,
	// resulting in messages received too late.
	// Better solution would be to send 'empty' messages at the end of
	// each time-step to all recipients that you send anything to
	// in it, send that synchronously, and send the rest using normal send. 
	MPI_Ssend( dp.getMem(), dp.getLength(), MPI_BYTE, destLP, g_eventinfo_tag, g_comm_events );
    }
#else // MPI  not enabled
    g_eq.push( e.getTime(), e );
#endif
}

} // namespace SimEngine

} // namespace simx

#endif
