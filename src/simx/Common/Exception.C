//--------------------------------------------------------------------------
// $Id: Exception.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
//  Module: Common
//  File: Exception.C
//  Author: Bisset
//  Created: 11/13/00 11:21:24
//
// @@COPYRIGHT@@
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
