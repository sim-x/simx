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
// $Id: Config.h,v 1.1.1.1 2011/08/18 22:19:45 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    Config.h
// Module:  Log
// Author:  Paula Stretz
// Created: July 11, 2002
// Description: 
//		Configuration file key definitions for the Logger class.
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_LOG_CONFIG_H
#define NISAC_LOG_CONFIG_H
#include <string>

//--------------------------------------------------------------------------

namespace Log {

  /// Name of the log file and the default value.
  static const std::string ky_LOG_FILE = "LOG_FILE";
  /// Default value:  Global::gProgName + ".log"

  /// Default module name used in logging.
  static const std::string ky_LOG_MODULE_NAME = "LOG_MODULE_NAME";
  /// Default value for config key LOG_MODULE_NAME
  static const std::string ky_LOG_MODULE_NAME_default = "Unknown";

  /// Default logging level to log file.
  /// Used for all modules not explicitly set with the LOG_MODULE_LEVEL
  /// key.
  static const std::string ky_LOG_LEVEL = "LOG_LEVEL";
  /// Default value for config key LOG_LEVEL
  static const std::string ky_LOG_LEVEL_default = "INFO";

  /// Default logging level to log stdout.
  /// Used for all modules not explicitly set with the LOG_MODULE_LEVEL
  /// key.
  static const std::string ky_LOG_COUT_LEVEL = "LOG_COUT_LEVEL";
  /// Default value for config key LOG_COUT_LEVEL
  static const std::string ky_LOG_COUT_LEVEL_default = "WARN";

  /// Process (thread) name used in logging.
  static const std::string ky_LOG_PROCESS_NAME = "LOG_PROCESS_NAME";
  /// Default value for config key LOG_PROCESS_NAME
  static const std::string ky_LOG_PROCESS_NAME_default = "t0";

  /// If true log is appended, otherwise log file started.
  static const std::string ky_LOG_APPEND_LOG  = "LOG_APPEND_LOG";
  /// Default value for config key LOG_APPEND_LOG
  static const bool ky_LOG_APPEND_LOG_default  = false;

  /// If true a stack backtrace is printed from the Logger::Abort method.
  static const std::string ky_LOG_BACKTRACE_ON_ABORT = 
  "LOG_BACKTRACE_ON_ABORT";
  /// Default value for config key LOG_BACKTRACE_ON_ABORT
  static const bool ky_LOG_BACKTRACE_ON_ABORT_default = true;

  /// Set the action to be taken on abort by Logger::Failure() and
  /// Logger::Abort().  Valid values are ABORT, EXIT, and EXCEPTION.
  static const std::string ky_LOG_ABORT_ACTION = "LOG_ABORT_ACTION";
  /// Default value for config key LOG_ABORT_ACTION
  static const std::string ky_LOG_ABORT_ACTION_default  = "ABORT";

  /// Should we abort on a failed NISAC_ASSERT?
  static const std::string ky_LOG_ABORT_ON_NISAC_ASSERT = 
  "LOG_ABORT_ON_NISAC_ASSERT";
  static const bool ky_LOG_ABORT_ON_NISAC_ASSERT_default = true;

  /// Set logging levels for individual modules.  
  /// \verbatim
  /// Format:  LOG_MODULE_LEVEL \modulename1>:<level>[;<modulename2:level>...]
  /// Examples:
  /// LOG_MODULE_LEVEL File:debug1;Config:info
  /// LOG_MODULE_LEVEL Config:info
  /// \endverbatim
  static const std::string ky_LOG_MODULE_LEVEL = "LOG_MODULE_LEVEL";

} // namespace
#endif

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
