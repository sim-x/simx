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
// $Id: util.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    util.h
// Module:  Global
// Author:  Randy Bosetti
// Created: June 02 2004
// Description: 
//
// @@
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
