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
// File:    Service.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 15 2005
//
// Description:
//	Base class for services  capable of receiving
//	an Info event
//	Services live on Entities and do most of the work
// @@
//
//--------------------------------------------------------------------------


#include "simx/Service.h"

namespace simx {

Service::Service(const ServiceName& name, Entity& ent, const ServiceInput& input)
    :	fName( name ),
	fEntity( ent )
{
}

Service::~Service()
{
}

const ServiceName& Service::getName() const
{
    return fName;
}

Random::TRandom& Service::getRandom() const
{
    return fEntity.getRandom();
}

Time Service::getNow() const
{
    return fEntity.getNow();
}

EntityID Service::getEntityId() const
{
    return fEntity.getId();
}

const Entity& Service::getEntity() const
{
    return fEntity;
}

void Service::print(std::ostream& os) const
{
    os << "Service(" << fName << ")";
}



} // namespace

#include <boost/python.hpp>
using namespace boost;
using namespace boost::python;
void export_Service() {

  class_<simx::Service,noncopyable>("Service",no_init)
    // .def("get_entity",&simx::Service::getEntity,
    //	 return_value_policy<copy_const_reference>());
    .def("get_name",&simx::Service::getName,"Returns name string of Service object",
	 return_value_policy<copy_const_reference>() )
    .def("get_random",&simx::Service::getRandom,"Returns the random number object associated with this Service",
	 return_value_policy<reference_existing_object>() )
    .def("get_now",&simx::Service::getNow,"Returns current simulation time. Note that this is the simulation time local to the logical process on which this service lives.")
    ;
  //;
}
