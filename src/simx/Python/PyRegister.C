//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        PyRegister.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     July 31 2012 
// Description: Registration functions for simx objects from Python.
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include <boost/python.hpp>
#include "simx/EntityManager.h"
#include "simx/Entity.h"
#include "simx/ServiceManager.h"
#include "simx/Service.h"
#include "simx/InfoManager.h"
#include "simx/userIO.h"
#include "simx/Python/PyInfo.h"

using namespace boost;
using namespace std;

namespace simx {
  namespace Python {

    //void registerInfos();
    // TODO (python , high. should not need a numerical value for registration
    void registerInfos() {
      theInfoManager().registerInfo< PyInfo >(1000);
    }

    void PyRegisterEntity( //const Entity::ClassType& name,
			   const python::object& py_entity_class) {
      
      Entity::ClassType ent_type;
      try {
	ent_type = python::extract<Entity::ClassType>
	  ( py_entity_class.attr("__name__"));
      }
      catch ( ... )
	{
	  PyErr_Print();
	  PyErr_Clear();
	  Logger::failure("PyRegister: Unable to register python entity");
	  return;
	}
      theEntityManager().registerPyEntity( ent_type, py_entity_class );
    }

    void PyRegisterService( //const Service::ClassType& name,
			    const python::object& py_service_class) {
      
      Service::ClassType serv_type;
      try {
	serv_type = python::extract<Service::ClassType>
	  ( py_service_class.attr("__name__"));
      }
      catch ( ... )
	{
	  PyErr_Print();
	  PyErr_Clear();
	  Logger::failure("PyRegister: Unable to register python service");
	  return;
	}
      theServiceManager().registerPyService( serv_type, py_service_class );
    }

    void PyRegisterPacker( const python::object& packer )
    {
      theInfoManager().fPacker = packer;
    }

    void PyRegisterUnpacker( const python::object& unpacker )
    {
      theInfoManager().fUnpacker = unpacker;
    }

    void PyRegisterServiceAddress(const python::str& addr_name,
				  const int addr_val )
    {
      string name;
      ServiceAddress addr;
      try {
      name = python::extract<string>(addr_name);
      addr = static_cast<ServiceAddress>( addr_val );
      UserIO::setPair(name, addr);
      

      }
      catch (...)
	{
	  PyErr_Print();
	  PyErr_Clear();
	  
	}
    }


  }
}


void export_PyRegister() {

  python::def("register_entity",simx::Python::PyRegisterEntity);
  python::def("register_service",simx::Python::PyRegisterService);
  python::def("register_packer",simx::Python::PyRegisterPacker);
  python::def("register_unpacker",simx::Python::PyRegisterUnpacker);
  python::def("register_address",simx::Python::PyRegisterServiceAddress);
}
