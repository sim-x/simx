//--------------------------------------------------------------------------
// File:    PyOutput.C
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: Oct 17 2012
//
// Description: Wrappers around simx output for 
//              exporting output functionality to Python
// @@COPYRIGHT@@
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
