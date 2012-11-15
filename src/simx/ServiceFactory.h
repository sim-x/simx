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
// File:    ServiceFactory.h
// Module:  simx
// Author:  Lukas Kroc
// Created: November 29 2004
//
// Description: 
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_SERVICEFACTORY
#define NISAC_SIMX_SERVICEFACTORY

#include "simx/Service.h"
#include "simx/logger.h"

#include "loki/Factory.h"

namespace simx {

class Entity;
class Input;
class ServiceData;

/// Handler for unknown Index identifier
template <typename IdentifierType, class AbstractProduct>
struct UnknownIndexService
{
    struct Exception : public std::exception
    {
	public:
    	    Exception(std::string d) { fMsg = std::string("Unknown service: <") + d + ">"; }
	    ~Exception() throw() {}
	private:
	    std::string fMsg;
    };
        
    static AbstractProduct* OnUnknownType(std::string d)
    {
	throw Exception(d);
    }
};

/// Generic service-creation function
/// ServiceClass - particular service to be created
/// EntityClass- what entity is expected by constructor
/// InputClass - what input is expected by constructor
/// type - This is only used for Python services, for determining the Python object at run time.

template<class ServiceClass, class EntityClass, class InputClass>
//boost::shared_ptr<Service>* serviceCreator(const ServiceName& name, Entity& owner, const Input& input, 
Service* serviceCreator(const ServiceName& name, Entity& owner, const Input& input, 
			const Service::ClassType& type)
			
{
  
    EntityClass* ow = dynamic_cast<EntityClass*>(&owner);
    if( !ow )
    {
	Logger::error() << "ServiceCreator: invalid EntityClass passed to creation of "
	    << typeid(ServiceClass).name() << ", expected " << typeid(EntityClass).name()
	    << ", received " << typeid(owner).name() << std::endl;
	Logger::failure( "invalid EntityClass for Service");
    }
    
    const InputClass* in = dynamic_cast<const InputClass*>(&input);
    if( !in )
    {
	Logger::error() << "ServiceCreator: invalid InputClass passed to creation of "
	    << typeid(ServiceClass).name() << ", expected " << typeid(InputClass).name()
	    << ", received " << typeid(input).name() << std::endl;
	return new ServiceClass( name, *ow, InputClass());
    }
    
     return new ServiceClass( name, *ow, *in );
    //return &boost::shared_ptr<ServiceClass>( new ServiceClass( name, *ow, *in ) );
}

/// A ServiceFactory is a Loki Factory - owned by ServiceManager object (singleton)
///
/// This factory stores a map of service identifiers to service creation
/// functions.  The identifier is a service class string
typedef Loki::Factory<	//Service,
  //boost::shared_ptr<Service>,
  Service,
                        Service::ClassType,
		        // this is the creator's signature:
                        Service* (*)(const ServiceName&, Entity&, const Input&, const Service::ClassType&),
  //boost::shared_ptr<Service>* (*)(const ServiceName&, Entity&, const Input&, const Service::ClassType&),
                        UnknownIndexService>
	ServiceFactory;

} // namespace

#endif

