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
// File:    PyServiceFactory.h
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: August 1 2012
//
// Description: Factory methods for Python services
//
// @@
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
