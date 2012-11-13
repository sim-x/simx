//--------------------------------------------------------------------------
// $Id: main_arg.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    main.C
// Module:  Global
// Author:  Keith Bisset
// Created: July 10 2002
// Time-stamp: "2006-06-07 15:08:57 gam"
// Description: This is a main() function that does some standard
// initialization, and then calls ModuleMain().  It also catches any
// exceptions that make it up this far.
//
// Additionally, it passes its args to ModuleMain, and doesn't read a
// config file.  It shouldn't be used by normal programs, only by
// utility programs.
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Global/main.h"
#include "simx/Common/Exception.h"
#include "simx/Global/util.h"
#include "simx/Common/Values.h"
#include "simx/Config/Configuration.h"
#include "simx/Log/Logger.h"
//LK: why? #include "config/config.h"

#include <stdlib.h>
#include <exception>
#include <iostream>
#include <cerrno>

//--------------------------------------------------------------------------

int main(int argc, char** argv)
{

  using namespace Global;
  using namespace Common;

  const int mySignals[15] = { 1,2,3,4,5,6,7,8,9,0,11,0,13,14,15 };

  InstallSignalHandler(mySignals);
  //  util_main(argc,argv);

  Values::SetProgName( argv[0] );

  Config::ConfigInit("/dev/null");
  Log::sInitLog( Values::gRank(), Values::gRankSuffix() );

  try {
    errno=0;
    ModuleMain(argc,argv);
    Log::log().Success( 0 );
  }

  catch(const std::exception& exception)
  {
    std::cout << "Exception caught:\n" 
	      << exception.what() << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception caught"
	      << std::endl;
  }

 std::cerr << Values::gProgName()
	   << " execution failed" << std::endl;

  return EXIT_FAILURE;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
