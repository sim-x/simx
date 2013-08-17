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
//
// File:        init.C 
// Module:      Global 
// Author:      Keith Bisset, Lukas Kroc, Sunil Thulasidasan
// Created:     July 10 2002 
// Time-stamp:  "2002-10-09 13:40:50 kbisset"
// Description:
// This is a main() function that calls MPI_Init, does some standard
// initialization, and then calls ModuleMain().  It also catches any
// exceptions that make it up this far.
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/Global/main.h"
#ifdef HAVE_MPI_H
#include "mpi.h"
#include "simx/Global/AbortHandlerMPI.h"
#endif
#include "simx/Global/OptionManager.h"
#include "simx/Global/util.h"
#include "simx/Common/Exception.h"
#include "simx/Common/Values.h"
#include "simx/Config/Configuration.h"
#include "simx/Log/Logger.h"


#include <iostream>
#include <fstream>
#include <cerrno>

#include <boost/python.hpp>

#include "simx/LP.h"


using namespace Log;
using namespace std;
using namespace boost;

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//extern simx::Time simx::LP::MINDELAY;
//extern void Global::ModuleMain();
namespace{



int die( const string& emsg );

//int main(int argc, char** argv)
 int init_mpi(const python::object& name_str)//, const python::object& config_str )
{

  string name("()");
  try {
    //name = python::extract<string>(name_str.attr("__str__"));
    name = python::extract<string>(name_str);
    //cout << "Name is " << name << endl;
  }
  catch(...) {
    PyErr_Print();
    PyErr_Clear();
    return -1;
  }
  


  // For speed, don't try to synchronize with stdio functions (printf,
  // scanf, etc).
  ios_base::sync_with_stdio(false);
#ifdef SIMX_USE_PRIME

  int i = 0;
  minissf::ssf_init(i, NULL);
  //set rank of this process
  Common::Values::SetRank( minissf::ssf_machine_index() );
  //Common::Values::SetRank( 0 );
#ifdef HAVE_MPI_H
  return MPI_SUCCESS;
#else
  return 0;
#endif

#else    // we are using simengine

  const int mySignals[15] = { 1,2,3,4,5,6,7,8,9,0,0,0,13,14,15 };
  Global::InstallSignalHandler(mySignals);

  // --------- Start MPI

  // mpirun mangles the command line.  MPI_Init demangles it, so
  // start MPI before doing anything with command-line args.

  int mpiReturn = 0;

  try 
  { 
//    mpiReturn = MPI_Init( &argc, &argv );
//LK: Start up MPI with threads
    int mpi_thread_provided;
    //mpiReturn = MPI_Init_thread( &argc, &argv, MPI_THREAD_MULTIPLE, &mpi_thread_provided );
    mpiReturn = MPI_Init_thread( NULL, NULL, MPI_THREAD_MULTIPLE, &mpi_thread_provided );
    if( mpi_thread_provided < MPI_THREAD_MULTIPLE )
    {
	cerr << "MPI cannot provide desired thread support" << endl;
	string err;
	switch( mpi_thread_provided )
	{
	case MPI_THREAD_SINGLE:
	    err = "MPI_THREAD_SINGLE";
	    break;
	case MPI_THREAD_FUNNELED:
	    err = "MPI_THREAD_FUNNELED";
	    break;
	case MPI_THREAD_SERIALIZED:
	    err = "MPI_THREAD_SERIALIZED";
	    break;
	case MPI_THREAD_MULTIPLE:
	    err = "MPI_THREAD_MULTIPLE";
	    break;
	default:
	    err = "(unknown value of mpi_thread_provided)";
	}
	cerr << "Thread support asked for: MPI_THREAD_MULTIPLE, obtained: " << err << endl << endl;
	throw;
    }
  }
  catch( ... )
  {
    const string xx( "Exception thrown by MPI_Init" ); 
    cerr << xx << endl;
    log().Failure( 0, xx );
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  if (mpiReturn != MPI_SUCCESS)
  {
    const string xx( "Error returned by MPI_Init" );
    cerr << xx << endl;
    log().Failure( 0, xx );
    MPI_Abort(MPI_COMM_WORLD, 1);
  }    


  
  Common::Values::SetProgName( Global::GetBaseName( name ) );

  cerr << Common::Values::gProgName() << ": MPI_Init OK" << endl;
  
  const string failMsg = Common::Values::gProgName() + 
			 " execution failed: ";
  int rank = -1;
  mpiReturn = MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  Common::Values::SetRank( rank );
  cout << "MPI rank is " << rank << endl;
  const int myRank( Common::Values::gRank() );
  int mpiSize;
  const int mpiReturn2( MPI_Comm_size( MPI_COMM_WORLD, &mpiSize ) );

  // MPI ok?
  if (mpiReturn != MPI_SUCCESS ||
      mpiReturn2 != MPI_SUCCESS ||
      myRank < 0 ||
      mpiSize < 1) 
  {
    string emsg = failMsg + "MPI not running properly.\n";
    emsg +="rank should be > 0; actually is ";
    emsg += myRank;
    emsg += "\nsize should be > 1; actually is ";
    emsg += mpiSize;
    cerr << emsg << endl;
    log().Failure( 0, emsg );
  }
  return MPI_SUCCESS;
#endif

}
 
 ///////////////////////////////////////////////////////////////////////


 //int init_config(const python::object& config_str)//, cons
 int init_config() 
{
  // TODO (remove code repetition below for failmsg
  const int myRank( Common::Values::gRank() );
  const string failMsg = Common::Values::gProgName() + 
    " execution failed: ";
  
  // Let only one node print error messages.
  const bool isPrintNode( (myRank == 0) ? true : false );

  // Global::util_main( argc, argv );
//   if (Global::gOptMan.GetNumOptionsAfterDDash() < 1 &&
//       Global::gOptMan.GetNumOptionsBeforeDDash() < 1)
//   {
//     const string emsg = 
//       "usage: " +
//       Common::Values::gProgName() +
//       " <config file> [<rank>] [<suffixlen>]";

//     if (isPrintNode)
//     { cerr << emsg << endl; }
//     log().Failure( 0, emsg );
//     MPI_Abort(MPI_COMM_WORLD, 1);
//   }

  // --------- Set up framework

  // Set the suffix based on the rank.
  string sfx( "." );
  sfx +=  'A' + myRank / 26;
  sfx +=  'A' + myRank % 26;
  Common::Values::SetRankSuffix( sfx );

  // string config_name("()");
  // try {
  //   config_name = python::extract<string>(config_str);
  //   cout << "Config file is " << config_name << endl;
  // }
  // catch(...) {
  //   PyErr_Print();
  //   PyErr_Clear();
  //   return -1;
  // }
  
  //init configuration file.
  try 
    {
      //Config::ConfigInit( config_name );
      Config::ConfigInit();
    }
  catch( const std::exception& exception )
    {
      string emsg = failMsg + " std::exception in init_config(): ";
      emsg += exception.what();
      if (isPrintNode)
	{ cerr << emsg << endl; }
      log().Failure( 0, emsg );
#ifdef HAVE_MPI_H
      MPI_Abort(MPI_COMM_WORLD, 1);
#endif
    }

  //TODO: use named constants below.
  return 0;
}

//////////////////////////////////////////////////////////////////////

int init_env() {

   // TODO (remove code repetition below for failmsg
  const int myRank( Common::Values::gRank() );
  const string failMsg = Common::Values::gProgName() + 
    " execution failed: ";

  // init logging
  Log::sInitLog( myRank, Common::Values::gRankSuffix() );
  // AbortHandlerMPI causes bogus error messages if run fails.
  //  log().addAbortHandler( new Global::AbortHandlerMPI() );

  // --------- Run application

  //  return -1;

  try 
  {
    errno = 0;
    //TODO, critical, python: get rid of two modulemain functions
    
      Global::initEnv();
  }

  // --------- oops

  catch( const Common::Exception& exception )
  {
    string emsg = failMsg + " Common::Exception";
    emsg += exception.what();
    return die( emsg );
  }
  catch( const std::exception& exception )
  {
    string emsg = failMsg + " std::exception";
    emsg += exception.what();
    return die( emsg );
  }
 catch( ... )
 {

   string emsg = failMsg + " unknown exception";
   return die( emsg );
 }
  return 0;
}



int run_simulation() {

   // TODO (remove code repetition below for failmsg
  //  const int myRank( Common::Values::gRank() );
  const string failMsg = Common::Values::gProgName() + 
    " execution failed: ";


  try 
  {
    errno = 0;
    //TODO, critical, python: get rid of two modulemain functions
    Global::startSimulation();
#ifdef SIMX_USE_PRIME
    Log::log().info(0) << "SUCCESS exit" << endl;
    return EXIT_SUCCESS;
#endif
  }

  // --------- oops
  catch( const Common::Exception& exception )
  {
    string emsg = failMsg + " Common::Exception";
    emsg += exception.what();
    return die( emsg );
  }
  catch( const std::exception& exception )
  {
    string emsg = failMsg + " std::exception";
    emsg += exception.what();
    return die( emsg );
  }
 catch( ... )
 {

   string emsg = failMsg + " unknown exception";
   return die( emsg );
 }

#ifdef SIMX_USE_PRIME

#else  // simengine   
  // --------- Successful completion

 cerr << Common::Values::gProgName() << ": calling MPI_Finalize" << endl;
 MPI_Finalize();
 //cerr << Common::Values::gProgName() << ": finalized" << endl;
 //log().Success( 0 );

 return EXIT_SUCCESS;

#endif
}

//--------------------------------------------------------------------------

int die( const string& emsg )
{
  const string fullmsg = emsg + " occurred while running simulation";

  cerr << fullmsg << endl;
  log().Failure( 0, fullmsg );

#ifndef SIMX_USE_PRIME
  MPI_Abort(MPI_COMM_WORLD, 1);
#endif
  return EXIT_FAILURE;
}

//BOOST_PYTHON_MODULE(HelloWorld)

}
void export_init() {
  python::def("init_mpi",init_mpi);
  python::def("init_config",init_config);
  //python::def("init_main",init_main);
  python::def("init_env",init_env);
  python::def("run",run_simulation);
}
//--------------------------------------------------------------------------
// end
//--------------------------------------------------------------------------
