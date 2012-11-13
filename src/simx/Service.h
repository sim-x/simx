//--------------------------------------------------------------------------
// File:    Service.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Jan 16 2005
//
// Description:
//	Base class for services  capable of receiving
//	an Info event
//	Services live on Entities and do most of the work
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_SERVICE
#define NISAC_SIMX_SERVICE

#include "simx/Entity.h"
#include "simx/InfoRecipient.h"
#include "simx/Input.h"

#include <iosfwd>

namespace simx {

class Info;

/// Service general input
struct ServiceInput : virtual public Input
{
    /// Profile entries
    /// CustomData entries
};

/// Base class for services capable of receiving an Info event
/// Services live on Entities and do most of the work
class Service
{
    public:
	typedef std::string ClassType;	///< which type identifies descendants

	Service(const ServiceName&, Entity&, const ServiceInput&);
	virtual ~Service() = 0;

	/// returns service name
	const ServiceName& getName() const;

	/// returns associated random stream (calls Entity::getRandom)
	Random::TRandom& getRandom() const;

	/// returns current simulation time (calls Entity::getNow)
	Time getNow() const;

	/// sends an Info
	/// By default, INVALIDATES (reset()) the pointer that it is given!
	/// does NOT copy the Info object, only the pointer
	/// the Info should not be altered after calling this 
	/// (even if another pointer to is exists so that we could)
	///
	/// \param Info to send (WILL BE INVALIDATED by default) must not be NULL
	/// \param Delay after which the Info is delivered (should be >= MINDELAY)
	/// \param Entity to deliver it to
	/// \param Service to deliver it to
	/// \param whether or not to invalidate the pointer (warns if the pointer is not unique and is true) (default true)
	template<typename InfoClass>
	void sendInfo(boost::shared_ptr<InfoClass>&, const Time&, const EntityID&, const ServiceAddress&, const bool = true) const;


	/// send an into do destination BUT BLOCKS ANS WAITS FOR RESPONSE
	
	template<typename InfoClass>
	void sendReceiveInfo(boost::shared_ptr<InfoClass>&, const EntityID&, const ServiceAddress&);
	
	/// send out-of-band info
	template<typename InfoClass>
	void sendControlInfo(boost::shared_ptr<InfoClass>&, const Time&, 
			     const EntityID&, const ServiceAddress&, const bool = true) const;

	
	/// returns entityID where this service lives
	EntityID getEntityId() const;


	/// returns handler to the entity where this service lives:
	const Entity& getEntity() const;

	/// debug printing
	virtual void print(std::ostream&) const;

    private:
	//const ServiceName&	fName;		///< not necessary, but handy for output (reference because it might be a large type)
	const ServiceName fName; 
	/* changed from reference to object; ServiceName& fName refers to
	   a string inside fServiceInfoMap of ServiceManager; need servicename during service
	   destruction to print out stats.
	   for some reason, ServiceManager gets destroyed before services get destroyed, invalidating this reference. 
	   ( why? this seems to be contrary to the destruction-order-is-reverse-of-construction-order rule)
	   Perhaps something to do with the loki Singleton implementation */

	const Entity&		fEntity;	///< not necessary, but handy for output

	// should not be needed, unimplemented
	//Service(const Service&);
	Service& operator=(const Service&);
};

//==============================================================================

template<typename InfoClass>
void Service::sendInfo(boost::shared_ptr<InfoClass>& info, const Time& delay, const EntityID& dest, const ServiceAddress& serv, const bool invalidate) const
{
    /// see that we have valid input
    if( !info )
    {
	Logger::error() << "Service (" << getEntityId() << "," << fName
	<< "): cannot send NULL info" << std::endl;
	return;
    }
    SMART_ASSERT( info );
    
    /// warn about non-uniqueness of the Info pointer, if not disabled
    if( invalidate && !info.unique() )
    {
	Logger::warn() << "Service (" << getEntityId() << "," << fName 
	    << "): sending an Info that is not unique " << info << std::endl;
    }
    Logger::debug3() << "Service (" << getEntityId() << "," << fName
	<< "): sending Info " << info << std::endl;

    // send it off, and invalidate if not disabled
    if( invalidate )
    {
	fEntity.processOutgoingInfo(giveup_smart_ptr(info), delay, dest, serv);
    } else
    {
	fEntity.processOutgoingInfo(info, delay, dest, serv);
    }
}

//==============================================================================
// out-of-band info
//==============================================================================
template<typename InfoClass>
void Service::sendControlInfo(boost::shared_ptr<InfoClass>& info, const Time& delay, 
			      const EntityID& dest, const ServiceAddress& serv, const bool invalidate) const
{
    /// see that we have valid input
    if( !info )
    {
	Logger::error() << "Service (" << getEntityId() << "," << fName
	<< "): cannot send NULL info" << std::endl;
	return;
    }
    SMART_ASSERT( info );
    
    /// warn about non-uniqueness of the Info pointer, if not disabled
    if( invalidate && !info.unique() )
    {
	Logger::warn() << "Service (" << getEntityId() << "," << fName 
	    << "): sending an Info that is not unique " << info << std::endl;
    }
    Logger::debug3() << "Service (" << getEntityId() << "," << fName
	<< "): sending Control Info " << info << std::endl;

    // send it off, and invalidate if not disabled
    if( invalidate )
    {
	fEntity.processOutgoingControlInfo(giveup_smart_ptr(info), delay, dest, serv);
    } else
    {
	fEntity.processOutgoingControlInfo(info, delay, dest, serv);
    }
}



//==============================================================================
// miscelanious functions that are not members of Service

/// sends an Info to multiple Entities
/// by default, INVALIDATES (reset()) the info pointer that it is given!
/// does NOT copy the Info object, only the pointer
/// - returns number of infos sent (possibly 0)
///
/// \param Service that send the info
/// \param Info to send (WILL BE INVALIDATED by default) must not be NULL
/// \param Delay after which the Info is delivered (should be >= MINDELAY)
/// \param Entities to deliver it to (STL container with .begin() .end() methods)
/// \param Service to deliver it to
/// \param whether or not to invalidate the pointer (warns if the pointer is not unique and is true) (default true)
template<typename InfoClass, typename Container> 
int multiSendInfo(
    const Service& service,
    boost::shared_ptr<InfoClass>& info, 
    const Time& delay,
    const Container& neighbors,
    const ServiceAddress& address,
    const bool invalidate = true)
{
    /// make sure we have valid input
    if( !info )
    {
	Logger::error() << "Service (" << service.getEntityId() << "," << service.getName()
	<< "): cannot multiSend NULL info" << std::endl;
	return 0;
    }
    SMART_ASSERT( info );

    /// warn about non-uniqueness of the Info pointer, if not disabled
    if( invalidate && !info.unique() )
    {
	Logger::warn() << "Service (" << service.getEntityId() << "," << service.getName()
	    << "): multisending an Info that is not unique " << info << std::endl;
    }

    Logger::debug3() << "Service (" << service.getEntityId() << "," << service.getName() 
	<< ": multisending Info " << info << " to neighbors " << neighbors << std::endl;
    
    /// send the info to each Entity listed in the neighbors input
    int eventsSent = 0;	///< how many copies of the Info we have sent so far
    for(typename Container::const_iterator it = neighbors.begin(); 
	it != neighbors.end(); 
	++it)
    {
	const EntityID& destEntId = *it;
	
	service.sendInfo( info, delay, destEntId, address, false );	///< we do NOT want to invalidate
	eventsSent++;
    } // for neighbors

    /// do how forget to invalidate the given info pointer, if needed
    if( invalidate )
    {
	info.reset();
    }
    
    return eventsSent;
}


//==============================================================================


inline std::ostream& operator<<(std::ostream& os, const Service& p)
{
    p.print(os);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Service* const p)
{
    if(p) { os << p->getName(); }
    else  { os << "NULL"; }
    return os;
}

} // namespace
#endif 

