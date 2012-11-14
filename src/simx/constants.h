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
// File:    constants.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Apr 13 2005
//
// Description: simx special constants (e.g. service names and addresses)
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_CONSTANTS
#define NISAC_SIMX_CONSTANTS

#include "simx/type.h"

namespace simx {
  
/* #if defined(PRIME_SSF_LTIME_FLOAT) */
/*     const Time MINDELAY = .000001;	///< minimum time delay for which events can be scheduled */
/* #elif defined(PRIME_SSF_LTIME_DOUBLE) */
/*     const Time MINDELAY = .000001;	///< minimum time delay for which events can be scheduled */
/* #elif defined(PRIME_SSF_LTIME_LONGLONG) */
/*     const Time MINDELAY = 1;	///< minimum time delay for which events can be scheduled */
/* #elif defined(PRIME_SSF_LTIME_LONG) */
/*     const Time MINDELAY = 1;	///< minimum time delay for which events can be scheduled */
/* #else */
/* #error "Time must be defined as float, double, long or long long, or else simx/constants.h must be changed." */
/* #endif */

/* defining local mindelay values (in case 0 doesn't work), for same-process items. */
#if defined(PRIME_SSF_LTIME_FLOAT)
    const Time LOCAL_MINDELAY = .000001;	///< minimum time delay for which events can be scheduled
#elif defined(PRIME_SSF_LTIME_DOUBLE)
    const Time LOCAL_MINDELAY = .000001;	///< minimum time delay for which events can be scheduled
#elif defined(PRIME_SSF_LTIME_LONGLONG)
    const Time LOCAL_MINDELAY = 1;	///< minimum time delay for which events can be scheduled
#elif defined(PRIME_SSF_LTIME_LONG)
    const Time LOCAL_MINDELAY = 1;	///< minimum time delay for which events can be scheduled
#else
#error "Time must be defined as float, double, long or long long, or else simx/constants.h must be changed."
#endif



} // namespace

#endif

