//--------------------------------------------------------------------------
// File:    config.h
// Module:  simx
// Author:  Lukas Kroc
// Created: March 29 2005
// Description: basic config keys for simx
//
// @@COPYRIGHT@@
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
