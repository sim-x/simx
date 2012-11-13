//--------------------------------------------------------------------------
// $Id: main_MPI.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
//
// File:        main_MPI.C 
// Module:      Global 
// Author:      Keith Bisset, Lukas Kroc
// Created:     July 10 2002 
// Time-stamp:  "2002-10-09 13:40:50 kbisset"
// Description:
// This is a main() function that calls MPI_Init, does some standard
// initialization, and then calls ModuleMain().  It also catches any
// exceptions that make it up this far.
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Global/main.h"
#include "simx/Global/AbortHandlerMPI.h"
#include "simx/Global/OptionManager.h"
#include "simx/Global/util.h"
#include "simx/Common/Exception.h"
#include "simx/Common/Values.h"
#include "simx/Config/Configuration.h"
#include "simx/Log/Logger.h"
#include "mpi.h"

#include <iostream>
#include <fstream>
#include <cerrno>

using namespace Log;
using namespace std;

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

int die( const string& emsg );

int main(int argc, char** argv)
{
  // For speed, don't try to synchronize with stdio functions (printf,
  // scanf, etc).
  ios_base::sync_with_stdio(false);

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
    mpiReturn = MPI_Init_thread( &argc, &argv, MPI_THREAD_MULTIPLE, &mpi_thread_provided );
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

  string name( argv[0] );
  Common::Values::SetProgName( Global::GetBaseName( name ) );

  cerr << Common::Values::gProgName() << ": MPI_Init OK" << endl;

  const string failMsg = Common::Values::gProgName() + 
			 " execution failed: ";
  int rank = -1;
  mpiReturn = MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  Common::Values::SetRank( rank );
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

  // Let only one node print error messages.
  const bool isPrintNode( (myRank == 0) ? true : false );

  Global::util_main( argc, argv );
  if (Global::gOptMan.GetNumOptionsAfterDDash() < 1 &&
      Global::gOptMan.GetNumOptionsBeforeDDash() < 1)
  {
    const string emsg = 
      "usage: " +
      Common::Values::gProgName() +
      " <config file> [<rank>] [<suffixlen>]";

    if (isPrintNode)
    { cerr << emsg << endl; }
    log().Failure( 0, emsg );
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // --------- Set up framework

  // Set the suffix based on the rank.
  string sfx( "." );
  sfx +=  'A' + myRank / 26;
  sfx +=  'A' + myRank % 26;
  Common::Values::SetRankSuffix( sfx );

  // init configuration file.
  try 
  {
    Config::ConfigInit( argv[1] );
  }
  catch( const exception& exception )
  {
    string emsg = failMsg + " std::exception in main_MPI (ConfigInit): ";
    emsg += exception.what();
    if (isPrintNode)
    { cerr << emsg << endl; }
    log().Failure( 0, emsg );
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // init logging
  Log::sInitLog( myRank, Common::Values::gRankSuffix() );
  // AbortHandlerMPI causes bogus error messages if run fails.
  //  log().addAbortHandler( new Global::AbortHandlerMPI() );

  // --------- Run application

  try 
  {
    errno = 0;
    Global::ModuleMain();
  }

  // --------- oops

  catch( const Common::Exception& exception )
  {
    string emsg = failMsg + " Common::Exception";
    emsg += exception.what();
    return die( emsg );
  }
  catch( const exception& exception )
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

  // --------- Successful completion

 cerr << Common::Values::gProgName() << ": calling MPI_Finalize" << endl;
 MPI_Finalize();
 //cerr << Common::Values::gProgName() << ": finalized" << endl;
 //log().Success( 0 );

 return EXIT_SUCCESS;
}

//--------------------------------------------------------------------------

int die( const string& emsg )
{
  const string fullmsg = emsg + " occurred in main_MPI (ModuleMain)";

  cerr << fullmsg << endl;
  log().Failure( 0, fullmsg );

  MPI_Abort(MPI_COMM_WORLD, 1);

  return EXIT_FAILURE;
}

//--------------------------------------------------------------------------
// end
//--------------------------------------------------------------------------
