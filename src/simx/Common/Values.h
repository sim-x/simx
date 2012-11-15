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

//-*-C++-*-----------------------------------------------------------------
// $Id: Values.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//-------------------------------------------------------------------------
//
// File:    Values.h
// Module:  Common
// Author:  gam
// Created: Thu May 25 16:15:06 2006
// Description:
//
// @@
//
//-------------------------------------------------------------------------

#ifndef NISAC_VALUES_COMMON
#define NISAC_VALUES_COMMON

#include <string>

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

namespace Common {

  class Values
  {
    
  public:

    static const std::string& gProgName();
    static int gRank();
    static const std::string& gRankSuffix();
    static unsigned int gRankExtLength();

    static void SetProgName( const std::string& name );
    static void SetRank( int rank );
    static void SetRankSuffix( const std::string& suffix );
    static void SetRankExtLength( unsigned int len );

  private:

    // fProgName == argv[0], set in Common::main family
    static std::string fProgName;
    static int fRank;
    static std::string fRankSuffix;
    static unsigned int fRankExtLength;

  };

} // namespace

#endif // NISAC_VALUES_COMMON

//-------------------------------------------------------------------------
// End of Values.h
//-------------------------------------------------------------------------
