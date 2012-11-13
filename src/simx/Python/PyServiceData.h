//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        PyServiceData.h
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Sep 26 2012
// Description: service data structure for services in python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef SIMX_PY_SERVICE_DATA
#define SIMX_PY_SERVICE_DATA

#include "simx/type.h"
#include <boost/python.hpp>
#include "simx/Service.h"

namespace simx {
  
  namespace Python {
    
    struct PyServiceData {
      
      // TODO: can(should) we pass references to the constructor instead?
    PyServiceData(  const ServiceName& name,
		    const Service::ClassType& type,
		    ProfileID pr_id, 
		    PyProfile& profile,
		    boost::python::object& data):
      //std::string data):
      fName( name ),
	fClassType( type ), 
	fProfileId( pr_id ),
	fProfile(profile),
	fData( data ) 
      {}
      
      ~PyServiceData() {}
      PyServiceData() {}
      ServiceName            fName;
      Service::ClassType     fClassType;
      ProfileID              fProfileId;
      PyProfile              fProfile;
      boost::python::object fData;
      //std::string fData;
    };
      
  } // namespace
} //namespace




#endif
