//--------------------------------------------------------------------------
// $Id: Logger.h,v 1.1.1.1 2011/08/18 22:19:45 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    Logger.h
// Module:  Log
// Author:  Paula Stretz
// Created: July 11, 2002
// Description: 
//		Logger class definition and inline methods.
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_LOG_LOGGER_H
#define NISAC_LOG_LOGGER_H

//--------------------------------------------------------------------------

#include "simx/Common/Exception.h"
#include "simx/Log/nullstream.h"

#include <iosfwd>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <stdlib.h>

//--------------------------------------------------------------------------

namespace Log {

  extern int findPriority(std::string);
 

  // Log::log(): a reference to the singleton Logger object.
  class Logger;
  extern Logger& log();

  // Initialize from configuration file.
  // Configuration file keys:
  // LOG_FILE, LOG_MODULE_NAME, LOG_LEVEL, LOG_COUT_LEVEL,
  // LOG_PROCESS_NAME, LOG_APPEND_LOG, LOG_BACKTRACE_ON_ABORT,
  // LOG_ABORT_ACTION, LOG_ABORT_ON_NISSAC_ASSERT
  extern void sInitLog(int rank=-1, std::string suffix = "");

  /// Logging stream
  typedef std::ostream LogStream;

  /// module identification
  typedef int ModuleId;

  /// Logging levels.
  /// Settable per module from configuration keys: OFF, FATAL, WARN,
  /// ERROR, INFO, DEBUG1, DEBUG2, DEBUG3
  typedef enum loglevel_e {
    kLevelOff = -1,		/// Off
    kLevelFatal = 0,		/// Fatal
    kLevelError = 300,		/// Error
    kLevelWarning = 400,	/// Warning
    kLevelInfo = 600,		/// Info
    kLevelDebug1 = 700,		/// Debug
    kLevelDebug2 = 725,		/// Debug
    kLevelDebug3 = 750,		/// Debug
    kLevelNotSet = 800		/// Unset
  } eLogLevel;

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
 
  /// \class AbortHandler Logger.h "Log/Logger.h"
  ///
  /// \brief Base class for abort handlers.
  /// Each derived class should define a Cleanup method to perform any
  /// needed action upon cleanup.
  class AbortHandler
  {
  public:
    /// Constructor
    AbortHandler(){};

    /// Destructor
    virtual ~AbortHandler();

    /// Clean up method.
    virtual void Cleanup()=0;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  /// \class Logger Logger.h "Log/Logger.h"
  ///
  /// \brief Wrapper class around the log4cpp logging mechanism.
  ///
  /// \sa http://log4cpp.sourceforge.net/
  class Logger
  {
  public:
  
    /// Action to take on abort
    typedef enum AbortAction_e {
      LOG_ACTION_ABORT,           /// call abort() (dumps core)
      LOG_ACTION_EXIT,            /// call exit()
      LOG_ACTION_EXCEPTION        /// throw an exception
    } AbortAction;

    // Structure to hold module level and assigned id.
    typedef struct modinfo_s {
      int  fLevel;
      ModuleId fModuleId;
    } ModInfo;

    //--------------------------------------------------------------------------

    /// Constructor
    Logger();
  
    /// Destructor
    ~Logger();

    /// Initialize the logger.  Called by free function sInitLog.
    void Init(std::string logname="logger.log",
	      std::string programname="UNKNOWN",
	      std::string processname="t0",
	      bool append_log=false,
	      std::string default_level="INFO",
	      std::string cout_level="WARN");

    /// Store the module name and associated level in the fModuleLevel
    /// map.
    void setModuleLevel(std::string module, std::string level);

    /// Return a module ID for the given module.  Multiple calls with
    /// the same modulename will return the same id
    int registerModule(std::string modulename);

    /// Add an abort handler.
    void addAbortHandler(AbortHandler* handler);

    /// Set action to take when abort method is called.
    void setAbortAction(AbortAction action);
    /// Set action to take when abort method is called.
    void setAbortAction(std::string action);

    /// Set value of boolean that controls whether a backtrace is
    /// logged from the Abort method.
    void setBacktraceOnAbort(bool val);

    /// Set a bool that determines whether we abort when a
    /// NISAC_ASSERT fails
    void setAbortOnNisacAssert(bool aona);
    bool getAbortOnNisacAssert() const;

    /// Write buffer to appropriate stream
    void output(ModuleId module, eLogLevel level,
		std::string buffer);

    /// Return stream to be used for debug1 logging.
    std::ostream& debug1(int moduleId);
    /// Return stream to be used for debug2 logging.
    std::ostream& debug2(int moduleId);
    /// Return stream to be used for debug3 logging.
    std::ostream& debug3(int moduleId);
    /// Return stream to be used for info logging.
    std::ostream& info(int moduleId);
    /// Return stream to be used for warn logging.
    std::ostream& warn(int moduleId);
    /// Return stream to be used for error logging.
    std::ostream& error(int moduleId);

    std::ostream& ios(ModuleId module, eLogLevel level);
  
    /// Log a success message and exit.
    void Success(int module);

    /// Log a failure message and exit.
    void Failure(int module, std::string message);

    /// Do final cleanup and exit
    ///
    /// -# Print backtrace if specified
    /// -# Call abort handlers in the same order they were added
    /// -# Terminate according to ABORT_ACTION
    ///
    /// \todo Make this function private.  Maybe change name to
    /// terminate?
    void Abort(int module, std::string message);

#undef Assert

    /// Log an assert message and terminate.
    void Assert(const std::string file,
		const std::string condition,
		int line,
		int module=0);

    /// Return true if this logger has been initialized.
    bool isInitialized() const;

    /// Return level at which we should start writing messages to cout
    /// as well as the log file
    eLogLevel CoutLevel() const;

  private:

    typedef std::map<std::string, ModInfo*, std::less<std::string> > Map;
    typedef Map::iterator  MapIterator;
    typedef Map::const_iterator MapConstIterator;
    typedef std::pair<std::string const,  ModInfo *> MapPair;

    // LK: substitute for log4cpp stream
    struct Stream
    {
	Stream(const std::string name) : fStream( name.c_str() ) {};
	std::ofstream fStream;
	int fPriority;
    };

    typedef std::map<ModuleId, Stream*, std::less<ModuleId> >
    CategoryMap;
    typedef CategoryMap::iterator  CategoryMapIterator;
    typedef CategoryMap::const_iterator CategoryMapConstIterator;
    typedef std::pair<ModuleId const,  Stream*> CategoryMapPair;

    typedef std::vector<std::vector<std::ostream*> > OStreamVec;

    NullStream  nullostream;  

    // return the current log level for the module.
    ModInfo *getLogLevel(std::string module);

    // Create a log4cpp::Category instance and store in vector indexed
    // by the given module ID.
    void createCategory(const ModuleId mod,
			const std::string modulename,
			int level); 
  
    // Next ModuleId for registering modules.
    ModuleId fCurrentModule;

    // True if the Logger's Init() method has been invoked.
    bool fInitialized;

    // If true, print a stack backtrace from the Abort method.
    bool fBacktraceOnAbort;

    // Level where we should start displaying messgaes to cout as well
    eLogLevel fCoutLevel;
  
    // Map between module names and logging level set via
    // configuration file keys.
    Map fModuleLevel;

    // Map of module IDs to log4cpp::Category objects.
    CategoryMap fModuleCategories;

    // The abort handlers
    std::vector<AbortHandler*> fgAbortHandlers;

    // The abort action
    AbortAction fAbortAction;

    // Do we abort on a failed NISAC_ASSERT_* ?
    bool fAbortOnNisacAssert;
  
    // the default logging level
    std::string fDefaultLevel;

    // Ostreams assciated with each logging stream
    OStreamVec fStreams;
    
    std::ostream	*fStream;
    eLogLevel		fLevel;
    std::string		fModulename;
  };

  //--------------------------------------------------------------------------

  inline void Logger::setBacktraceOnAbort(bool val)
  { fBacktraceOnAbort = val; }

  inline bool Logger::isInitialized() const
  {return fInitialized;}

  inline eLogLevel Logger::CoutLevel() const 
  {return fCoutLevel;}

  inline std::ostream& Logger::error(ModuleId module)
  {
    return ios(module, kLevelError);
  }

  inline std::ostream& Logger::warn(ModuleId module)
  {
    return ios(module, kLevelWarning);
  }

  inline std::ostream& Logger::info(ModuleId module)
  {
    return ios(module, kLevelInfo);
  }

  //--------------------------------------------------------------------------

  inline std::ostream& Logger::debug1(ModuleId module)
  {
    return ios(module, kLevelDebug1);
  }

  inline std::ostream& Logger::debug2(ModuleId module)
  {
    return ios(module, kLevelDebug2);
  }

  inline std::ostream& Logger::debug3(ModuleId module)
  {
    return ios(module, kLevelDebug3);
  }

  //--------------------------------------------------------------------------

  inline void Logger::output(ModuleId module,
			     eLogLevel level, 
			     std::string buffer)
  {
    if (fModuleCategories.count(module) == 0) {
      throw Common::Exception("Logger::info()..invalid module ID");
    }

    switch (level)
    {
    case kLevelFatal:
      // fall through
    case kLevelError:
      fModuleCategories[module]->fStream << buffer;
      break;
    case kLevelWarning:
      fModuleCategories[module]->fStream << buffer; 
      break;
    case kLevelInfo:
      fModuleCategories[module]->fStream << buffer;
      break;
    case kLevelDebug1:
      // fall through
    case kLevelDebug2:
      // fall through
    case kLevelDebug3:
      if (fModuleCategories[module]->fPriority >= level)
        fModuleCategories[module]->fStream << buffer;
      break;
    case kLevelOff:
      // fall through
    case kLevelNotSet:
      // fall through
    default:
      ; // do nothing
    }

  }

  //--------------------------------------------------------------------------

  inline void Logger::Success(ModuleId module) {
    if (fModuleCategories.count(module) == 0) {
      throw Common::Exception("Logger::success()..invalid module ID");
    }
    info(module) << "SUCCESS exit" << std::endl;
    exit(EXIT_SUCCESS);
  }

  //--------------------------------------------------------------------------

  // Set the abort action.
  inline void Logger::setAbortAction(AbortAction action) 
  { fAbortAction = action;}

  inline void Logger::setAbortOnNisacAssert(bool aona)
  { fAbortOnNisacAssert = aona; }

  inline bool Logger::getAbortOnNisacAssert() const
  { return(fAbortOnNisacAssert); }

  //--------------------------------------------------------------------------


} //namespace

#endif // NISAC_LOG_LOGGER_H

//--------------------------------------------------------------------------
// Logger.h
//--------------------------------------------------------------------------
