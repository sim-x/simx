// Copyright (c) 2012. Los Alamos National Security, LLC. 

// This material was produced under U.S. Government contract DE-AC52-06NA25396
// for Los Alamos National Laboratory (LANL), which is operated by Los Alamos 
// National Security, LLC for the U.S. Department of Energy. The U.S. Government 
// has rights to use, reproduce, and distribute this software.  

// NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, 
// EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  
// If software is modified to produce derivative works, such modified software should
// be clearly marked, so as not to confuse it with the version available from LANL.

// Additionally, this library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License v 2.1 as published by the 
// Free Software Foundation. Accordingly, this library is distributed in the hope that 
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See LICENSE.txt for more details.

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
// @@
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
