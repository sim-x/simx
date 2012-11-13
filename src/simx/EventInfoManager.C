//--------------------------------------------------------------------------
// File:    EventInfoManager.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 12 2005
//
// Description: 
//	Event for InfoManager deliveries
//
// @@COPYRIGHT@@
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

