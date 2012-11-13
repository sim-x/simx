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
// @@COPYRIGHT@@
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
