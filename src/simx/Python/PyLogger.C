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
// File:    PyLogger.C
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: Oct 9 2012
//
// Description: Wrappers around simx logger for 
//              exporting logging functionality to Python
// @@
//
//--------------------------------------------------------------------------


#include <boost/python.hpp>

#include "simx/logger.h"
#include "simx/Python/PyUtility.h"

//#include "simx/output.h"
//#include "simx/type.h"
//#include "simx/Python/PyEntity.h"
//#include "simx/Python/PyService.h"

using namespace boost;
using namespace std;
using namespace simx;


//TODO (python, high) : '\n',endl should come from python
namespace simx {

  namespace Python {

   
    
    void PyDebug1 (const python::str& msg ) {

      Logger::debug1() << extract_py_str( msg ) << endl;

    }
      
  
void PyDebug2 (const python::str& msg ) {

  Logger::debug2() << extract_py_str( msg ) << endl;

    }

void PyDebug3 (const python::str& msg ) {

  Logger::debug3() << extract_py_str( msg ) << endl;
    }

void PyDebugInfo (const python::str& msg ) {

  Logger::info() << extract_py_str( msg ) << endl;
    }

void PyWarn (const python::str& msg ) {

  Logger::warn() << extract_py_str( msg ) << endl;
    }

void PyError (const python::str& msg ) {

  Logger::error() << extract_py_str( msg ) << endl;
    }

void PyFailure (const python::str& msg ) {
  
  Logger::failure(extract_py_str( msg)+'\n');
}

   
    
    
  } // namespace Python

} // namespace simx

using namespace simx::Python;



void export_PyLogger() {

  python::def("debug1",&PyDebug1);
  python::def("debug2",&PyDebug2);
  python::def("debug3",&PyDebug3);
  python::def("debug_info",&PyDebugInfo);
  python::def("warn",&PyWarn);
  python::def("error",&PyError);
  python::def("failure",&PyFailure);
  

}

