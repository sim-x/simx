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
// File:    control.h
// Module:  simx
// Author:  Lukas Kroc
// Created: March 29 2005
//
// Description: 
//	Object that provides basic structure of simx simulation
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_SIMX
#define NISAC_SIMX_SIMX

#include "simx/type.h"

#include "loki/Singleton.h"

/// Mobicom simx
namespace simx {

class LP;

/// Object that provides basic structure of simx simulation
namespace Control {

    typedef std::map<LPID,LP*> LpPtrMap;
    
    /// Simulation phase
    enum eSimPhase {
        kPhaseInit,
        kPhaseRun,
        kPhaseWrapUp
    };


    /// SIMULATION SETUP AND CONTROL
	/// to initialize the program, must be called first before other methods in here
	/// \param Name of the module (e.g. PacketSim, SessionSim ...)
	void init(const std::string& modulename);
	
	/// prepares Output functionality
	void prepareOutput();
	
	/// prepares Services (might need initited Output)
	void prepareServices();
	
	/// creates Entities (needs prepared Services)
	void createEntities();

	/// TODO: LK: this should be abandoned, it's only here for ActivitySim. Use EntityManager::createEntity instead
        /// creates one entity 
//        void createEntity(EntityID, std::string, int);

	
	/// starts the simulation (reads in Info files, needs created Entities)
	/// DOESN'T RETURN UNTIL THE END OF SIMULATION
	void startSimulation();

    /// GETTERS
	/// returns number of computer nodes used (unix processes)
	int getNumMachines();
	/// returns rank of current process (0..getNumMachines()-1)
	int getRank();
	/// returns number of logial processes (LPs)
	int getNumLPs();
	/// return the processor name on this this process runs
	const std::string& getProcessorName();
	/// returns pointers to local LPs
	const LpPtrMap& getLpPtrMap();
    
    /// Returns the current phase of the simulation.
    eSimPhase getSimPhase();

} // Control namespace
 
} // simx namespace
#endif

