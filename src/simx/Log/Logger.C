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
// $Id: Logger.C,v 1.1.1.1 2011/08/18 22:19:45 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    Logger.C
// Module:  Log
// Author:  Paula Stretz
// Created: July 11, 2002
// Description: 
//	Methods for the Logger class.  Declaration of static global Logger
//  object, gLog and function for retrieval of the Logger object, log().
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/Log/Logger.h"
#include "simx/Log/Config.h"
#include "simx/Log/LogStreambuf.h"
#include "simx/Common/Assert.h"
#include "simx/Common/backtrace.h"
#include "simx/Common/Exception.h"
#include "simx/Common/Values.h"
#include "simx/Config/Configuration.h"

#include "simx/loki/Singleton.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <signal.h>
#include <sstream>
#include <sys/timeb.h>

using namespace std;

//--------------------------------------------------------------------------

namespace {

  // Utility routine used below in "transform()".
  int tolow( char ch )
  { return tolower( ch ); }

}

//--------------------------------------------------------------------------

namespace Log {

  // The logger object.  Accessible with TheLog::Instance()
  typedef Loki::SingletonHolder<Logger> TheLog;

/*
  // Constant definitions for debug levels.
  static const int kFatal = 0;
  static const int kError = 100;
  static const int kWarn = 200;
  static const int kInfo = 500;
  static const int kNoset = kInfo;
  static const int kDebug1 = 700;
  static const int kDebug2 = 725;
  static const int kDebug3 = 750;
*/

  //--------------------------------------------------------------------------
  // free functions
  //--------------------------------------------------------------------------

  // Call the Success method of the logger object
  void Success(int module = 0)
  {
    log().Success(module);
  }

  //--------------------------------------------------------------------------

  // Return the global logger object.
  Logger& log() { return TheLog::Instance(); }

  //--------------------------------------------------------------------------

  void sInitLog(int rank, string suffix)
  {
    using Config::gConfig;    

    if (log().isInitialized())
    { return; }

    string logname     = Common::Values::gProgName() + ".log";

    // Get values from configuration file.
    string modname     = ky_LOG_MODULE_NAME_default;
    string level       = ky_LOG_LEVEL_default;
    string coutlevel   = ky_LOG_COUT_LEVEL_default;
    string processname = ky_LOG_PROCESS_NAME_default;
    bool appendlog     = ky_LOG_APPEND_LOG_default;
    bool btrace        = ky_LOG_BACKTRACE_ON_ABORT_default;
    string action      = ky_LOG_ABORT_ACTION_default;
    bool nisacabort    = ky_LOG_ABORT_ON_NISAC_ASSERT_default;

    if (gConfig.IsBound(ky_LOG_FILE)) {
      gConfig.GetConfigurationValue(ky_LOG_FILE, logname);
    }

    if (gConfig.IsBound(ky_LOG_MODULE_NAME)) {
      gConfig.GetConfigurationValue(ky_LOG_MODULE_NAME, modname);
    }

    if (gConfig.IsBound(ky_LOG_LEVEL)) {
      gConfig.GetConfigurationValue(ky_LOG_LEVEL, level);
    }

    if (gConfig.IsBound(ky_LOG_COUT_LEVEL)) {
      gConfig.GetConfigurationValue(ky_LOG_COUT_LEVEL, coutlevel);
    }

    if (gConfig.IsBound(ky_LOG_PROCESS_NAME)) {
      gConfig.GetConfigurationValue(ky_LOG_PROCESS_NAME, processname);
    }

    if (gConfig.IsBound(ky_LOG_APPEND_LOG)) {
      gConfig.GetConfigurationValue(ky_LOG_APPEND_LOG, appendlog);
    }

    if (gConfig.IsBound(ky_LOG_BACKTRACE_ON_ABORT)) {
      gConfig.GetConfigurationValue(ky_LOG_BACKTRACE_ON_ABORT, btrace);
    }

    if (gConfig.IsBound(ky_LOG_ABORT_ACTION)) {
      gConfig.GetConfigurationValue(ky_LOG_ABORT_ACTION, action);
    }

    if (gConfig.IsBound(ky_LOG_ABORT_ON_NISAC_ASSERT)) {
      gConfig.GetConfigurationValue
	(ky_LOG_ABORT_ON_NISAC_ASSERT,nisacabort);
    }
  
    // Append the appropriate suffix to the log file name and thread
    // name if rank is specified.  Logfile suffix is of the form .AA,
    // .AB, ...  Process name is of the form <processname><rank>
    if (rank != -1) {
      logname += suffix;
      char tmp[50];
      sprintf(tmp, "%d", rank);
      processname += tmp;
    }
    

/*
    // Parse the individual module logging levels, if they exist.
    string dummy;
    if (gConfig.IsBound(ky_LOG_MODULE_LEVEL)) {
      gConfig.GetConfigurationValue(ky_LOG_MODULE_LEVEL, dummy);

      // Parse and store the module/level pairs in the log() object.
      replace(dummy.begin(), dummy.end(), ';',' ');
      stringstream dstream(dummy.c_str());
      while (static_cast<size_t> (dstream.tellg()) < dummy.length()) {
	string d;
	dstream >> d;
	if (d.find(':', 0) != string::npos) {
	  replace(d.begin(), d.end(), ':',' ');
	  stringstream ddstream(d.c_str());
	  string mod;
	  string thelevel;
	  ddstream >> mod;
	  ddstream >> thelevel;
	  log().setModuleLevel(mod, thelevel);
	}
      }
    }
*/

    // Initialize the logging object.
    log().Init
      (logname, modname, processname, appendlog, level, coutlevel);
    log().setAbortAction(action);
    log().setBacktraceOnAbort(btrace);
    log().setAbortOnNisacAssert(nisacabort);
  }

  //--------------------------------------------------------------------------

  /// Log_Assert Log/Logger.C Log/assert.h
  /// \brief The handler for NISAC_ASSERT
  /// 
  /// \todo Automagically find the current module number.
  void Log_Assert(const char *exp,
		  const char *func,
		  const char *file,
		  long line,
		  const char *msg)
  {
    log().error(0)
      << file << ": " 
      << func << " : "
      << exp << " failed at line "
      << line << ": "
      << msg
      << endl;

    if (log().getAbortOnNisacAssert())
    {
      log().Abort(0,msg);
    }
  }

  //--------------------------------------------------------------------------

  void Log_Checkpoint(const char *func,
		      const char *file,
		      long line,
		      const char *msg)
  {
    log().debug1(0) 
      << "Checkpoint - "
      << file << ": "
      << func << " : "
      << line << ": " 
      << msg 
      << endl;
  }

  //--------------------------------------------------------------------------
  
  // Return an integer representation of the priority based on the
  // given string level.
  int findPriority(string level)
  {
    // lowercase level to simplify comparisons
    string lc_level( level );
    transform( lc_level.begin(), lc_level.end(),
	       lc_level.begin(),
	       tolow );

    if (lc_level == "info")
      return kLevelInfo;
    else if (lc_level == "warn")
      return kLevelWarning;
    else if (lc_level == "error")
      return kLevelError;
    else if (lc_level == "debug3")
      return kLevelDebug3;
    else if (lc_level == "debug2")
      return kLevelDebug2;
    else if (lc_level == "debug1")
      return kLevelDebug1;
    else if (lc_level == "fatal")
      return kLevelFatal;
    else
      return kLevelNotSet;
  }

  //--------------------------------------------------------------------------

  // Return an integer representation of the level based on the
  // given string level.
  eLogLevel findLevel(const string& level)
  {
    // lowercase level to simplify comparisons
    string lc_level( level );
    transform( lc_level.begin(), lc_level.end(),
	       lc_level.begin(),
	       tolow );

    if (lc_level == "info")
      return kLevelInfo;
    else if (lc_level == "warn")
      return kLevelWarning;
    else if (lc_level == "error")
      return kLevelError;
    else if (lc_level == "debug3")
      return kLevelDebug3;
    else if (lc_level == "debug2")
      return kLevelDebug2;
    else if (lc_level == "debug1")
      return kLevelDebug1;
    else if (lc_level == "fatal")
      return kLevelFatal;
    else
      return kLevelNotSet;
  }

  //--------------------------------------------------------------------------
  // AbortHandler
  //--------------------------------------------------------------------------

  //Virtual destructor for AbortHandler class.
  AbortHandler::~AbortHandler() {}

  //--------------------------------------------------------------------------
  // Logger
  //--------------------------------------------------------------------------

  // Constructor for Logger object.
  Logger::Logger() : 
    nullostream(),
    fCurrentModule(1),
    fInitialized(false),
    fBacktraceOnAbort(true),
    fCoutLevel( kLevelOff ),
    fAbortAction(LOG_ACTION_EXIT),
    fDefaultLevel("INFO"),
    fStream( NULL ),
    fLevel( kLevelOff ),
    fModulename("")
  { 
  }

  //--------------------------------------------------------------------------

  // Destructor for Logger object.
  Logger::~Logger()
  {
    MapIterator mit;
    for (mit = fModuleLevel.begin(); mit != fModuleLevel.end(); ++mit) {
      delete (*mit).second;
    }
    fModuleLevel.clear();
    fModuleCategories.clear();
    for (unsigned i=0; i < fStreams.size(); ++i)
      for (unsigned j=0; j < fStreams[i].size(); ++j)
	if (fStreams[i][j] != 0)
	{
	  delete fStreams[i][j];
	}
  }

  //--------------------------------------------------------------------------

  // Initialize the logger.  Called by free function sInitLog() that
  // initializes configuration values.
  void 
  Logger::Init(string logname, string subsystem,
	       string threadname, bool append_log,
	       string default_level,
	       string cout_level)
  {
    if (!fInitialized) {

/*      
      fDefaultLevel = default_level;

      // Create the default Category and store in 0th position of
      // fModuleCategories vector.  Create an entry in the Map for the
      // module, logging level, and module ID.
      int curMod = fCurrentModule;
      fCurrentModule = 1;

      CategoryMapPair
	pr(0, new Stream(subsystem) );
      fModuleCategories.insert(pr);

      fModuleCategories[0]->fPriority = Log::findPriority(default_level);
      fCoutLevel = findLevel(cout_level);

      setModuleLevel(subsystem, fDefaultLevel);
      fCurrentModule = curMod;
		

      // Reregister the modules registered before we were initialized
      for (Map::iterator it = fModuleLevel.begin();
	   it != fModuleLevel.end();
	   ++it)
      {
        registerModule(it->first);
      }
*/

      fLevel = findLevel(default_level);
      fCoutLevel = findLevel(cout_level);
      
      fStream = new ofstream( logname.c_str() );
      if( !fStream )
      {
	error(0) << "Cannot initialize log" << endl;
      }
    
      fInitialized = true;

    }
  }

  //--------------------------------------------------------------------------

  // Set the log level for the given module.  Module names/levels are
  // stored in the fModuleLevel map.
  void Logger::setModuleLevel(string module, string level)
  {
    ModInfo* mi = static_cast<ModInfo *> (0);
    MapIterator mit = fModuleLevel.find(module);
    int priority = Log::findPriority(level);

    if (mit == fModuleLevel.end()) {
      // New module-level pair. Find priority and assign module ID.
      mi = new ModInfo();
      mi->fLevel = priority;
      mi->fModuleId = -1;
      MapPair p(module, mi);
      fModuleLevel.insert(p);
    } else {
      // Module already in map.
      mi = (*mit).second;
      mi->fLevel = priority;
    }

    if (fModuleCategories.count(mi->fModuleId) != 0) {
      CategoryMapIterator cit = fModuleCategories.find(mi->fModuleId);
      (*cit).second->fPriority = mi->fLevel;
    } 
  }

  //--------------------------------------------------------------------------

  // Get the log level for the given module.  Default level is
  // kLevelInfo for those modules not found in the map.
  Logger::ModInfo* Logger::getLogLevel(string module)
  {
    if (!fModuleLevel.empty() && (fModuleLevel.count(module) != 0)) {
      MapIterator mit = fModuleLevel.find(module);
      return (*mit).second;
    }
    return static_cast<ModInfo *> (0);
  }

  //--------------------------------------------------------------------------

  // Create a log4cpp::Category instance and store in vector indexed
  // by the give module ID.
  void Logger::createCategory(const ModuleId mod, const string modulename, 
			      int level)
  {
    if (fModuleCategories.count(mod) == 0) {
      CategoryMapPair 
	pr(mod, new Stream(modulename));
      fModuleCategories.insert(pr);
      fModuleCategories[mod]->fPriority = level;
    }
  }

  //--------------------------------------------------------------------------

  // Return a module ID for the given module.
  int Logger::registerModule(string modulename)
  {

//    if( fModulename == "" )  
//	fModulename = modulename;

    return 0;
/*
    //     if (!isInitialized()) {
    //       throw Common::Exception
    // 	("Logger is not initialized..Call Init() method before registerModule");
    //     }

    ModuleId id = fCurrentModule;
    ModInfo *mp = getLogLevel(modulename);

    if (mp) {
      // Module is already in Map of module names and logging levels
      if (mp->fModuleId < 0) {
	mp->fModuleId = id;
      } else {
	// Module already has a valid ID.
	id = mp->fModuleId;
      }
    } else {
      // New module registering.  Add to map of known modules.
      setModuleLevel(modulename, fDefaultLevel);
      mp = (*fModuleLevel.find(modulename)).second;
      mp->fModuleId = id;
    }

    // New module
    if (id == fCurrentModule)
      fCurrentModule++;

    if (isInitialized()) {
      if (fModuleCategories.count(mp->fModuleId) == 0) {
	createCategory(mp->fModuleId, modulename, mp->fLevel);

	// Initialize associated ostreams conatiner
	while (fStreams.size() < static_cast<unsigned>(fCurrentModule))
	{
	  vector<ostream*> streams;
	  for (int i=0; i <= kLevelNotSet; ++i)
	    streams.push_back(0);
	  fStreams.push_back(streams);
	}
      }
    }
    return static_cast<int> (mp->fModuleId);
*/
  }

  //--------------------------------------------------------------------------

  // Add an abort handler.
  void Logger::addAbortHandler(AbortHandler* handler)
  {
    fgAbortHandlers.push_back(handler);
  }

  //--------------------------------------------------------------------------

  // Log a failure message and exit.
  void Logger::Failure(int module, string message)
  {
    if (fModuleCategories.count(module) > 0) {
      error(module) << "FAILURE "<< message << endl;
    } else {
      error(0) << "FAILURE "<< message << endl;
    }

    // Call exit handlers then return non zero value.
    Abort(module, message);
  }

  //--------------------------------------------------------------------------

  // Log an abort message and exit.
  void Logger::Abort(int module, string message)
  {
    int tmod = module;
    if (tmod >= fCurrentModule) {
      tmod = 0;
    } 
	
    // if (fBacktraceOnAbort)
    //   error(0) << Common::Backtrace();
  
    for (unsigned int i = 0; i < fgAbortHandlers.size(); ++i)
      fgAbortHandlers[i]->Cleanup();

    switch(fAbortAction) {
    case LOG_ACTION_ABORT:
      abort();
    case LOG_ACTION_EXIT:
      exit(EXIT_FAILURE);
    case LOG_ACTION_EXCEPTION:
      throw Common::Exception(message);
    default:
      abort();
    }
  }

  //--------------------------------------------------------------------------

  // Log an assert message and exit.
  void Logger::Assert(const string file, const string condition, 
		      int line, int module)
  {
    int mID = module;
    // Use the default module if value is out of range.
    if (mID >= fCurrentModule) {
      mID = 0;
    }
    error(0) 
      << "FAILURE "
      << file 
      << ":"
      << line 
      << " failed assertion '"
      << condition
      << "'" 
      << endl;

    Abort(module, condition);
  }

  //--------------------------------------------------------------------------

  // Set the abort action from a string name.
  void Logger::setAbortAction(string action)
  {

    // lowercase action to simplify comparisons
    string lc_action( action );
    transform( lc_action.begin(), lc_action.end(),
	       lc_action.begin(),
	       tolow );

    if (lc_action == "exit")
      setAbortAction(LOG_ACTION_EXIT);
    else if (lc_action == "exception")
      setAbortAction(LOG_ACTION_EXCEPTION);
    else if (lc_action == "abort")
      setAbortAction(LOG_ACTION_ABORT);
  }

  //--------------------------------------------------------------------------

  // Return an iostream, creating it if necessary
  ostream& Logger::ios(ModuleId module, eLogLevel level)
  {
    if (module < 0)
    {
      log().warn(0)
	<< "Logger passed invalid id: " 
	<< module << endl;
      module = 0;
    }

    if( !fStream )
	return( nullostream );

    if( level > fLevel )
	return( nullostream );

    time_t tt;
    time( &tt );
    stringstream ss;
    ss << ctime( &tt );
    string prefix;
    getline( ss, prefix );

    *fStream << prefix << "\t" << level << "\t";

    return *fStream;

/*
    //      TODO: Return a /dev/null ostream if we ask for a debuglevel
    //      TODO: not supported by the module
    for (Map::iterator i = fModuleLevel.begin(); i != fModuleLevel.end(); ++i) {
      if (i->second->fModuleId == module) {
        if (level > i->second->fLevel) {
          return(nullostream);
        }
      }
    }

    if( !fInitialized )
          return(nullostream);
    

    if (fStreams[module][level] == 0)
    {
      fStreams[module][level] =
	new ostream(new Log::LogStreambuf(*this, module, level));
    }
    return *fStreams[module][level];
*/
  }

  //--------------------------------------------------------------------------
  
} // namespace

//--------------------------------------------------------------------------
// end
//--------------------------------------------------------------------------
