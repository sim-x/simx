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
// File:    LogStreambuf.h
// Module:  Log
// Author:  Keith Bisset
// Created: January 13 2003
//
// Description: Streambuf that forwards its output to a log4cpp
// logging stream.  Adapted from the example on page 230 of "Standard
// C++ Streams and Locales" by Langer and Kreft.
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_LOG_LOGSTREAMBUF
#define NISAC_LOG_LOGSTREAMBUF

//--------------------------------------------------------------------------

#include "simx/Log/Logger.h"

#include <streambuf>
//#include <iostream>
#include <iosfwd>
#include <string>

//--------------------------------------------------------------------------

namespace Log {

  class Logger;

  /// \class LogStreambuf LogStreambuf.h "Log/LogStreambuf.h"
  ///
  /// \brief Streambuf that forwards its output to a log4cpp
  /// logging stream.
  ///
  /// Adapted from the example on page 230 of "Standard
  /// C++ Streams and Locales" by Langer and Kreft.
  class LogStreambuf : 
    public std::basic_streambuf<char, std::char_traits<char> >
  {
  public:

    /// Construct a streambuf
    LogStreambuf(Logger& log, int module, eLogLevel level);
    ~LogStreambuf();
    
  protected:

    int_type overflow(const int_type c = traits_type::eof());

    int sync();
    
  private:

    Logger& fLog;
    int fModule;
    eLogLevel fLevel;
    
    std::string fBuffer;
    
    // Unimplemented to prevent copying
    LogStreambuf(const LogStreambuf&);
    LogStreambuf& operator=(const LogStreambuf&);
  };

  std::ostream& operator<<(std::ostream& os, eLogLevel l);

} // namespace

#endif // NISAC_LOG_LOGSTREAMBUF

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
