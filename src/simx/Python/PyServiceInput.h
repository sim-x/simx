//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        PyServiceInput.h
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Sep 26 2012 
// Description: Service input class for services in Python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef SIMX_PY_SERVICE_INPUT
#define SIMX_PY_SERVICE_INPUT

#include <boost/python.hpp>

#include "simx/type.h"
#include "simx/Service.h"

namespace simx {

  namespace Python {

    struct PyServiceInput : public ServiceInput {

      void readData( const boost::python::object& data)
      {
	//assert(false);
	fData = data;
      }

      void readProfile( boost::shared_ptr<PyProfile>& profile_ptr) {
	fProfile = profile_ptr;
      }
      
      const PyProfile& getProfile() const {
	return *fProfile;
      }

      boost::python::object fData;
    private:
      boost::shared_ptr<PyProfile> fProfile;
    };
  }
}

#endif
    
