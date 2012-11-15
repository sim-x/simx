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
// File:    PyOutput.C
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: Oct 17 2012
//
// Description: Wrappers around simx output for 
//              exporting output functionality to Python
// @@
//
//--------------------------------------------------------------------------

#include <boost/python.hpp>

#include "simx/logger.h"
#include "simx/Python/PyUtility.h"

#include "simx/output.h"
#include "simx/type.h"
#include "simx/Python/PyEntity.h"
#include "simx/Python/PyService.h"

using namespace boost;
using namespace std;
using namespace simx;

namespace simx {

  namespace Python {
    
    void PyOutput( const PyEntity& ent, const OutputRecordType rec_type,
		   const python::str& msg) 
    {
      Output::output( ent, rec_type, Control::getSimPhase() )
	<< (extract_py_str(msg)+'\n'); 
    }
    
    void PyOutput( const PyService& serv, const OutputRecordType rec_type,
    		   const python::str& msg) 
    {
      Output::output( serv, rec_type, Control::getSimPhase() )
    	<< (extract_py_str(msg)+'\n'); 
    }
    
  } //namespace Python

} // namespace simx

using namespace simx::Python;

// syntax is BOOST_PYTHON_FUNCTION_OVERLOADS( unique-name,
// overloaded-name, min-args, max-args ) 
// Also see python::defs below
BOOST_PYTHON_FUNCTION_OVERLOADS(PyEntOutput,PyOutput,3,3);
BOOST_PYTHON_FUNCTION_OVERLOADS(PyServOutput,PyOutput,3,3);


void export_PyOutput() {
  python::def("output",static_cast< void(*)
	      (const PyEntity&, const OutputRecordType,
	       const python::str&)>
	      (&PyOutput), PyEntOutput() );
  python::def("output",static_cast< void(*)
	      (const PyService&, const OutputRecordType,
	       const python::str&)>
	      (&PyOutput), PyServOutput() );
}
