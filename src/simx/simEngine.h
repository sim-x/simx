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
// File:    simEngine.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 25 2010
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_SIMENGINE
#define NISAC_SIMX_SIMENGINE


#include <iostream>
#include <algorithm>

#include "mpi.h"

#include "simx/type.h"
#include "simx/EventInfo.h"

#include <assert.h>


namespace simx {

namespace SimEngine {

//=======================================================
// control functions
//=======================================================

// initializes MPI
void init();

// returns the number of machines in the MPI world
int getNumMachs();	

// returns the rank of this machine
int getRank();

// 'prepares' for the simulation to be started
void prepare(Time start, Time end);

// runs the simulation, does not return untill the end
void run();

// shuts down MPI
void finalize();


//=======================================================
// utility functions
//=======================================================

// returns the current simulation time
Time getNow();

// packs an event, and sends it off
void sendEventInfo( LPID destLP, const simx::EventInfo& e );


} // namespace SimEngine

} // namespace simx

#endif

