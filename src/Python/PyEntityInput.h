//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        PyEntityInput.h
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Sep 25 2012 
// Description: Entity input class for entities in Python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef SIMX_PY_ENTITY_INPUT
#define SIMX_PY_ENTITY_INPUT

#include <boost/python.hpp>

#include "simx/type.h"
#include "simx/Entity.h"

namespace simx {

  namespace Python {

    struct PyEntityInput : public EntityInput {

      virtual void readData( const boost::python::object& data)
      {
	//assert(false);
	fData = data;
      }

      virtual void readProfile( boost::shared_ptr<PyProfile>& );

      const PyProfile& getProfile() const {
	return *fProfile;
      }

      boost::python::object fData;
    private:
      void loadServices(boost::python::dict&);
      boost::shared_ptr<PyProfile> fProfile;

    };
  }
}

#endif
    
