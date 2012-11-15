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

// $Id$
//--------------------------------------------------------------------------
//
// File:        PyUtility.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Oct 1 2012 
// Description: Utility functions for python module.
//
// @@
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
