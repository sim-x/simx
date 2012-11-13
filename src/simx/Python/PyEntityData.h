//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        pyEntityData.h
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     July 10 2012 
// Description: entity data structure for entities in python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef SIMX_PY_ENTITY_DATA
#define SIMX_PY_ENTITY_DATA

#include "simx/type.h"
#include <boost/python.hpp>
#include "simx/Entity.h"
#include "simx/Python/PyUtility.h"

namespace simx {
  
  namespace Python {
    
    struct PyEntityData {
      
      // TODO: can(should) we pass references to the constructor instead?
      PyEntityData( boost::python::tuple& ent_id, 
		    const Entity::ClassType& type,
		    ProfileID pr_id, 
		    PyProfile& profile,
		    boost::python::object& data):
		    //std::string data):
      fEntityId( py2EntityId(ent_id) ),
      fEntityType( type ), 
      fProfileId( pr_id ),
	fProfile( profile ),
      fData( data ) 
      {}
      
      PyEntityData() {}
      //boost::python::tuple fEntityId;
      EntityID fEntityId;
      Entity::ClassType      fEntityType;
      ProfileID              fProfileId;
      PyProfile              fProfile;
      boost::python::object        fData;
      //std::string fData;
    };
      
  } // namespace
} //namespace




#endif
