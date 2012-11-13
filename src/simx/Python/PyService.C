// $Id$
//--------------------------------------------------------------------------
//
// File:        PyService.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     August 1 2012 
// Description: Base class for services in Python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------


#include "simx/Python/PyService.h"
#include "simx/InfoManager.h"
#include "simx/Python/PyUtility.h"



using namespace std;
using namespace boost;


namespace simx {

  namespace Python {

    
    

    void PyService::receive( shared_ptr<PyInfo> info )

    {
      Logger::debug3() << "PyService.C: Service " << getName() << " PyInfo received" << endl;
      PyGILState_STATE gstate;
      gstate = PyGILState_Ensure();
      try {
	if ( info->fPickled )
	  //   fPyObj.attr("recv")(info->fPickledData,true);
	  {
	    //info->fData = theInfoManager().getUnpacker()(info->fPickledData);
	    info->setData( theInfoManager().
	     		   getUnpacker()(
					 info->fPickledData));
	  }
	//else
	//fPyObj.attr("recv")(info->fData,false);
	//fPyObj.attr("recv")(*(info->fData));
	fPyObj.attr("recv")(info->getData());
      }
      catch(...)
	{
	  Logger::error() << "PyService.C: Service " << getName()
			  << " error in handling incoming info" << endl;
	  PyErr_Print();
	  PyErr_Clear();
	}
      PyGILState_Release(gstate);
    }

    void PyService::sendPyInfo(const python::object& py_info,
			       const Time time,
			       const python::tuple& dest_ent,
			       const long dest_serv ) {

      EntityID e_id = py2EntityId( dest_ent );
      
      shared_ptr<PyInfo> info;
      theInfoManager().createInfo( info );
      //info->fData = &py_info;
      info->setData( py_info );
      //info->fData = boost::make_shared<boost::python::object>(py_info);
      sendInfo( info, time, e_id, 
		static_cast<ServiceAddress>( dest_serv ) );
    }

    void PyService::print_i() {
      std::cout << "i is " << i << std::endl;
    }

    boost::python::tuple PyService::getPyEntityId() const {
      return fPyEnt.getPyId();
    }

  }
}

using namespace boost;
using namespace boost::python;

void export_PyService() {

  

  //void export_PyServiceInput() {

  //boost::python::class_<simx::Python::PyServiceInput>("PyServiceInput",boost::python::init<>() )
  //;
  

  python::class_<simx::ServiceName>("ServiceName",python::no_init);

  python::class_<simx::Python::PyService,
		 python::bases<simx::Service> >
    ("PyService",
					      python::init<const simx::ServiceName&,
					      simx::Python::PyEntity&,
					      const simx::Python::PyServiceInput&,
							   const python::object&>() )
    .def("print_i",&simx::Python::PyService::print_i)
    //.def_readwrite("i_",&simx::Python::PyService::i)
    .def("send_info",&simx::Python::PyService::sendPyInfo)
    .def("get_entity",&simx::Python::PyService::getEntity,
	 return_value_policy<copy_const_reference>())
    .def("get_entity_id",&simx::Python::PyService::
	 getPyEntityId)
    ;
}
