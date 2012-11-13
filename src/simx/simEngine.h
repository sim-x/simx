//--------------------------------------------------------------------------
// File:    simEngine.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 25 2010
//
// @@COPYRIGHT@@
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

