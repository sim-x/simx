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
// File:    logger.h
// Module:  simx
// Author:  Lukas Kroc
// Created: May 6 2005
//
// Description: simx logging
//
// @@
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

