//--------------------------------------------------------------------------
// File:    LogStreambuf.C
// Module:  Log
// Author:  Keith Bisset, C. David Tallman
// Created: January 13 2003
//
// Description: Streambuf that forwards its output to a log4cpp
// logging stream.  Adapted from the example on page 230 of "Standard
// C++ Streams and Locales" by Langer and Kreft.
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Log/LogStreambuf.h"
#include <iostream>

namespace Log {

//--------------------------------------------------------------------------

  LogStreambuf::LogStreambuf(Logger& log, int module, eLogLevel level)
    : fLog(log),
      fModule(module),
      fLevel(level)
  {
  }

//--------------------------------------------------------------------------

  LogStreambuf::~LogStreambuf()
  {
    sync();
  }

//--------------------------------------------------------------------------

  /// Stream insertion operator
  std::ostream& operator<<(std::ostream& os, eLogLevel l)
  {
    switch (l)
    {
    case kLevelFatal: return os << "FATAL"; 
    case kLevelError:return os << "ERROR"; 
    case kLevelWarning:return os << "WARN"; 
    case kLevelInfo:return os << "INFO"; 
    case kLevelDebug1:return os << "DEBUG1"; 
    case kLevelDebug2:return os << "DEBUG2"; 
    case kLevelDebug3:return os << "DEBUG3"; 
    case kLevelOff:return os << "OFF"; 
    case kLevelNotSet:return os << "NOTSET"; 
    default:return os << "UNKNOWN(" << static_cast<int>(l) << ")"; 
    }
  }

//--------------------------------------------------------------------------

  int LogStreambuf::sync()
  {
    fLog.output(fModule, fLevel, fBuffer);
    if (fLevel <= log().CoutLevel())
      std::cout << fLevel << ": " << fBuffer << std::flush;
    fBuffer = "";
    return 0;
  }

//--------------------------------------------------------------------------

  LogStreambuf::int_type LogStreambuf::overflow(int_type c)
  {
    if (fBuffer[fBuffer.size()-1] == '\n')
      fBuffer.insert(fBuffer.end(), '\t');    
    fBuffer.insert(fBuffer.end(), c);
    return c;
  }

}
