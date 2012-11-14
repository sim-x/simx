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

//-------------------------------------------------------------------------
// $Id: Values.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//-------------------------------------------------------------------------
//
// File:    Values.C
// Module:  Common
// Author:  gam
// Created: Fri May 26 08:24:55 2006
// Description:
//
// @@
//
//-------------------------------------------------------------------------

#include "simx/Common/Values.h"

//-------------------------------------------------------------------------

namespace Common {

  std::string Values::fProgName( "" );
  int Values::fRank( -1 );
  std::string Values::fRankSuffix( "" );
  unsigned int Values::fRankExtLength( 2 );
  
  const std::string& 
  Values::gProgName()
  { return fProgName; }
  
  int
  Values::gRank()
  { return fRank; }

  const std::string&
  Values::gRankSuffix()
  { return fRankSuffix; }

  unsigned int
  Values::gRankExtLength()
  { return fRankExtLength; }

  void
  Values::SetProgName( const std::string& name )
  { fProgName = name; }

  void
  Values::SetRank( int rank )
  { fRank = rank; }

  void
  Values::SetRankSuffix( const std::string& suffix )
  { fRankSuffix = suffix; }

  void
  Values::SetRankExtLength( unsigned int len )
  { fRankExtLength = len; }
  
} // namespace

//-------------------------------------------------------------------------
// End of Values.C
//-------------------------------------------------------------------------
