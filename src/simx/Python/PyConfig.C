//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        pyConfig.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     July 5 2012 
// Description: Provides interface for setting configuration values from
//              Python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------


#include <boost/python.hpp>
#include "simx/Config/Configuration.h"

using namespace std;
using namespace boost;


//namespace simx {

//namespace Python {

int setConfigurationValue(const python::object& key_str, 
			  const python::object& val_str) {
      
  string key, val;
  try {
    key = python::extract<string>(key_str);
    val = python::extract<string>(val_str);
  }
  catch(...) {
    PyErr_Print();
    PyErr_Clear();
    return -1;
  }
      
  Config::gConfig.SetConfigurationValue(key,val);
  return 0;
}

int createProfile( const python::object& profile_name,
		   const python::object& profile_id,
		   const python::dict& profile_map ) {

  string p_name, p_id;
  map<string,string> cmap;
  try {
    p_name = python::extract<string>(profile_name);
    p_id = python::extract<string>(profile_id);
    python::list keys = profile_map.keys();

    for (int i = 0; i < python::len(keys); ++i) {
      cmap[python::extract<string>(keys[i])] = 
	python::extract<string>(profile_map[keys[i]]);
    }
  }
  catch(...) {
    PyErr_Print();
    PyErr_Clear();
    return -1;
  }
  Config::gConfig.createConfigurationSet( p_name, p_id, cmap );
  return 0;
}

// TODO (high): change name of export function
void export_SetConfigurationValue() {
      
  boost::python::def("set_config_value",setConfigurationValue);
  boost::python::def("create_profile",createProfile);
}



//}
//}
