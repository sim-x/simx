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
// $Id: Exception.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
//  Module: Common
//  File: Exception.C
//  Author: Bisset
//  Created: 11/13/00 11:21:24
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/Common/Exception.h"
#include "simx/Common/backtrace.h"
#include <cerrno>
#include <cstring>

using namespace std;

//--------------------------------------------------------------------------

#include <iostream>

namespace Common {

  Exception::Exception(const string& message)
    : fMessage(message),
      fBacktrace(Backtrace()),
      fErrno(errno)
  {

    if (fErrno > 0)
    {
      fMessage += " (";
      fMessage += strerror(fErrno);
      fMessage += ")";
    }
// LK: too long and useless
//    fMessage += "\nBacktrace:\n";
//    fMessage += "\tFile\t\tFunction\n";
//    fMessage += fBacktrace;
  }
 
  //--------------------------------------------------------------------------

  Exception::Exception(const Exception& e)
    : std::exception(e),
      fMessage(e.fMessage),
      fBacktrace(e.fBacktrace),
      fErrno(errno)
  {
  }

  //--------------------------------------------------------------------------

  Exception& Exception::operator=(const Exception& exception)
  {
    fMessage = exception.fMessage;
    fBacktrace = exception.fBacktrace;
    fErrno = exception.fErrno;
    return *this;
  }

  //--------------------------------------------------------------------------

  const string& Exception::GetMessage() const
  {
    return fMessage;
  }
  
  //--------------------------------------------------------------------------

  const string& Exception::GetBacktrace() const
  {
    return fBacktrace;
  }

  //--------------------------------------------------------------------------

  int Exception::Errno() const
  {
    return fErrno;
  }

  //--------------------------------------------------------------------------

  const char* Exception::what() const throw()
  {
    return fMessage.c_str();
  }

} // namespace

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
