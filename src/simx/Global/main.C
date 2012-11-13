//--------------------------------------------------------------------------
// $Id: main.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    main.C
// Module:  Global
// Author:  Keith Bisset
// Created: July 10 2002
// Description: This is a main() function that does some standard
// initialization, and then calls ModuleMain().  It also catches any
// exceptions that make it up this far.  
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Global/main.h"
#include "simx/Global/OptionManager.h"
#include "simx/Global/util.h"
#include "simx/Common/Exception.h"
#include "simx/Common/Values.h"
#include "simx/Config/Configuration.h"
#include "simx/Log/Logger.h"

#include <stdlib.h>
#include <exception>
#include <iostream>
#include <cerrno>

//--------------------------------------------------------------------------

int main(int argc, char** argv)
{
  using namespace Global;
  using namespace Common;
  using namespace std;

  const int mySignals[15] = { 1,2,3,4,5,6,7,8,9,0,11,0,13,14,15 };

  InstallSignalHandler(mySignals);
  util_main( argc, argv );

  if (gOptMan.GetNumOptionsAfterDDash() < 1 &&
      gOptMan.GetNumOptionsBeforeDDash() < 1)
  {
    cout << "usage: "
	 << Values::gProgName()
	 << " <config file> [<rank>] [<suffixlen>]\n"; 
    cerr << Values::gProgName()
	 <<" execution failed\n";
    return EXIT_FAILURE;
  }

  try {
    errno = 0;
    if (gOptMan.GetNumOptionsAfterDDash() < 1)
      Config::ConfigInit(gOptMan[0][0].c_str());
    else
      Config::ConfigInit(gOptMan.FreeOpt(0)[0].c_str());
  }
  catch( const exception& exception )
  {
    cout << exception.what()
	 << endl;
    cerr << Values::gProgName()
	 <<" execution failed\n";
    return EXIT_FAILURE;
  }

  Log::sInitLog( Values::gRank(), Values::gRankSuffix() );

  try {
    errno=0;
    ModuleMain();
    Log::log().Success( 0 );
  }
 catch(const exception& exception)
 {
   cout << "Exception caught:\n"
	<< exception.what() << endl;
 }
 catch(...)
 {
   cout << "Unknown exception caught"
	<< endl;
 }

 cerr << Values::gProgName()
      << " execution failed\n";

 return EXIT_FAILURE;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
