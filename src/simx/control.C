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
// $Id: control.C,v 1.23 2011/01/07 20:04:41 sunil Exp $
//--------------------------------------------------------------------------
// File:    control.C
// Module:  simx
// Author:  Lukas Kroc
// Created: March 29 2005
//
//	Control functions for starting simulation
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/control.h"

#include "simx/config.h"
#include "simx/writers.h"
#include "simx/constants.h"
#include "simx/logger.h"
#include "simx/output.h"

#include "simx/LP.h"
#include "simx/EntityManager.h"
#include "simx/EntityData.h"
#include "simx/ServiceManager.h"
#include "simx/InfoManager.h"
#include "simx/PackedData.h"

#ifndef __APPLE__
#include "Common/ProcessStats.h"
#endif
#include "Common/Values.h"
#include "Random/Random.h"


#ifdef HAVE_MPI_H
#include "mpi.h"
#endif
#include <unistd.h>

#ifdef SIMX_USE_PRIME
    #include "ssf.h"
    #include "simx/Messenger.h"
#endif

#include "simEngine.h"



//--------------------------------------------------------------------------

using namespace Config;
using namespace std;

//--------------------------------------------------------------------------


// out-of-band signalling hook; will be invoked 
// from within PrimeSSF during each epoch ( see ssf/src/sim/teleport.cc ) 

// LK: TODO: put this ^ into SimEngine
#ifdef SIMX_USE_PRIME
#ifdef HAVE_MPI_H
// void simx_messaging()
// {
//   simx::Messenger::checkStatus();

// }
#endif
namespace simx {

  PyThreadState* py_main_thread_state = NULL;
  
  void simx_restore_py_main_thread_state()
  {
    //assert(false);
    //PyEval_RestoreThread( py_main_thread_state );
  }
}

#endif

//--------------------------------------------------------------------------


/// "private" control stuff
namespace {
  
  using namespace simx;
  using namespace simx::Control;
  
  
  /// map of LPID to LP* of LPs on this computer node
  LpPtrMap	fLpPtrMap;
  
  /// "global" variables
  int 		fNumMachs = 0;
  int 		fNumLPs = 0;
  int		fRank = 0;
  string	fProcessorName("");

  
  eSimPhase fPhase = kPhaseInit;

  //#ifdef SIMX_USE_PRIME
  


  //#endif

  
  /// initilizes global variables
  void initGlobals(const std::string& modulename)
  {
    //========================================================================
    // do global initialization

#ifdef SIMX_USE_PRIME
    fNumMachs = minissf::ssf_num_machines();	
    fRank = minissf::ssf_machine_index();
#else
    SimEngine::init();
    fNumMachs = SimEngine::getNumMachs();	
    fRank = SimEngine::getRank();
#endif

#ifdef HAVE_MPI_H
    int proc_name_len = MPI_MAX_PROCESSOR_NAME + 1; // for null termination
    char proc_name[proc_name_len]; 
    int resultlen;
    int rc = MPI_Get_processor_name(proc_name, &resultlen);
    proc_name[resultlen] = '\0';
    if(rc != MPI_SUCCESS) {
      char errStr[MPI_MAX_ERROR_STRING + 1]; // for null termination
      MPI_Error_string(rc, errStr, &resultlen);
      errStr[resultlen] = '\0';      
      Logger::failure(errStr);
    }
#else
    char proc_name[256];
    size_t len;
    gethostname( proc_name, len );
#endif

    fProcessorName = proc_name;
    Logger::info() 
      << "Control: Starting "
      << fRank << "/"
      << fNumMachs << " on " << proc_name << " with pid " << getpid()  
      << endl;
    
    gConfig.GetConfigurationValueRequired(ky_NUMBER_LPS, fNumLPs);
    if (fNumLPs == 0) 
    {
      fNumLPs = getNumMachines();
      Logger::info() 
	<< "Control: NumLPs set to default equal to numMachines " 
	<< getNumMachines () 
	<< endl;
    } 

    Random::TRandom::SetNumberStreams(fNumLPs);
      
    int rSeed;
    gConfig.GetConfigurationValue(ky_SEED, rSeed, 0);
    if( rSeed != 0 )
    {
      /// when not zero (means determine from time etc..), make it
      /// different for each machine
      rSeed += fRank;
    }
    Logger::info() 
      << "Control: setting random seed to " 
      << rSeed 
      << endl;
    Random::TRandom::SetSeed( rSeed );
    
    fPhase = kPhaseInit;
    
  }
  
  /// create LPs
  void createLPs()
  {
    //========================================================================
    // create LPs which will be served with this unix process
    SMART_VERIFY( fLpPtrMap.empty() )( fLpPtrMap )
      .msg("Control::createLPs() can only be called once");
    SMART_VERIFY( getNumLPs() > 0 )( getNumLPs() )
      .msg("Control::createLPs() can only be called after Control::initGlobals");

    for (int i=0; i < getNumLPs(); ++i)
    {
      //create the lp's
      // NOTE: the module is important! In particular, we need that LP(i) is on machine_rank(i)
      if (i % getNumMachines() == getRank())
      {
	LP* nlp = new LP(i);
	fLpPtrMap[i] = nlp;
      }
    }
        
    if( fLpPtrMap.empty() )
    {
      // it may happen that we have no LPs assigned, but it is strange
      Logger::warn()
	<< "Control: no LPs assigned to getRank() "
	<< getRank() << endl;
    }
  }

  /// reads in Info files and schedules events read in
  void readInfoFiles()
  {	
    //========================================================================
    // schedule external events using InfoManager
    string infoFilenames;
    //gConfig.GetConfigurationValueRequired(ky_INFO_FILES, infoFilenames);
    if ( gConfig.GetConfigurationValue(ky_INFO_FILES, infoFilenames) )
      theInfoManager().prepareDataFiles( infoFilenames );
  }


} // namespace

//==============================================================================
/// "public" control stuff

namespace simx {

  /// used to register object we work with, defined in register.C
  void registerAll();

  namespace Control {

    /// to initialize the program, must be called first before other
    /// methods in here 
    /// \param Name of the module (e.g. PacketSim, SessionSim ...)
    void init(const std::string& modulename)
    {

      Logger::init(modulename);	///< register the logging module name
     
#ifndef NDEBUG
      //TODO: remove __APPLE__ macro here and everywhere else in this file
#ifndef __APPLE__
      Logger::info() << "MEM_BEGIN: " 
      << Common::ProcessStats::GetMemInfo() << endl;
#endif
#endif     
        
      // read global settings
      initGlobals(modulename);
    
      // now create LPs
      createLPs();
    
      // register objects from simx
      registerAll();
    
      // some logging:
      Logger::info() 
	<< "Control: typename(Time): '"
	<< typeid(Time).name() 
	<< "', sizeof(Time): "
	<< sizeof(Time)
	<< ", MINDELAY: " 
	<< simx::LP::MINDELAY << endl;

#ifdef SIMX_USE_PRIME
      // TODO: Out-of-band messaging does not work with minissf.
      // bool ob_mesging;
      // if ( gConfig.GetConfigurationValue(ky_OUT_OF_BAND_MESSAGING,ob_mesging )
      // 	   && ob_mesging )
      // 	{
      // 	  if ( getNumLPs() < 2 )
      // 	    {
      // 	      Logger::error() << " Control: init(): Out-of-band Messaging "
      // 			      << " cannot be activated in serial simulations " << endl;
      // 	    }
      // 	  else
      // 	    {
      // 	      Messenger::initCommunicator();
      // 	      MPI_Barrier(MPI_COMM_WORLD);
      // 	    }
      // 	}
#endif


	///===================================
	/// Init simulation (so that entities/services can schedule events at creation time)	
#ifdef HAVE_MPI_H
      MPI_Barrier(MPI_COMM_WORLD);
#endif
      Time endTime;
      gConfig.GetConfigurationValueRequired(ky_END_TIME, endTime);
      Logger::info() 
	<< "Control: Starting simulation (" 
	<< 0 << "," 
	<< endTime << ")" << endl;

      // go to negative beginning to allow infos for time 0 to be delivered on time
      // prime ssf does not allow negative time!
#ifdef SIMX_USE_PRIME
      //minissf::Entity::startAll(Time(0), endTime);
      //minissf::ssf_start(endTime);
#else
	SimEngine::prepare(0, endTime);
#endif

      fPhase = kPhaseRun;

    }
    
    /// prepares Output functionality
    void prepareOutput()
    {
      //========================================================================
      // prepare Output services
      if(gConfig.IsBound(ky_OUTPUT_FILE)) {
         string outputFile;
         gConfig.GetConfigurationValueRequired(ky_OUTPUT_FILE, outputFile);
   
         Logger::info()
   	<< "Control: preparing output to file: "
   	<< outputFile+Common::Values::gRankSuffix() << endl;
         Output::init( outputFile+Common::Values::gRankSuffix() );
      }
      else {
         Logger::info() << "Control: no output" << endl;
      }
    }
	
    /// prepares Services (might need initited Output)
    void prepareServices()
    {
      //========================================================================
      // prepare services using ServiceManager
      string serviceFilenames;
      gConfig.GetConfigurationValueRequired(ky_SERVICE_FILES, serviceFilenames);

      Logger::info() 
	<< "Control: preparing services from file(s): " 
	<< serviceFilenames << endl;
      theServiceManager().prepareServices( serviceFilenames );
    }

    /// creates Entities (needs prepared Services)
    void createEntities()
    {
      //========================================================================
      // create Entities using EntityManager
      string entityFilenames;
      gConfig.GetConfigurationValueRequired(ky_ENTITY_FILES, entityFilenames);

      Logger::info() 
	<< "Control: creating entities from file(s): " 
	<< entityFilenames << endl;
      theEntityManager().createEntities( entityFilenames );
    }

    // TODO: LK: should be abandoned!	
    // create an entity (needs prepared Services)
//    void createEntity(EntityID eid, string etype, int eprof)
//    {
//      //========================================================================
//      // create an Entity using the EntityManager
//      EntityData newEntity(eid, etype, eprof);
//      Logger::info()
//        << "Control: creating entity "
//        << eid << " " << etype << " " << eprof << endl;
//      theEntityManager().createEntity( eid, etype newEntity);
//    }


    /// starts the simulation (reads in Info files, needs created Entities)
    void startSimulation()
    {
      //int debug_wait = 1;
      //while(debug_wait);
#ifndef NDEBUG
#ifndef __APPLE__
      Logger::info() << "MEM_MIDDLE: " 
	   << Common::ProcessStats::GetMemInfo() << endl;
#endif
#endif

      //int debug_wait = 1;
      //while (debug_wait);

      //========================================================================
      // run the simulation 

      // now read in Info files and schedule external events
      readInfoFiles();

      // this doesn't return until the simulation is finished
#ifdef SIMX_USE_PRIME
      //minissf::Entity::joinAll();
      Time endTime;
      gConfig.GetConfigurationValueRequired(ky_END_TIME, endTime);
      
      //if (minissf::ssf_num_machines() > 1)
      //py_main_thread_state = PyEval_SaveThread();
      
      minissf::ssf_start(endTime);
#else
	SimEngine::run();
#endif

      fPhase = kPhaseWrapUp;

#ifdef SIMX_USE_PRIME
      //settle out-of-channel Messenger
      // TODO: out-of-band messaging does not work with minissf
      // Messenger::finalize();
#endif 
     
      Logger::info() << "Control: Simulation finished" << endl;

#ifndef NDEBUG
#ifndef __APPLE__
      Logger::info()
	<< "MEM_END: "
	<< Common::ProcessStats::GetMemInfo() 
	<< endl;
#endif
#endif

#ifdef SIMX_USE_PRIME
      minissf::ssf_finalize();
#else
	SimEngine::finalize();
#endif

    }

    int getNumMachines()
    {
      return fNumMachs;
    }

    int getRank()
    {
      return fRank;
    }

    int getNumLPs()
    {
      return fNumLPs;
    }

    const string& getProcessorName()
    {
	return fProcessorName;
    }
    
    const LpPtrMap& getLpPtrMap()
    {
	return fLpPtrMap;
    }
    
    eSimPhase getSimPhase() {
        return fPhase;
    }
  } //Control namespace
  
  
} //simx namespace


