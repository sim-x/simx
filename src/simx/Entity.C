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
// File:    Entity.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 7 2005
//
// Description:
//	Base class for everything uniquely identifiable (has globally
//	unique ID)
//	Entitites have services, which do most of the work
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/Entity.h"

#include "simx/LP.h"
#include "simx/ServiceManager.h"
#include "simx/EventInfo.h"
#include "simx/readers.h"
#include "simx/writers.h"
#include "simx/constants.h"
#include "simx/logger.h"
#include "simx/ControlInfoWrapper.h"
#include <boost/python.hpp>

using namespace std;
using namespace boost;

namespace simx {

//====================================================================
// EntityInput

void EntityInput::readProfile(Input::ProfileSource& p)
{
   // first let parent set-up
   Input::readProfile(p);

   // then set-up yourself
   // set ServiceMap
   string result;
   if( p.get("SERVICES", result) )
   {
      loadServices( *this, result );
   } else
   {
      // no services specified
      Logger::debug3() << "EntityInput: no SERVICES specification in a profile" << std::endl;
   }
}

void EntityInput::loadServices( EntityInput& input, string result )
{
    // TODO: LK: why is the service assignment so complicated? Why is it not only Address=Name?

      stringstream sstr(result);
      string setting;
      while( sstr >> setting )
      {
         int pos = setting.find('=');
         if( pos <= 0 )
         {
            // erase a possible service when no '=' is used
            ServiceAddress addr = boost::lexical_cast<ServiceAddress>(setting);
            Logger::debug3() << "EntityInput: erasing service " 
                             << addr << std::endl;            
            for( Services::iterator itr = input.fServices.begin();  
                 itr != input.fServices.end();  ++itr ) {
               bool hasAddr = false;
               for( ServiceAddresses::iterator addrItr = itr->first.begin();  
                    addrItr != itr->first.end();  ++addrItr ) {
                  if(*addrItr == addr) {
                     Logger::debug3() << "EntityInput: erasing ServiceAddress" 
                                      << std::endl;                        
                     hasAddr = true;
                     itr->first.erase(addrItr);
                     break;
                  }
               }   
               if(hasAddr) {
                  if(itr->first.size() == 0) {
                     // erase the whole thing 
                     Logger::debug3() << "EntityInput: erasing ServiceAssignment" 
                                      << std::endl;                       
                     input.fServices.erase(itr);  
                  }   
                  break;
               }
            } 
         } else
         {
            Logger::debug3() << "EntityInput: adding service" 
                             << std::endl;             
            // add serviceaddr-servicename setting
            string addressStr = setting.substr(0,pos);
            ServiceName id = boost::lexical_cast<ServiceName>( 
               setting.substr(pos+1,setting.size())
            );
            Logger::debug3() << "EntityInput: id: " << id << std::endl; 
                        
            ServiceAddresses addresses;
            pos = addressStr.find(',');
            while(pos > 0) {
               ServiceAddress address = boost::lexical_cast<ServiceAddress>(
                  addressStr.substr(0, pos));
               addresses.push_back(address);
               addressStr = addressStr.substr(pos+1, addressStr.size()); 
               pos = addressStr.find(',');  
            }
            // add last address
            ServiceAddress address = boost::lexical_cast<ServiceAddress>(
                  addressStr);
            addresses.push_back(address);
                              
            Logger::debug3() << "EntityInput: addresses: " << std::endl;                  
            for( ServiceAddresses::const_iterator itr = addresses.begin();
                 itr != addresses.end();  ++itr ) {
               Logger::debug3() << "   " << *itr << std::endl;
            }
            
            input.fServices.push_back(ServiceAssignment(addresses, id));
         }
      }

}

void EntityInput::print(std::ostream& os) const
{
    os 	<< "EntityInput("
	<< "Services=";
   for( Services::const_iterator itr = fServices.begin();
        itr != fServices.end(); ++itr ) {
      os << "[addresses=";
      ServiceAddresses addresses = itr->first;
      for( ServiceAddresses::const_iterator addrItr = addresses.begin();
           addrItr != addresses.end();  ++addrItr ) {
         os << *addrItr << ",";      
      }
      os << "id=" << itr->second << "]";
   } 
	os << ")";
}

//====================================================================
// Entity

  Entity::Entity(const EntityID& id, LP& lp, const EntityInput& input)
    : 	fId( id ),
	fLP( lp )
{

    //Initializee the weight to default 1

    fWeight = 1;
    Logger::debug2() << "Entity " << id << ": is being created on LP=" << fLP.getId() << endl;

}

Entity::~Entity()
{
}

EntityID Entity::getId() const
{
    return fId;
}


Weight Entity::getWeight(void) const
{
    return fWeight;
}


Random::TRandom& Entity::getRandom() const
{
    return fLP.getRandom();
}

Time Entity::getNow() const
{
    if(Control::getSimPhase() != Control::kPhaseRun) {
        Logger::failure("Entity::getNow(): simulation not running!");    
    }
    return fLP.getNow();
}

void Entity::processOutgoingInfo(const boost::shared_ptr<const Info> info, const Time& delay, const EntityID& dest, const ServiceAddress& serv) const
{
    if(Control::getSimPhase() != Control::kPhaseRun) {
        Logger::failure("Entity::processOutgoingInfo(): simulation not running!");    
    }
        
    EventInfo event;
    event.setTo(dest, serv);
    event.setDelay(delay);
    event.setInfo(info);

    // send it off
    fLP.sendEventInfo( event );
}

void Entity::processOutgoingControlInfo(const boost::shared_ptr<const Info> info, 
					const Time& delay, const EntityID& dest, const ServiceAddress& serv) const
{
  
    ControlInfoWrapper cinfo;
    cinfo.setTo(dest, serv);
    cinfo.setDelay(delay);
    cinfo.setInfo(info);

    // send it off
    fLP.sendControlInfo( cinfo );
}



void Entity::processIncomingInfo(boost::shared_ptr<Info> info, const ServiceAddress& address) const
{
    Logger::debug2() << "Entity " << fId << ": received Info " << info 
	<< " to address " << address << endl;

    SMART_ASSERT( info )( fId )( address ).msg("received NULL Info");
    SMART_ASSERT( info.unique() )( fId )( address )( info ).msg("expected unique info Ptr");

    // try to give it to a particular service
    boost::shared_ptr<Service> service;
    if( !getService( address, service ) )
    {
        Logger::warn() << "Entity " << fId << ": received Info for nonexistent service " 
    	    << address << endl;
    } else
    {
	const InfoHandler& infoHandler = info->getInfoHandler();
	/// gives up the pointer, so that the service's arg will be the only Ptr to it
	infoHandler.execute( service, giveup_smart_ptr(info) );
    }
}

void Entity::processIncomingControlInfo(boost::shared_ptr<Info> info, const ServiceAddress& address) const
{
    Logger::debug2() << "Entity " << fId << ": received Control Info " << info 
	<< " to address " << address << endl;

    SMART_ASSERT( info )( fId )( address ).msg("received NULL Info");
    SMART_ASSERT( info.unique() )( fId )( address )( info ).msg("expected unique info Ptr");

    // try to give it to a particular service
    boost::shared_ptr<Service> service;
    if( !getService( address, service ) )
    {
        Logger::warn() << "Entity " << fId << ": received Info for nonexistent service " 
    	    << address << endl;
    } else
    {
	const InfoHandler& infoHandler = info->getInfoHandler();
	/// gives up the pointer, so that the service's arg will be the only Ptr to it
	infoHandler.executeControl( service, giveup_smart_ptr(info) );
    }
}


void Entity::setService(ServiceAddress addr, boost::shared_ptr<Service> srv)
{
    if( srv )
    {
	fServices[addr] = srv;
    } else
    {
	ServiceMap::size_type erased = fServices.erase( addr );
	if( erased == 0 )
	{
	    Logger::warn() << "Entity " << fId << ": trying to erase a service that does not exist: "
		<< addr << endl;
	}
    }
}

void Entity::getServiceAddresses(std::list<ServiceAddress>& addresses) const
{
    for(ServiceMap::const_iterator iter = fServices.begin();
        iter != fServices.end();
        ++iter)
    {
	SMART_ASSERT( iter->second )( fId ).msg("Invalid entry in ServiceMap");
	addresses.push_back( iter->first );
    }
}


void Entity::print(ostream& os) const
{
    os 	<< "Entity("
	<< "Id=" << fId
	<< "LPid=" << fLP.getId()
	<< "Services=" << fServices
	<< ")";
}

//======================================================================================

void Entity::createServices(Entity& ent, const EntityInput::Services& services)
{

    // create the services
    for( EntityInput::Services::const_iterator itr = services.begin();
         itr != services.end();  ++itr ) {
      ServiceName id = itr->second;

      // create service
      boost::shared_ptr<Service> service = theServiceManager().createService(id, ent);

      if( !service )
      {
          // service not known - error message printer in ServiceManager
          continue;
      }
      
      for( EntityInput::ServiceAddresses:: const_iterator addrItr = itr->first.begin();
           addrItr != itr->first.end();  ++addrItr ) {
         ServiceAddress address = *addrItr;
         SMART_ASSERT( address );	// it must not be 0, that is reserved

         /// check if we're not overwriting something
         boost::shared_ptr<Service> serv;
         if( ent.getService( address, serv ) ) {
            Logger::warn() << "createServices: overwriting ServiceAddress " 
                           << address << " on Entity " << ent.getId() << endl;
         }
         ent.setService( address, service );
      }
   }
}



} // namespace


void export_EntityInput() {

  python::class_<simx::EntityInput>("EntityInput",python::init<>() )
    ;

}

void export_Entity() {

  python::class_<simx::Entity,boost::noncopyable>("Entity",python::no_init)
    .def("getNow",&simx::Entity::getNow)
    ;
}
