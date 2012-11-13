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

#include "simx/Python/PyServiceData.h"

using namespace std;
using namespace simx::Python;
using namespace simx;
using namespace boost::python;

void export_PyServiceData() {
class_<PyServiceData>("ServiceData",init<>() )
  .def(init<const ServiceName&, const Service::ClassType&,
       ProfileID,PyProfile&,object&>() )
    .def_readwrite("name_",&PyServiceData::fName)
    .def_readwrite("type_",&PyServiceData::fClassType)
    .def_readwrite("profile_id_",&PyServiceData::fProfileId)
  .def_readonly("profile_",&PyServiceData::fProfile)
    .def_readwrite("data_",&PyServiceData::fData)
  ;
}
