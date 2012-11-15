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
// $Id: Exception.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
//  Module: Common
//  File: Exception.h
//  Author: Bisset
//  Created: 11/13/00 11:21:24
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_COMMON_EXCEPTION
#define NISAC_COMMON_EXCEPTION

//--------------------------------------------------------------------------

#include <exception>
#include <string>

//--------------------------------------------------------------------------

namespace Common {

  /// \class Exception Exception.h "Common/Exception.h"
  ///
  /// \brief An exception signals the failure of a member function.
  ///
  /// An Exception message contains:
  /// -# the message used to construct the exception
  /// -# a system error message, if errno > 0 when the excpetion was
  /// thrown
  /// -# a stack backtrace when the exception was thrown

  class Exception : public std::exception 
  {
  public:

    ///  Construct an exception with the specified message text.
    Exception(const std::string& message = "Unknown NISAC error.");

    ///  Construct a copy of the given exception.
    Exception(const Exception& exception);

    ///  Destroy an exception.
    virtual ~Exception() throw() {}

    ///  Make the exception a copy of the given exception.
    Exception& operator=(const Exception& exception);

    ///  Return the message text for the exception.
    const std::string& GetMessage() const;

    ///  Return the backtrace from the spot the exception was thrown
    const std::string& GetBacktrace() const;

    /// Standard function.  Returns the same information as GetMessage().
    virtual const char* what() const throw();
      
    /// Errno at the time the exception was thrown
    int Errno() const;
      
  private:

    // What caused the exception.
    std::string fMessage;

    // stack Backtrace from spot thrown
    std::string fBacktrace;

    // errno when exception was thrown
    int fErrno;
  };

} // namespace

//--------------------------------------------------------------------------
#endif // NISAC_COMMON_EXCEPTION
//--------------------------------------------------------------------------
