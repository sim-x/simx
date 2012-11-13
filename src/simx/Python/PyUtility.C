// $Id$
//--------------------------------------------------------------------------
//
// File:        PyUtility.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Oct 1 2012 
// Description: Utility functions for python module.
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Python/PyUtility.h"

using namespace simx;
using namespace boost;
using namespace std;

#include "simx/type.h"
#include "simx/logger.h"

namespace simx {

  namespace Python {

    const EntityID py2EntityId( const python::tuple& in )
    {
      try {
	char i = python::extract< char >( in[0] );
	int j =  python::extract<simx::simxLong >( in[1] );
	
	return EntityID(i, j);
      } catch(...)
	{
	  PyErr_Print();
	  PyErr_Clear();
	  return EntityID();
	}
    }
    
    const python::tuple EntityId2py( const EntityID& id ) {
      
      return python::make_tuple( id.get<0>(),id.get<1>() );
  
    }
    
    
    //    namespace simx {
    //namespace Python {
    string debug_msg;
    
    
    const string& extract_py_str( const python::str& msg ) {
      
      try {

	debug_msg = python::extract<string>( msg );
	return debug_msg;
      }

      catch (...) {
	
	Logger::error() << "simx::Python::PyLogger:: "
			<< " unable to extract python string" << endl;
	PyErr_Print();
	PyErr_Clear();
	debug_msg = "";
	return debug_msg;
      }
      
    }
  } //namespace Python
} //namespace simx


//}]
//}
