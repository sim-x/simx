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
// @@COPYRIGHT@@
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
