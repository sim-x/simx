//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        PyEntityData.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     July 10 2012 
// Description: entity data structure for entities in python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Python/PyEntityData.h"

using namespace std;
using namespace simx::Python;
using namespace simx;
using namespace boost::python;

void export_PyEntityData() {
  
  class_<PyEntityData>("EntityData",init<>() )
    .def(init<tuple&,const Entity::ClassType&,
	 ProfileID,PyProfile&,object&>() )
    .def_readwrite("id_",&PyEntityData::fEntityId)
    .def_readwrite("type_",&PyEntityData::fEntityType)
    .def_readwrite("profile_id_",&PyEntityData::fProfileId)
    .def_readonly("profile_",&PyEntityData::fProfile)
    .def_readwrite("data_",&PyEntityData::fData)
  ;
}
