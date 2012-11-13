// $Id$
//--------------------------------------------------------------------------
//
// File:        PyUtility.h
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Oct 1 2012 
// Description: Utility functions for python module.
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef SIMX_PY_UTILITY_H
#define SIMX_PY_UTILITY_H

#include <boost/python.hpp>
#include "simx/type.h"

 namespace simx {

   namespace Python {

     const simx::EntityID 
       py2EntityId( const boost::python::tuple& in );

    // py2EntityId( const boost::python::tuple& in )
    // {
    //   try {
    // 	char i = boost::python::extract< char >( in[0] );
    // 	int j = boost::python::extract< int >( in[1] );
	
    // 	return simx::EntityID(i, j);
    //   } catch(...)
    // 	{
    // 	  PyErr_Print();
    // 	  PyErr_Clear();
    // 	  return simx::EntityID();
    // 	}
    // }

     const boost::python::tuple EntityId2py( const simx::EntityID& );


     //namespace simx {
     //namespace Python {
    
     const std::string& extract_py_str( const boost::python::str&  );
     
   } //namespace Python
 } //namespace simx

#endif
