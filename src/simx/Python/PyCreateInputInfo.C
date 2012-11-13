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
// @@COPYRIGHT@@
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
