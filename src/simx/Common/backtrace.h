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
// $Id: backtrace.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    backtrace.h
// Module:  Common
// Author:  Keith Bisset
// Created: July 23 2002
// Description: Functions for doing a stack backtrace, demangleing C++
//  symbols, if avaliable
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_COMMON_BACKTRACE
#define NISAC_COMMON_BACKTRACE
#include <string>

//--------------------------------------------------------------------------

namespace Common {

  /// Return the current function call stack as a string
  std::string Backtrace();

  /// Return the demangled form of the symbol name. The demangled name
  /// is returned if the system supports it, otherwise name is returned
  /// unchanged.  It is currently only supported on GCC 3.1 and above.
  /// If not supported, it just returns a copy of the argument.
  //
  /// Note that a static buffer is used by the system, so don't count on
  /// calling demangle several times in a row and using the results, link
  /// in an output.
  const char* demangle(const char* name);

  /// Return the demangled form of the symbol name. The demangled name
  /// is returned if the system supports it, otherwise name is returned
  /// unchanged.  It is currently only supported on GCC 3.1 and above.
  /// If not supported, it just returns a copy of the argument.
  std::string demangle(const std::string& name);

} // namespace

//--------------------------------------------------------------------------
#endif // NISAC_COMMON_BACKTRACE
//--------------------------------------------------------------------------
