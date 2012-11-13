//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        PyManager.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Oct 16 2012 
// Description: Wrappers around the various object manager functions
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include <boost/python.hpp>
#include "simx/EntityManager.h"
#include "simx/Python/PyUtility.h"
#include "simx/Python/PyEntity.h"
#include "simx/type.h"

using namespace std;
using namespace boost;

namespace simx {

  namespace Python {

    python::object gNoneObject = python::api::object();

    LPID getPyEntityLpId( const python::tuple& ent_id) {

      return theEntityManager().findEntityLpId( py2EntityId( ent_id ) );
    }

    python::object& getEntity( const python::tuple& ent_id ) {
      
      shared_ptr<PyEntity> py_ent;
      if ( theEntityManager().getEntity( py2EntityId( ent_id ), py_ent ))
	return py_ent->getPyObj();
      else
	return gNoneObject;
    }

    // TODO (python, high). a better way to do this is to pass in 
    // the object types instead of char
    void probePyEntities( python::str& ent_type, python::object& cls_method, 
			  python::tuple& args ) {
      
      string type_str = python::extract<string>(ent_type);
      //      const char etype = type_str.c_str()
      string name = python::extract<string>
	( cls_method.attr("__name__") );
      

      tuple<string,python::tuple> t = tuple<string,python::tuple>
	(name,args);

       theEntityManager().probeEntities(&PyEntity::pyCallBack,
					t, *type_str.c_str() );
    }
    
  } //namespace Python

} //namespace simx


using namespace boost::python;

void export_PyManager() {

  python::def("get_entity_lp_id",&simx::Python::getPyEntityLpId);
  python::def("get_entity",&simx::Python::getEntity,
	      return_value_policy<reference_existing_object>() );
  python::def("probe_entities",&simx::Python::probePyEntities );
}
