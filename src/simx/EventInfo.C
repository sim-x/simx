// copyright (c) 2012. Los Alamos National Security, LLC. 

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
// File:    EventInfo.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Jan 14 2005
//
// Description: 
//	Event for Info deliveries
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/EventInfo.h"
#include "simx/Info.h"
#include "simx/InfoManager.h"
#include "simx/EntityManager.h"
#include "simx/logger.h"
#include "simx/LP.h"
#include "simx/constants.h"
#include "simx/Controller.h"

#include "Common/backtrace.h"

using namespace std;


namespace simx {


using namespace std;

EventInfo::EventInfo()
  : fDestEntity(),
    fDestService(),
    fDelay(),
    fTime(),
    fInfo()
{
}

EventInfo::~EventInfo()
{
}

void EventInfo::pack(PackedData& dp) const
{
    dp.add(fDestEntity);
    dp.add( static_cast<int>(fDestService) );
    dp.add(fDelay);
    dp.add(fTime);
    
    if( fInfo )
    {
	dp.add( fInfo->getInfoHandler().getClassType() );
	fInfo->pack( dp );
    } else
    {
	dp.add( Info::ClassType() );
	Logger::error() << "EventInfo: no Info to pack in " << *this << endl;
    }
}

void EventInfo::unpack(PackedData& dp)
{
  //Logger::debug3() << "simx::EventInfo unpack" << *this << endl;
    dp.get(fDestEntity);
    int tmpInt;

    dp.get(tmpInt);
    fDestService = static_cast<ServiceAddress>(tmpInt);
    dp.get(fDelay);
    dp.get(fTime);
    // get the Info
    Info::ClassType type;
    dp.get(type);
    if( type!=Info::ClassType() )
    {
	boost::shared_ptr<Info> info;
	theInfoManager().createInfo( type, info );
	info->unpack(dp);
	fInfo = info;
    } else
    {
	Logger::error() << "EventInfo: no Info to unpack in " << *this << endl;
    }
}

void EventInfo::execute(void)
{
    SMART_ASSERT( fInfo )( *this ).msg("EventInfo: received EventInfo with no Info");

    /// first look if there is something on CONTROLLER_INPUT:
    static const EntityManager& em = theEntityManager();

    // commenting out below, since there is no controller anymore in C++ land.- ST
    //em.getController().pollInput();

    // info goes to entity to deal with
    boost::shared_ptr<Entity> entity;
    if( !theEntityManager().getEntity( fDestEntity, entity ) )
    {
	Logger::error() << "EventInfo: cannot find entity " << fDestEntity 
	    << ", discarding EventInfo " << *this << endl;
	return;
    }
    
    // DEBUGGING!
    // check that the current time is the time that I am supposed to execute!
#ifdef DEBUG
    Logger::debug3() << "Executing ";
    print(Logger::debug3());
    Logger::debug3() << endl;
#endif
    Time currentTime = entity->getNow();
    // This doesn't work for types like uint64_t -- Do not calculate diff for these!
    //Time diff = abs(currentTime - fTime);

#if defined(PRIME_SSF_LTIME_FLOAT)
    Time diff = abs(currentTime - fTime);
    SMART_ASSERT(diff < LP::MINDELAY);
#elif defined(PRIME_SSF_LTIME_DOUBLE)
    Time diff = abs(currentTime - fTime);
    SMART_ASSERT(diff < LP::MINDELAY);
#elif defined(PRIME_SSF_LTIME_LONG)
    SMART_ASSERT(currentTime == fTime)( currentTime )( fTime );
#elif defined(PRIME_SSF_LTIME_LONGLONG)
    //SMART_ASSERT(diff == 0)(currentTime)(fTime);
    if ( currentTime > fTime )
      Logger::warn() << "EventInfo::execute(): Event arrived in the past "
		     << "EventInfo: " << *this << endl;
#else
#error Prime SSF time type not defined
#endif

    /// make sure that what you're giving to Entity::processIncomingInfo
    /// is the only Ptr to the object, and copy only if necessary    

    /// the const cast is not nice, but safe, because we know we're the only ones
    /// having a pointer to the object
    boost::shared_ptr<Info> noconstInfo( boost::const_pointer_cast<Info>( giveup_smart_ptr(fInfo) ) );
    SMART_ASSERT( noconstInfo );
    if( !noconstInfo.unique() )
    {
#ifdef DEBUG
	Logger::debug3() << "EventInfo: making copy of info " << noconstInfo << endl;
#endif
	theInfoManager().duplicateInfo( *noconstInfo, noconstInfo );
	SMART_ASSERT( noconstInfo );
    } else
    {
#ifdef DEBUG
	Logger::debug3() << "EventInfo: info is unique " << noconstInfo << endl;
#endif
    }
    SMART_ASSERT( noconstInfo.unique() )( noconstInfo );
    
    entity->processIncomingInfo( giveup_smart_ptr(noconstInfo), fDestService );
}


// GETTERS:
// TO:
const EntityID& EventInfo::getDestEntity() const 
{
    return fDestEntity;
}

const ServiceAddress& EventInfo::getDestService() const 
{
    return fDestService;
}
// WHEN:
Time EventInfo::getDelay() const 
{
    return fDelay;
}
Time EventInfo::getTime() const 
{
    return fTime;
}


// SETTERS:
// TO:
void EventInfo::setTo(const EntityID entId, const ServiceAddress servAddr)
{ 
    fDestEntity = entId; 
    fDestService = servAddr; 
}
// WHEN:
void EventInfo::setDelay(const Time delay)
{ 
    fDelay = delay; 
}
void EventInfo::setTime(const Time time) 
{
    fTime = time;
}
// WHAT:
void EventInfo::setInfo(const boost::shared_ptr<const Info> info)
{
    SMART_VERIFY( info ).msg("EventInfo: cannot set NULL info");
    fInfo = info; 
}


void EventInfo::print(ostream& os) const
{
    os << "EventInfo("
	<< "to(" << fDestEntity << "," << fDestService << "),"
	<< "delay(" << fDelay << "),"
   << "time(" << fTime << "),"
	<< "what(" << fInfo << ")"
	<< ")";
}

} // namespace
