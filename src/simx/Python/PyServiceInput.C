//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        PyServiceInput.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Sep 25 2012 
// Description: Service input class for services in Python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Python/PyServiceInput.h"

using  namespace std;
using namespace simx::Python;
using namespace simx;
using namespace boost::python;

void export_PyServiceInput() {

  class_<PyServiceInput>("ServiceInput",init<>() )
    .def("get_profile",&PyServiceInput::getProfile,
	 return_value_policy<reference_existing_object>() )
    .def_readonly("data_",&PyServiceInput::fData)
    
    ;
}
