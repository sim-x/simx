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
// File:        PyCreateEntity.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     July 10 2012 
// Description: creates simx entities from entity data supplied by python
//
// @@
//
//--------------------------------------------------------------------------

#include <boost/python.hpp>
#include <boost/tuple/tuple.hpp>
#include "simx/Python/PyEntityData.h"
#include "simx/EntityManager.h"
#include "simx/Python/PyUtility.h"

using namespace boost;
using namespace std;

namespace simx{ 
  namespace Python  {

    void PyCreateEntity( const PyEntityData& ent_data ) {
      
      theEntityManager().createPyEntity( ent_data );
      //if ( python::len(ent_data.fEntityId) != 2 )
      //Logger::error() << "PyCreateEntity: Invalid entity id from python" << endl;
      //else {
      //try {
	  //	  EntityID id = tuple<char,simxLong>(python::extract<char>(ent_data.fEntityId[0]),
	  //					python::extract<simxLong>(ent_data.fEntityId[1]));
      //  EntityID id = py2EntityId( ent_data.fEntityId );
	  //stringstream ss(ent_data.fData);
	  //stringstream ss 
	  //stringstream* ss = new stringstream();

	    //theEntityManager().createEntityPrivate(id, ent_data.fEntityType, ent_data.fProfileId,*ss);
	    // TODO (Python ) high: throw exception here if false is returned.
      //  theEntityManager().createPyEntity(id, ent_data.fEntityType, 
      //  ent_data.fProfileId, ent_data.fData);
	// }
      // 	catch(...) {
      // 	  PyErr_Print();
      // 	  PyErr_Clear();
      // 	}
      // }
    }
  }
}

void export_PyCreateEntity() {

  python::def("create_pyentity",simx::Python::PyCreateEntity);
}
