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
// $Id$
//--------------------------------------------------------------------------
//
// File:        PyManager.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Oct 16 2012 
// Description: Wrappers around the various object manager functions
//
// @@
//
//--------------------------------------------------------------------------

#include <boost/python.hpp>
#include "simx/EntityManager.h"
#include "simx/InfoManager.h"
#include "simx/Python/PyUtility.h"
#include "simx/Python/PyEntity.h"
#include "simx/type.h"

using namespace std;
using namespace boost;

namespace simx {

  namespace Python {

    //python::object gNoneObject = python::api::object();

    LPID getPyEntityLpId( const python::tuple& ent_id) {

      return theEntityManager().findEntityLpId( py2EntityId( ent_id ) );
    }

    //python::object& getEntity( const python::tuple& ent_id ) {
    python::object getEntity( const python::tuple& ent_id ) {
      
      shared_ptr<PyEntity> py_ent;
      if ( theEntityManager().getEntity( py2EntityId( ent_id ), py_ent ))
	return py_ent->getPyObj();
      else
	//return gNoneObject;
	return python::object();
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
    

    void setPyEventScheduler( python::object event_scheduler) {
      
      theInfoManager().setPyEventScheduler( event_scheduler );

    }
      

    void setPyEventSchedulerTimer(Time time) {
      theInfoManager().setPyEventSchedulerTimer(time);
    }


  } //namespace Python

} //namespace simx


using namespace boost::python;

void export_PyManager() {

  python::def("get_entity_lp_id",&simx::Python::getPyEntityLpId);
  python::def("get_entity",&simx::Python::getEntity);
  //	      return_value_policy<reference_existing_object>() );
  python::def("probe_entities",&simx::Python::probePyEntities );
  python::def("set_event_scheduler",&simx::Python::setPyEventScheduler);
  python::def("set_event_scheduler_timer",&simx::Python::setPyEventSchedulerTimer);
}
