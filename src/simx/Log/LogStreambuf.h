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
// @@COPYRIGHT@@
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
