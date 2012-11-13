//--------------------------------------------------------------------------
// File:    constants.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Apr 13 2005
//
// Description: simx special constants (e.g. service names and addresses)
//
// @@COPYRIGHT@@
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

