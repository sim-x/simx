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
// @@COPYRIGHT@@
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
