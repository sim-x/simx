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
// File:        PyCreateInputInfo.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     July 10 2012 
// Description: creates simx info from info data supplied by python
//
// @@
//
//--------------------------------------------------------------------------

#include <boost/python.hpp>
#include <boost/tuple/tuple.hpp>
#include "simx/Python/PyInfoData.h"
#include "simx/InfoManager.h"
#include "simx/InfoData.h"

using namespace boost;
using namespace std;

namespace simx{ 
  namespace Python  {

   

    void PyCreateInputInfo( PyInfoData& data ) {
      
      // if ( python::len(data.fEntityId) != 2 )
      // 	Logger::error() << 
      // 	  "PyCreateInputInfo: Invalid entity id from python" << endl;
      // else {
      // 	try {
	  //EntityID id = tuple<char,simxLong>(python::extract<char>(data.fEntityId[0]),
	  //					python::extract<simxLong>(data.fEntityId[1]));

	  //InfoData info_data(data.fTime, id,
	  //		     static_cast<ServiceAddress>(data.fServiceAddress), 
	  //		     data.fType, data.fProfileId );
	  //stringstream ss(data.fData);
	  //info_data.readCustomData(ss);
	  //theInfoManager().createInfoFromInfoData( info_data );
	  theInfoManager().createPyInfoFromInfoData( data );
	  
	  //}
	// catch(...) {
	//   PyErr_Print();
	//   PyErr_Clear();
	// }
    }
  }
}
  //}

void export_PyCreateInputInfo() {

  python::def("create_input_info",simx::Python::PyCreateInputInfo);
}
