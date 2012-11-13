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
// @@COPYRIGHT@@
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
    .def("get_name",&simx::Service::getName,
	 return_value_policy<copy_const_reference>() )
    .def("get_random",&simx::Service::getRandom,
	 return_value_policy<reference_existing_object>() )
    .def("get_now",&simx::Service::getNow)
    ;
  //;
}
