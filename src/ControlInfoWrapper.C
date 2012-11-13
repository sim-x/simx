//--------------------------------------------------------------------------
// File:    ControlInfoWrapper.C
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: Oct 09 2008
//
// Description: 
//	Wrapper class around an Info that is sent out-of-band. Will hold
//      information such as time sent, destination entity service etc.
//      //TODO: mechanism to delay acting on this info if the simulation clock
//      //TODO: on receiving LP is out of sync with message time.    
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/ControlInfoWrapper.h"
#include "simx/Info.h"
#include "simx/type.h"
#include "simx/InfoManager.h"
#include "simx/EntityManager.h"
#include "simx/logger.h"
#include "simx/LP.h"
#include "simx/constants.h"

#include "Common/backtrace.h"

using namespace std;

namespace simx {


using namespace std;

ControlInfoWrapper::ControlInfoWrapper()
{
}

void ControlInfoWrapper::setSentTime(const Time time) {
   fSentTime = time;
}

Time ControlInfoWrapper::getSentTime() const {
   return fSentTime;
}

void ControlInfoWrapper::pack(PackedData& dp) const
{
  dp.add(fSrcLP);
  dp.add(fDestLP);
    dp.add(fDestEntity);
    dp.add( static_cast<int>(fDestService) );
    dp.add(fSentTime);
    dp.add(fDelay);
   
    
    if( fInfo )
    {
	dp.add( fInfo->getInfoHandler().getClassType() );
	fInfo->pack( dp );
    } else
    {
	dp.add( Info::ClassType() );
	Logger::error() << "ControlInfoWrapper: no Info to pack in " << *this << endl;
    }
}

void ControlInfoWrapper::unpack(PackedData& dp)
{
  //Logger::debug3() << "simx::ControlInfoWrapper unpack" << *this << endl;
  dp.get(fSrcLP);
  dp.get(fDestLP);
    dp.get(fDestEntity);
    int tmpInt;
    dp.get(tmpInt);
    fDestService = static_cast<ServiceAddress>(tmpInt);
    dp.get(fSentTime);
    dp.get(fDelay);


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
	Logger::error() << "ControlInfoWrapper: no Info to unpack in " << *this << endl;
    }
}

//void ControlInfoWrapper::execute(LP& lp)
void ControlInfoWrapper::execute()

{
  //SMART_ASSERT( fInfo )( *this )( lp.getId() ).msg("ControlInfoWrapper: received ControlInfoWrapper with no Info");
  SMART_ASSERT( fInfo )( *this )( fDestLP ).msg("ControlInfoWrapper: received ControlInfoWrapper with no Info");

    // info goes to entity to deal with
    boost::shared_ptr<Entity> entity;
    if( !theEntityManager().getEntity( fDestEntity, entity ) )
    {
	Logger::error() << "ControlInfoWrapper: cannot find entity " << fDestEntity 
	    << ", discarding ControlInfoWrapper " << *this << endl;
	return;
    }
    
    //TODO: Add warning here if out-of-band message sent-time + delay is
    //TODO: unsynchronized with this LP. Of course, the user should be aware
    //TODO: that out-of-band messaging is inherently risky.

    /// make sure that what you're giving to Entity::processIncomingInfo
    /// is the only Ptr to the object, and copy only if necessary    
    if( !fInfo.unique() )
    {
	Logger::debug3() << "ControlInfoWrapper: making copy of info " << fInfo << endl;
	theInfoManager().duplicateInfo( *fInfo, fInfo );
    } else
    {
	Logger::debug3() << "ControlInfoWrapper: info is unique " << fInfo << endl;
    }
    /// the const cast is not nice, but safe, because we know we're the only ones
    /// having a pointer to the object
    boost::shared_ptr<Info> noconstInfo( boost::const_pointer_cast<Info>( giveup_smart_ptr(fInfo) ) );

    SMART_ASSERT( noconstInfo );
    SMART_ASSERT( noconstInfo.unique() )( noconstInfo );
    
    entity->processIncomingControlInfo( giveup_smart_ptr( noconstInfo ), fDestService );
}


// GETTERS:
// TO:
const EntityID& ControlInfoWrapper::getDestEntity() const 
{
    return fDestEntity;
}

const ServiceAddress& ControlInfoWrapper::getDestService() const 
{
    return fDestService;
}
// WHEN:
const Time& ControlInfoWrapper::getDelay() const 
{
    return fDelay;
}

LPID ControlInfoWrapper::getSrcLP() const
{
  return fSrcLP;
}

LPID ControlInfoWrapper::getDestLP() const
{
  return fDestLP;
}

// SETTERS:
// TO:
void ControlInfoWrapper::setTo(const EntityID entId, const ServiceAddress servAddr)
{ 
    fDestEntity = entId; 
    fDestService = servAddr; 
}

void ControlInfoWrapper::setSrcLP( const LPID lpId )
{
  fSrcLP = lpId;
}

void ControlInfoWrapper::setDestLP( const LPID lpId )
{
  fDestLP = lpId;
}

// WHEN:
void ControlInfoWrapper::setDelay(const Time delay)
{ 
    fDelay = delay; 
}
// WHAT:
void ControlInfoWrapper::setInfo(const boost::shared_ptr<const Info> info)
{
    SMART_VERIFY( info ).msg("ControlInfoWrapper: cannot set NULL info");
    fInfo = info; 
}


void ControlInfoWrapper::print(ostream& os) const
{
    os << "ControlInfoWrapper("
	<< "to(" << fDestEntity << "," << fDestService << "),"
	<< "delay(" << fDelay << "),"
   << "Sent time(" << fSentTime << "),"
	<< "what(" << fInfo << ")"
	<< ")";
}

} // namespace
