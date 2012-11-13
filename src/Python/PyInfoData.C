//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        PyInfoData.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     July 10 2012 
// Description: info data structure for simx entities created from python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Python/PyInfoData.h"
#include "simx/Python/PyUtility.h"

using namespace std;
using namespace simx::Python;
using namespace simx;
using namespace boost::python;

namespace simx {

  namespace Python {
    
    PyInfoData::PyInfoData(  Time time, 
		 boost::python::tuple& ent_id, 
		 long address,
		 Info::ClassType type,
		 ProfileID pr_id,
			     PyProfile& profile,
			     boost::python::object& data):
			     //std::string& data) :

      // fTime( time ),
      // fEntityId( py2EntityId(ent_id) ),
      // fServiceAddress( address ),
      // fType( type ), 
      // fProfileId( pr_id ),
      // fData( data ) 

      InfoData( time, py2EntityId(ent_id), static_cast<ServiceAddress>( address ),
		type, pr_id ), fProfile( profile ), fData( data)

    {}
      
  }
}

void export_PyInfoData() {
  
  //  class_<PyInfoData>("InfoData",init<>() )
  class_<PyInfoData>("InfoData",init<Time,tuple&,long,Info::ClassType,
		     ProfileID,PyProfile&,object&>() )
    // .def_readwrite("time_",&PyInfoData::fTime)
    // .def_readwrite("id_",&PyInfoData::fEntityId)
    // .def_readwrite("address_",&PyInfoData::fServiceAddress)
    // .def_readwrite("type_",&PyInfoData::fType)
    // .def_readwrite("profile_id_",&PyInfoData::fProfileId)
    // .def_readwrite("data_",&PyInfoData::fData)
  ;
}
