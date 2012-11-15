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
// File:    LP.h
// Module:  simx
// Author:  K. Bisset
// Created: June 25 2004
//
// Description:
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_LP
#define NISAC_SIMX_LP

#include "simx/type.h"

#include "Random/Random.h"

#include <vector>

namespace simx {

#ifdef SIMX_USE_PRIME
    class DassfLP;
#endif

class EventInfo;
class EventInfoManager;
class Entity;
class ControlInfoWrapper;

/// \class LP LP.h "simx/LP.h"
///
/// \brief Logical Process.  Handle the interface to the simulation.
///
/// This should be the only place we directly interface to DaSSF.
/// This allows us to easily replace it in the future.  LPs and there
/// interconnections are fixed during the simulation.  For now, the
/// LPs are fully connected.  This can be relaxed, but it requires
/// supported entity migration.
class LP
{
    public:
	/// Construct an LP.
	/// \param id the id of this LP, must be globally unique
	explicit LP(LPID id);
	
	~LP();
	
	/// return the id of this LP
	LPID getId() const;

	/// returns associated random stream
	Random::TRandom& getRandom() const;
  
	/// return current simulation time.
	Time getNow() const;

	/// Send an info event to another entity
	void sendEventInfo(EventInfo& e) const;

	/// Sends an event notifying the InfoManager that it should read new chunk of data from input
	void sendEventInfoManager(EventInfoManager& e) const;

	//for sending out-of-band info
	void sendControlInfo( ControlInfoWrapper& cinfo );

	static Time MINDELAY;
	
    protected:
    private:
	LPID 		fId;     	///< Our id

#ifdef SIMX_USE_PRIME
	DassfLP*	fDassfLP;	///< the DassfLP object
#endif

	mutable Random::TRandom	fRandom;	///< random stream to use with this LP
						///< mutable so that we can get a number even from const LP&

	/// these aren't needed, intentionally left unimplemented
	LP(const LP& rhs);
	LP& operator=(const LP& rhs);

};
 


} // namespace

#endif // NISAC_SIMX_LP

