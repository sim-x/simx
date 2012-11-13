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
// @@COPYRIGHT@@
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
