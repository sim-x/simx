//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        PyEntityInput.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Sep 25 2012 
// Description: Entity input class for entities in Python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Python/PyEntityInput.h"

using  namespace std;
using namespace simx::Python;
using namespace simx;
using namespace boost;

namespace simx {

  namespace Python {
    
    void PyEntityInput::readProfile( boost::shared_ptr<PyProfile>& profile_ptr ) {
	fProfile = profile_ptr;
	python::object services = profile_ptr->get("SERVICES");
	//check for none
	if ( services == python::api::object())
	  {
	    Logger::warn() << "PyEntityInput: No SERVICES specification in profile" << endl;
	    
	  }
	else
	  {
	    python::dict serv_dict;
	    try {
	      serv_dict = python::extract<python::dict>(services);
	    }
	    catch (...) {
	      PyErr_Print();
	      PyErr_Clear();
	      Logger::error() << "PyEntityInput: SERVICES object is not a python dictionary" << endl;
	    }
	    loadServices(serv_dict);
	    
	  }
    }

    // TODO (Python) EntityInput::loadservices also has the ability to erase services
    // which isn't present here. Add it.
    void PyEntityInput::loadServices(python::dict& serv_dict) {
      python::list keys = serv_dict.keys();
      if ( python::len(keys) == 0 ) {
	Logger::warn() << "PyEntityInput: Empty service list" << endl;
	return;
      }
      for ( int i = 0; i < python::len(keys); ++i ) {
	try {
	  int sa = (python::extract<int>(keys[i]));
	  ServiceAddress s_addr = static_cast<ServiceAddress>(sa);
	  ServiceName  s_name = python::extract<ServiceName>( serv_dict[keys[i]] );
	  this->fServices.push_back(ServiceAssignment
				    (ServiceAddresses(1,s_addr),s_name));
	  
	}
	catch (...)
	  {
	    PyErr_Print();
	    PyErr_Clear();
	    Logger::error() << "PyEntityInput: Error while extracting service list" << endl;
	  }
      }
    }

  }
}
using namespace boost::python;
void export_PyEntityInput() {

  class_<PyEntityInput>("EntityInput",init<>() )
    .def("get_profile",&PyEntityInput::getProfile,
	 return_value_policy<reference_existing_object>() )
    .def_readonly("data_",&PyEntityInput::fData)
    ;
}
