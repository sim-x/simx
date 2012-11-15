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
// File:    EventInfoManager.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 12 2005
//
// Description: 
//	Event for InfoManager deliveries
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/EventInfoManager.h"
#include "simx/type.h"
#include "simx/InfoManager.h"

using namespace std;

namespace simx {

using namespace std;

EventInfoManager::EventInfoManager(int fid, Time delay)
  : EventInfo(),
    fFileId( fid ),
    fDelay( delay )
{
}

EventInfoManager::EventInfoManager()
  : EventInfo(),
    fFileId(),
    fDelay()
{
}

void EventInfoManager::pack(PackedData& dp) const
{
    SMART_VERIFY( false ).msg("EventInfoManager shound never need to pack! (never needs to leave the LP)");
}

void EventInfoManager::unpack(PackedData& dp)
{
    SMART_VERIFY( false ).msg("EventInfoManager shound never need to unpack! (never needs to leave the LP)");
}

void EventInfoManager::execute(void)
{
    // call the entity manager's method
    theInfoManager().readDataFile( fFileId );
}

// GETTERS:
int EventInfoManager::getFileId() const 
{ 
    return fFileId; 
}

Time EventInfoManager::getDelay() const 
{ 
    return fDelay; 
}


void EventInfoManager::print(ostream& os) const
{
    os 	<< "EventInfoManager(" 
	<< fFileId << ","
	<< fDelay << ")";
}

} // namespace

