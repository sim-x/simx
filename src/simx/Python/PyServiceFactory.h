//--------------------------------------------------------------------------
// File:    PyServiceFactory.h
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: August 1 2012
//
// Description: Factory methods for Python services
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef SIMX_PY_SERVICE_FACTORY_H
#define SIMX_PY_SERVICE_FACTORY_H

#include <boost/python.hpp>
#include "simx/ServiceFactory.h"
#include "simx/ServiceManager.h"
#include "simx/Service.h"
#include "simx/Entity.h"
#include "simx/Python/PyEntity.h"
#include "simx/Python/PyService.h"
#include "simx/Python/PyServiceInput.h"

using namespace simx::Python;

namespace simx {
  
  // template specialization of serviceCreator for Python services
  template<> 
  Service* serviceCreator<PyService, PyEntity, PyServiceInput>
  //boost::shared_ptr<Service>* serviceCreator<PyService, PyEntity, PyServiceInput>
  ( const ServiceName& name, Entity& owner, const Input& input,
    const Service::ClassType& type) {
    
    //std::cout << "service name is " << name << std::endl;
    //std::cout << "service type is " << type << std::endl;
    
    PyEntity* ow = dynamic_cast<PyEntity*>( &owner );
    if ( ! ow ) {
      Logger::error() << 
	"Python ServiceCreator: invalid Entity class passed to creation of PyService "
		      << " expected PyEntity " << ", received " 
		      << typeid(owner).name() << std::endl;
      Logger::failure( "invalid Entity class for PyService");
    }
    
    const PyServiceInput* in = dynamic_cast<const PyServiceInput*>(&input);
    if( !in )
      {
	Logger::error() << "Python ServiceCreator: invalid Input class passed to creation of "
			<< " PyService" << ", expected " << " PyServiceInput class, "
			<< ", received " << typeid(input).name() << std::endl;
	
	/// this is sort of convoluted here, but I don't know a way to
	/// directly invoke "new" on a python-class object.
	/// We need to return a raw pointer that does not go out of scope
	/// when the function returns, so this seems to be the only way to
	/// do it.
	return new PyService( * (boost::python::extract<PyService* >
			      ( theServiceManager().fPyObjMap[ type ]
				(boost::ref(name), ow->getPyObj(),
				 PyServiceInput() )))
			      );
  }
    return new PyService( * (boost::python::extract<PyService* >
			     ( theServiceManager().fPyObjMap[ type ]
			       (boost::ref(name), ow->getPyObj(),
				boost::ref(*in) )))
			  );
  }

} //namespace

#endif
