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
// File:    logger.C
// Module:  simx
// Author:  Lukas Kroc
// Created: May 6 2005
//
// Description: simx logging
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/logger.h"
#include "simx/simEngine.h"

#include "Log/Logger.h"

#include <limits>

//#include <boost/python.hpp>


/// unnamed namespace for internal logger stuff
namespace {

    using namespace simx;

#ifdef SIMX_USE_PRIME
    Time	simTime = std::numeric_limits<Time>::min();	///< the current simulaition time, updated by "setTime"
#endif
    int		moduleLogId = Log::log().registerModule("");	///< identification of the module for framework/Log

    /// prints the simTime if different from already printed
    /// called every time before log stream is returned, which makes
    /// sure that correct time of any log can be find out
    void printTime(std::ostream& os, const int level)
    {
	static Time lastPrintedTime = std::numeric_limits<Time>::min();	///< when was the last SimTime: message printed
	static int lastPrintedLevel = std::numeric_limits<int>::min();	///< what logging level was it done on

#ifndef SIMX_USE_PRIME
	Time simTime = SimEngine::getNow();
#endif
	
	/// print the time if not yet printed....
	/// ....or if the new level is lower (more important)
	if( lastPrintedTime != simTime ||
	    level < lastPrintedLevel )
	{
	    os << "============= SimTime: " << simTime << " =============" << std::endl;
	    lastPrintedTime = simTime;
	    lastPrintedLevel = level;
	}
    }

} // namespace

namespace simx {

namespace Logger {

    /// Return stream to be used for debug1 logging.
    std::ostream& debug1()
    {
	std::ostream& os = Log::log().debug1( moduleLogId );
	printTime( os, Log::kLevelDebug1 );
	return os;
    }

    /// Return stream to be used for debug2 logging.
    std::ostream& debug2()
    {
	std::ostream& os = Log::log().debug2( moduleLogId );
	printTime( os, Log::kLevelDebug2 );
	return os;
    }

    /// Return stream to be used for debug3 logging.
    std::ostream& debug3()
    {
	std::ostream& os = Log::log().debug3( moduleLogId );
	printTime( os, Log::kLevelDebug3 );
	return os;
    }

    /// Return stream to be used for info logging.
    std::ostream& info()
    {
	std::ostream& os = Log::log().info( moduleLogId );
	printTime( os, Log::kLevelInfo );
	return os;
    }

    /// Return stream to be used for warn logging.
    std::ostream& warn()
    {
	std::ostream& os = Log::log().warn( moduleLogId );
	printTime( os, Log::kLevelWarning );
	return os;
    }

    /// Return stream to be used for error logging.
    std::ostream& error()
    {
	std::ostream& os = Log::log().error( moduleLogId );
	printTime( os, Log::kLevelError );
	return os;
    }

    /// Log a failure message and exit.
    void failure(const std::string& message)
    {
	std::ostream& os = Log::log().error( moduleLogId );
	printTime( os, Log::kLevelFatal );
	Log::log().Failure( moduleLogId, message );
    }

    /// initializes logging stream
    void init(const std::string& moduleName)
    {
	moduleLogId = Log::log().registerModule( moduleName );
	
	/// set precision of log streams to high
	const unsigned prec = 20;
	Log::log().error( moduleLogId ).precision( prec );
	Log::log().warn( moduleLogId ).precision( prec );
	Log::log().info( moduleLogId ).precision( prec );
	Log::log().debug1( moduleLogId ).precision( prec );
	Log::log().debug2( moduleLogId ).precision( prec );
	Log::log().debug3( moduleLogId ).precision( prec );

	debug2() << "logger: registering moduleName " << moduleName << std::endl;
	
    }

#ifdef SIMX_USE_PRIME
    /// sets the current simulation time
    /// (MAY be set backwards, if entities being processed are not synchronized)
    void setTime(Time time)
    {
        simTime = time;
    }
#endif

} //Logger namespace

} //simx namespace

// using namespace boost;
// void export_logger() {
//   python::def("debug1",simx::Logger::debug1,
// 	      python::return_value_policy<python::reference_existing_object>())
//     ;
// }

