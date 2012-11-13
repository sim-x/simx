//--------------------------------------------------------------------------
// File:    logger.h
// Module:  simx
// Author:  Lukas Kroc
// Created: May 6 2005
//
// Description: simx logging
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_LOGGER
#define NISAC_SIMX_LOGGER

#include "simx/type.h"
#include "simx/logger.h"

namespace simx {

namespace Logger {

    /// Return stream to be used for debug1 logging.
    std::ostream& debug1();
    /// Return stream to be used for debug2 logging.
    std::ostream& debug2();
    /// Return stream to be used for debug3 logging.
    std::ostream& debug3();
    /// Return stream to be used for info logging.
    std::ostream& info();
    /// Return stream to be used for warn logging.
    std::ostream& warn();
    /// Return stream to be used for error logging.
    std::ostream& error();

    /// Log a failure message and exit.
    void failure(const std::string& message);
    
    /// initializes logging stream
    void init(const std::string& moduleName);

#ifdef SIMX_USE_PRIME
    /// sets the current simulation time
    /// (MAY be set backwards, if entities being processed are not synchronized)
    void setTime(Time time);
#endif

} // Logger namespace


} //simx namespace

#endif

