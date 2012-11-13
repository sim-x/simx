//--------------------------------------------------------------------------
// File:    PyLogger.C
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: Oct 9 2012
//
// Description: Wrappers around simx logger for 
//              exporting logging functionality to Python
// @@COPYRIGHT@@
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

