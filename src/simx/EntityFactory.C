//--------------------------------------------------------------------------
// File:    EntityFactory.h
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: July 31 2012
//
// Description: 
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/EntityFactory.h"
#include "simx/Python/PyEntity.h"
#include "simx/Python/PyEntityInput.h"
#include "simx/LP.h"

using namespace std;
using namespace boost;

namespace simx { 


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
  
    
} // namespace
