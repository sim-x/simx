//--------------------------------------------------------------------------
// $Id: util.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    util.h
// Module:  Global
// Author:  Randy Bosetti
// Created: June 02 2004
// Description: 
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef MODULE_GLOBAL_UTIL
#define MODULE_GLOBAL_UTIL

#include <string>

//--------------------------------------------------------------------------

namespace Global {

  //  std::string DecToAbc( unsigned int num );
  std::string DecToAbc( int num );
  std::string& GetBaseName( std::string& name );
  void InstallSignalHandler( const int which[15] );

  int util_main( int argc, char **argv );

}

//--------------------------------------------------------------------------
#endif // MODULE_GLOBAL_UTIL
//--------------------------------------------------------------------------
