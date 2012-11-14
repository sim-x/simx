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
// $Id: type.h,v 1.20 2010/12/15 17:12:22 ctallman Exp $
//--------------------------------------------------------------------------
// File:    type.h
// Module:  simx
// Author:  K. Bisset
// Created: September  7 2004
//
// Description: simx types.
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_TYPE
#define NISAC_SIMX_TYPE

// whether to use PRIME (if not, uses simEngine)
//#define SIMX_USE_PRIME
// whether to expect linking with SST (exclusive with USE_PRIME)
//#define SIMX_SST 1

#include "boost/tuple/tuple_comparison.hpp"
#include "boost/tuple/tuple_io.hpp"
#include "boost/smart_ptr.hpp"
#include <boost/python.hpp>

#include "Common/Assert.h"

#ifdef SIMX_USE_PRIME
    #include "ssf.h"
#endif

#include <limits.h>

//--------------------------------------------------------------------------

/// \file type.h
/// \brief Type definitions used in the simulator core.

namespace simx {

  /// Definition of time, as in DaSSF. This typedef defines type of
  /// time throughout the program.
#ifdef SIMX_USE_PRIME
    typedef prime::ssf::ltime_t Time;  
#elif SIMX_SST
    typedef uint64_t Time;	//< make sure this is the same as in SST
#else
    typedef long Time;
    #define PRIME_SSF_LTIME_LONG
#endif

  // SSF supports 64-bit long long's only
  // On a 32-bit machine, that is a long long
  // On a 64-bit machine, that is a long
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
  typedef long long simxLong;
#else
  typedef long simxLong;
#endif
  typedef boost::tuple<char,simxLong> EntityID;
  typedef int LPID;
  typedef int ProfileID;

  // for python; profiles are python dictionaries
  typedef boost::python::dict PyProfile;

  typedef std::string ServiceName;

  enum ServiceAddress 
    {
      kServiceAddressMIN = INT_MIN, 
      kServiceAddressMAX = INT_MAX 
    };

  ///< size of info objects
  typedef unsigned int	Size;
  ///< type used to identify a line of output
  typedef int OutputRecordType;

/// used to emulate bahavior of auto_ptr in shared_ptr (transfer of ownership)
template<typename T>
boost::shared_ptr<T> giveup_smart_ptr(boost::shared_ptr<T>& in)
{
    boost::shared_ptr<T> tmp(in);
    in.reset();
    return tmp;
}


} // simx namespace

//--------------------------------------------------------------------------
#endif // NISAC_SIMX_TYPE
//--------------------------------------------------------------------------
