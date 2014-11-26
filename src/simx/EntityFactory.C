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
// File:    EntityFactory.h
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: July 31 2012
//
// Description: 
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/EntityFactory.h"
#include "simx/Python/PyEntity.h"
#include "simx/Python/PyEntityInput.h"
#include "simx/LP.h"

using namespace std;
using namespace boost;

namespace simx { 

using boost::shared_ptr;

  void PyEntityCreator::registerObject( const Entity::ClassType& name, 
					const python::object& py_entity_class ) {
    
    if (! fPyObjMap.insert( std::make_pair( name, py_entity_class)).second) {
      std::cerr << "ERROR: EntityFactor: Python entity class '" << name 
		<< "' is already registered" << std::endl; 
      throw Common::Exception("Error in Python entity class registration"); 
    }
  }


  // TODO (python, high): move this to PyEntityFactory
  shared_ptr<Entity> PyEntityCreator::create( const EntityID& id, LP& lp, const Input& input, 
						    const Entity::ClassType& type ) const {
    
    //const EntityInput* in = dynamic_cast<const EntityInput*>(&input);
    const Python::PyEntityInput* in = dynamic_cast<const Python::PyEntityInput*>(&input);



    PyEntityClassMap::const_iterator iter = fPyObjMap.find( type );
    if ( iter == fPyObjMap.end() ) {
      
      Logger::error() << "Cannot find Python object associated with type " << type << endl;
      throw ExceptionInvalidEntityInput();
    }
    
    SMART_ASSERT( iter->second );
    
    const python::object& py_class = iter->second;
    shared_ptr<Python::PyEntity> e = python::extract<shared_ptr<Python::PyEntity> >
      (py_class(boost::ref(id),boost::ref(lp),boost::ref(in)));
    return e;
  }
  
  
   shared_ptr<Entity> PyEntityCreator::create( const EntityID& id, LP& lp, const Input& input, 
					       const python::object& py_class ) const {
    
    //const EntityInput* in = dynamic_cast<const EntityInput*>(&input);
    const Python::PyEntityInput* in = dynamic_cast<const Python::PyEntityInput*>(&input);
    shared_ptr<Python::PyEntity> e = python::extract<shared_ptr<Python::PyEntity> >
      (py_class(boost::ref(id),boost::ref(lp),boost::ref(in)));
    return e;
  }

    
} // namespace
