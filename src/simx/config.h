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
// File:    config.h
// Module:  simx
// Author:  Lukas Kroc
// Created: March 29 2005
// Description: basic config keys for simx
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_CONFIG
#define NISAC_SIMX_CONFIG

#include <string>

//--------------------------------------------------------------------------

namespace simx {

//--------------------------------------------------------------------------
// files

static const std::string ky_OUTPUT_FILE = "OUTPUT_FILE";

static const std::string ky_SERVICE_FILES = "SERVICE_FILES";

static const std::string ky_ENTITY_FILES = "ENTITY_FILES";

static const std::string ky_INFO_FILES = "INFO_FILES";


//--------------------------------------------------------------------------
// Other

/// simulation end time
static const std::string ky_END_TIME = "END_TIME";

/// number of LPs
static const std::string ky_NUMBER_LPS = "NUMBER_LPS";

/// random seed
static const std::string ky_SEED = "SEED";

/// MINDELAY
static const std::string ky_MINDELAY = "MINDELAY"; 

/// OUT-OF-BAND 
static const std::string ky_OUT_OF_BAND_MESSAGING = "OUT_OF_BAND_MESSAGING";

/// Services for the Controller
static const std::string ky_CONTROLLER_SERVICES = "CONTROLLER_SERVICES";

/// Input file (pipe) for the controller. If specified in .cfg file, will create
/// pipe(s) CONTROLLER_INPUT.<rank> into which one can write commands for Controller
/// (ended with '\n') on a specific machine. The pipe(s) are deleted at the end of 
/// the simulation.
static const std::string ky_CONTROLLER_INPUT = "CONTROLLER_INPUT";

/// [11/19/2008 by Guanhua Yan]
static const std::string ky_CONTROLLER_OUTPUT = "CONTROLLER_OUTPUT";
} // namespace

#endif 

//--------------------------------------------------------------------------
// end
//--------------------------------------------------------------------------
