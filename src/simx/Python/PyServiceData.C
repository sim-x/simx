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
// File:        PyServiceData.h
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Sep 26 2012
// Description: service data structure for services in python
//
// @@
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
