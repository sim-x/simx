//--------------------------------------------------------------------------
// $Id: main_dassf.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
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
#include "simx/Global/AbortHandlerMPI.h"
#include "simx/Common/Exception.h"
#include "simx/Common/Values.h"
#include "simx/Common/Versions.h"
#include "simx/Config/Configuration.h"
#include "simx/Log/Logger.h"

#include "ssf.h"

#include <stdlib.h>
#include <exception>
#include <iostream>
#include <cerrno>

//--------------------------------------------------------------------------

//int _ssf_main_tmp0 = 0;
//extern int ssf_main(int argc, char** argv);
//
//int _ssf_register_main(int (*)(int, char**));
//
//int _ssf_main_tmp1 = _ssf_register_main(ssf_main);

//--------------------------------------------------------------------------

int main(int argc, char** argv)
{
  if (argc == 2 && strcmp(argv[1], "--version") == 0)
  {
    using namespace Common;
    write_versions();
    return EXIT_SUCCESS;
  }
  
  // Don't try to synchronize with stdio functions (printf, scanf,
  // etc)
  std::ios_base::sync_with_stdio(false);

  // Configuration file must be an argument.
  if (argc < 2) {
    std::cout 
      << "Usage: <program> <config file> [<rank>]" 
      << std::endl;
    exit(EXIT_FAILURE);
  }
  // Process and store rank number, if supplied.
  if (argc > 2) {
    Common::Values::SetRank( atoi(argv[2]) );
  } else {
    Common::Values::SetRank( prime::ssf::ssf_machine_index() );
  }      

  // Set the suffix based on the rank, if supplied.
  if (Common::Values::gRank() != -1) {
    std::string sfx( "." );
    sfx += 'A'+ Common::Values::gRank() / 26;
    sfx += 'A'+ Common::Values::gRank() % 26;
    Common::Values::SetRankSuffix( sfx );
  }

  // init configuration file.
  Config::ConfigInit(argv[1]);

  // init logging
  Log::sInitLog(Common::Values::gRank(), Common::Values::gRankSuffix());
  Log::log().addAbortHandler(new Global::AbortHandlerMPI());
  
  try
  {
    errno=0;
    Global::ModuleMain();
    Log::log().info(0) << "SUCCESS exit" << std::endl;
    return EXIT_SUCCESS;
  }

 catch(const std::exception& exception)
 {
   std::cout << "Exception caught:\n" << exception.what() << std::endl;
 }
 catch(...)
 {
   std::cout << "Unknown exception caught:\n" << std::endl;
 }
 std::cerr << "Execution Failed" << std::endl;
 return EXIT_FAILURE;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


