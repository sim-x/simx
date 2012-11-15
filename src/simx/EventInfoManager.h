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
// File:    EventInfoManager.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 12 2005
//
// Description: 
//	Event for InfoManager wakeups when new info is in files
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_EVENTINFOMANAGER
#define NISAC_SIMX_EVENTINFOMANAGER

#include "simx/type.h"
#include "simx/EventInfo.h"

namespace simx {

class PackedData;
class LP;

/// \class EventInfoManager EventInfoManager.h "simx/EventInfoManager.h"
///
/// \brief Event for InfoManager deliveries.
///
/// Holds information about an InfoManager delivery
/// When en event gets send (e.g. LP::sendInfoManager), the content of fInfoManager is NOT copied,
/// so changing the content subsequently is undefined (whether the changes get send or not)
class EventInfoManager : public EventInfo
{
    public:
	/// constructor with params
	/// \param ID of the file to look into
	/// \param delay after which it should fire
	EventInfoManager(int fid, Time delay);
	/// default constructor (to make it possible to unpack)
	EventInfoManager();

	/// pack EventInfoManager
        virtual void pack(PackedData&) const;
	/// unpack EventInfoManager
        virtual void unpack(PackedData&);

        /// Print EventInfoManager
	virtual void print(std::ostream& os) const;

        /// Execute operation
        virtual void execute(void);

    // GETTERS:
	/// returns a file id that should be examined
	int	getFileId() const;

	/// returns delay with whith it should be delivered
	Time	getDelay() const;

    protected:
    private:
	int	fFileId;	///< file id that has new data available
	Time	fDelay;

};


//============================================================================

inline std::ostream& operator<<(std::ostream& os, const EventInfoManager& e)
{
    e.print(os);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const EventInfoManager* ep)
{
    if(ep) { ep->print(os); } 
    else  { os << "NULL"; }
    return os;
}

} // namespace
#endif 


