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

// $Id: Messenger.C,v 1.4 2010/02/28 05:01:39 kroc Exp $
//--------------------------------------------------------------------------
// File:    Messenger.C
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: Oct 09 2008
//
//	Functions for handling out-of-band messaging
//
// @@
//
//--------------------------------------------------------------------------

#ifdef HAVE_MPI_H
#ifdef SIMX_USE_PRIME



#include "simx/Messenger.h"


#include "simx/ControlInfoWrapper.h"
#include "mpi.h"
#include "ssf.h"
#include "simx/PackedData.h"
#include "boost/tuple/tuple.hpp"
#include "boost/shared_array.hpp"
#include <list>
#include "Common/Exception.h"
#include <cstdio>


using namespace std;

namespace simx {
  
  namespace Messenger {
    
    typedef boost::tuple<MPI_Request, boost::shared_array<char> >
    pending_request_t;
    
    typedef std::list<pending_request_t> pending_list_t;
    
    pending_list_t fPending;
    typedef std::vector<ControlInfoWrapper*> control_messages_t;
    control_messages_t fRecvdMessages;
    
    //mpi globals
    MPI_Comm SIMX_WORLD;
    MPI_Group new_group;
    MPI_Group orig_group;
    
    MPI_Request fRecvReq;
    
    const int SIMX_CONTROL_TAG = 1;
    
    const int MSG_BUF_SIZE = 1048576; // 1 MB
    const int SND_MSG_BUF_SIZE = 1024; // 

    //char sendBuf[MSG_BUF_SIZE];
    char fRecvBuf[MSG_BUF_SIZE];
    
    int fMRank; //MPI rank of this messenger
    bool fMessengerActive = false;

//-------------------------------------------------------------------------

    bool MessengerActive() 
    {
      return fMessengerActive;
    }

//-------------------------------------------------------------------------

    int getMessengerRank() 
    {
      if (MessengerActive())
	return fMRank;
      else
	return -1 ;
    }

 
//-------------------------------------------------------------------------
    
    void handleMPIReturn(const std::string& where, int rc) {
      if(rc != MPI_SUCCESS) {
	// throw Exception with MPI error string
	char errStr[MPI_MAX_ERROR_STRING + 1]; // for null termination
	int resultlen;
	MPI_Error_string(rc, errStr, &resultlen);
	errStr[resultlen] = '\0';
	throw Common::Exception( "(" + where + ") MPI Error:" + errStr);  
      }
    }


 
//-------------------------------------------------------------------------
    

    bool completed( pending_request_t pending )
    {
      return ( pending.get<0>() == MPI_REQUEST_NULL );
    }

//-------------------------------------------------------------------------

    void sendProgress() 
    {
      static string where = "simx::Messenger::sendProgress()";
      for ( pending_list_t::iterator iter = fPending.begin();
	    iter != fPending.end(); ++iter )
	{
	  int flag;
	  handleMPIReturn( where,
			   MPI_Test( &( iter->get<0>() ), &flag,
				     MPI_STATUS_IGNORE ));
	}
      fPending.remove_if( completed );
    }

//-------------------------------------------------------------------------

  void initReceive()
  {
    static string where = "simx::Messenger::initReceive()";
    handleMPIReturn( where, MPI_Irecv( fRecvBuf, MSG_BUF_SIZE, 
				       MPI_PACKED, MPI_ANY_SOURCE,
				       SIMX_CONTROL_TAG, SIMX_WORLD,
				       &fRecvReq ) );
  }
    
//-------------------------------------------------------------------------
    
    void processReceivedMessages()
    {
      SMART_ASSERT( ! fRecvdMessages.empty() );
      for ( unsigned int i = 0; i < fRecvdMessages.size(); i++ )
	{
	  fRecvdMessages[i]->execute();
	  delete fRecvdMessages[i];
	}
      fRecvdMessages.clear();
      
    }
//-------------------------------------------------------------------------
    
    void unpackControlMessage( minissf::CompactDataType* mesg )
    {
      PackedData pd( mesg );
      ControlInfoWrapper* cinfo = new ControlInfoWrapper;
      cinfo->unpack( pd );
      fRecvdMessages.push_back( cinfo );
    }


//-------------------------------------------------------------------------

    void recvPackedMessage( int recv_size )
    {
      //TODO: does not compile

      // static string where  = "simx::Messenger::recvPackedMessage()";
      // int pos = 0;
      // while ( pos < recv_size )
      // 	{
      // 	  LPID srcLP, destLP;
      // 	  handleMPIReturn( where, MPI_Unpack( fRecvBuf, recv_size, &pos,
      // 					      &srcLP, 1, MPI_INT, SIMX_WORLD ));
      // 	  handleMPIReturn( where, MPI_Unpack( fRecvBuf, recv_size, &pos, 
      // 					      &destLP, 1, MPI_INT, SIMX_WORLD ));
      // 	  SMART_ASSERT( destLP == getMessengerRank() )( destLP )( getMessengerRank() )
      // 	    .msg("simx::Messenger::recvPackedMessage(): destLP and Messenger rank do not match ");
	  
      // 	  minissf::CompactDataType* packedMesg = new minissf::CompactDataType;
      // 	  packedMesg->unpack( SIMX_WORLD, fRecvBuf, pos, recv_size );
      // 	  unpackControlMessage( packedMesg );
      // 	  delete packedMesg;
      // 	}
    }


//-------------------------------------------------------------------------

    int testForRecvdMessages(MPI_Status& status)
    {
      SMART_ASSERT( fRecvdMessages.empty() );
      static string where = "simx::Messenger::testForRecvdMessages()";
      //      MPI_Status status;
      int recvd_msg;
      handleMPIReturn( where, MPI_Test( &fRecvReq, &recvd_msg, &status ));
      return recvd_msg;
    }



//-------------------------------------------------------------------------
    
    void checkForRecvdMessages()
    {
      // SMART_ASSERT( fRecvdMessages.empty() );
      static string where = "simx::Messenger::checkForRecvdMessages()"; // 
//       MPI_Status status;
//       int recvd_msg;
//       handleMPIReturn( where, MPI_Test( &fRecvReq, &recvd_msg, &status ));
      
      int recvd_msg;
      MPI_Status status;
      if ( ( recvd_msg = testForRecvdMessages( status ) ) )
	{
	  int recv_size;
	  handleMPIReturn( where, MPI_Get_count( &status, 
						 MPI_PACKED, &recv_size ));
	  SMART_ASSERT( recv_size > 0 )( recv_size );
	  if ( status.MPI_TAG !=  SIMX_CONTROL_TAG )
	    {
	      Logger::error() << where << " Message of Unknown MPI tag received "
			      << endl;
	      return;
	    }
	  
	  Logger::debug2() << where << " Rank " << fMRank 
			   << "Message arrived from " << status.MPI_SOURCE << endl;
	  recvPackedMessage( recv_size );
	  //Initiate another receive
	  initReceive();
	  processReceivedMessages();
	}
      
    

    }

//-------------------------------------------------------------------------

    void packControlMessageForSending( ControlInfoWrapper& cinfo, 
				       minissf::CompactDataType* mesg )
    {
      PackedData pd(mesg);
      cinfo.pack( pd );
    }

//-------------------------------------------------------------------------


 void checkStatus()
    {
      if ( ! MessengerActive() )
	return;
      checkForRecvdMessages();
      sendProgress();
    }

//-------------------------------------------------------------------------

    void sendPackedMessage( LPID srcLP, LPID destLP, 
			    minissf::CompactDataType* mesg )
    {
     
      // TODO: Does not compile.

      //  //TODO: consolidate all the packing functions in one place
      // assert( mesg );
      // static string where = "simx::Messenger::sendPackedMessage()";
      // int pos = 0;
      // boost::shared_array<char> sendBuf(new char[SND_MSG_BUF_SIZE]);
      // handleMPIReturn( where, MPI_Pack( &srcLP, 1, MPI_INT, sendBuf.get(), 
      // 					MSG_BUF_SIZE, &pos, SIMX_WORLD ));
      // handleMPIReturn( where, MPI_Pack( &destLP, 1, MPI_INT, sendBuf.get(),
      // 					MSG_BUF_SIZE, &pos, SIMX_WORLD ));
      // if ( mesg->pack( SIMX_WORLD, sendBuf.get(), pos, MSG_BUF_SIZE ) )
      // 	//TODO: 1 probably isn't the real MPI error code,
      // 	//TODO: but that's what ssf returns. To be fixed.
      // 	handleMPIReturn( where, 1 );
      // MPI_Request request;
      // handleMPIReturn( where, MPI_Isend( sendBuf.get(), pos, MPI_PACKED,
      // 					 destLP, SIMX_CONTROL_TAG,
      // 					 SIMX_WORLD, &request ));
      // pending_request_t pending( request, sendBuf );
      // fPending.push_back( pending );
      
    }

//-------------------------------------------------------------------------


    bool sendMessage( LPID srcLP, LPID destLP, ControlInfoWrapper& cinfo )
    {
      //check on progress
      //checkStatus();

      if ( ! MessengerActive() )
	{
	  Logger::error() << "simx::Messenger: Out-of-band Messaging not "
			  << "initialized; ignoring send request Src LP: "
			  << srcLP << " Dest LP: " << destLP << endl;
	  return false;
	}

      SMART_ASSERT( srcLP == getMessengerRank() )( srcLP )( getMessengerRank() )
	.msg(" simx::Messenger:: LP & Messenger Ranks do not match ");
      
      minissf::CompactDataType* packedMesg = new minissf::CompactDataType;
      if ( ! packedMesg )
	{
	  Logger::error() << "simx::Messenger::sendMessage(): Unable to create"
			  << " CompactDataType " << endl;
	  return false;
	}
      packControlMessageForSending( cinfo, packedMesg );
      sendPackedMessage( srcLP, destLP, packedMesg );
      Logger::debug2() << "simx::Messenger: Control Message initiated from src LP "
		       << srcLP << " to dest LP " << destLP << endl;
      delete packedMesg;
      return true;
      
    }

//-------------------------------------------------------------------------

  
    void initCommunicator()
    {
      Logger::info() << "simx::Messenger: Checking if MPI has been initialized..." << std::endl;
      int mpi_running = 0;
      MPI_Initialized( &mpi_running );
      if ( mpi_running )
	{
	  static string where = "simx::Messenger::initCommunicator()";
	  int mpi_res;
	  Logger::info() << "simx::Messenger: MPI is active. Initializing simx communicator" << std::endl; 

	  int numMPIprocs;
      
	  mpi_res = MPI_Comm_size( MPI_COMM_WORLD, &numMPIprocs );
	  handleMPIReturn( where, mpi_res );
	  //int* newRank = (int*)malloc( numMPIprocs * sizeof(int));
	  int newRank[numMPIprocs];
	  // if ( ! newRank )
// 	    {
// 	      Logger::error() << "simx::Messenger: Malloc failure. "
// 			      << "Unable to initialize Messenger " << endl;
// 	      return;
// 	    }
	  for ( int i = 0; i < numMPIprocs; i++ )
	    newRank[i] = i;
	  handleMPIReturn( where, 
			   MPI_Comm_group( MPI_COMM_WORLD, &orig_group ) );
	  handleMPIReturn( where, 
			   MPI_Group_incl( orig_group, numMPIprocs, newRank, &new_group ) );
	  handleMPIReturn( where,  
			   MPI_Comm_create( MPI_COMM_WORLD, new_group, &SIMX_WORLD ) );
	  handleMPIReturn( where, 
			   MPI_Comm_rank ( SIMX_WORLD, &fMRank) );
	  simx::Logger::info() << "simx Messenger: SIMX_WORLD: Process Rank" 
				  << fMRank << std::endl;
	  fMessengerActive = true;
	  
	  fRecvdMessages.clear();

	  initReceive();
	  
	}
      else
	{
	  Logger::info() << "No MPI found, proceeding with simulation " << std::endl;
	}
      
    }

//-------------------------------------------------------------------------

    void finalize()
    {
      if ( ! MessengerActive() )
	return;
      static string where = "simx::Messenger::finalize() ";
      if ( ! fPending.empty() )
	{
	  Logger::warn() << where << "Rank: " << fMRank 
			 << " There were outstanding ControlInfos to be sent. Deleting " << endl;
	  fPending.clear();
	}
      MPI_Status status;
      if ( testForRecvdMessages( status ) )
	{
	  Logger::warn() << where << "Rank: " << fMRank 
			 << " There were outstanding "
			 << "ControlInfos to be received from Messenger: Rank " 
			 << status.MPI_SOURCE << endl;
	}
      
      
    }
    
//-------------------------------------------------------------------------
    
  } //namespace Messenger
  
} //namespace simx


#endif // SIMX_USE_PRIME
#endif //HAVE_MPI_H
