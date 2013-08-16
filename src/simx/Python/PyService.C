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

// $Id$
//--------------------------------------------------------------------------
//
// File:        PyService.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     August 1 2012 
// Description: Base class for services in Python
//
// @@
//
//--------------------------------------------------------------------------


#include "simx/Python/PyService.h"
#include "simx/InfoManager.h"

#include "simx/Python/PyUtility.h"
#ifdef SIMX_USE_PRIME
#include "simx/EntityManager.h"
#include "simx/Python/PyRemoteInfo.h"
#endif


using namespace std;
using namespace boost;


namespace simx {

  namespace Python {

    void PyService::receive( shared_ptr<PyInfo> info )

    {
      Logger::debug3() << "PyService.C: Service " << getName() << ": PyInfo received" << endl;
      //PyGILState_STATE gstate;
      //gstate = PyGILState_Ensure();
      try {
	if ( info->fPickled )
	  //   fPyObj.attr("recv")(info->fPickledData,true);
	  {
#ifdef SIMX_USE_PRIME
	    SMART_ASSERT(false)
	      ("Pickled PyInfo should never be used with SSF. Only PyRemoteInfo may be used");
#endif
	    //info->fData = theInfoManager().getUnpacker()(info->fPickledData);
	    
	    info->setData( theInfoManager().
			   getUnpacker()(
					 info->fPickledData));
	    
	    //fPyObj.attr("recv")( theInfoManager().getUnpacker()( info->fPickledData ));
	    
	  }
	//else
	//fPyObj.attr("recv")(info->fData,false);
	//fPyObj.attr("recv")(*(info->fData));
	//assert(info->getData());
	//else
	fPyObj.attr("recv")(info->getData());
      }
      catch(...)
	{
	  Logger::error() << "PyService.C: Service " << getName()
			  << ": error in handling incoming info" << endl;
	  PyErr_Print();
	  PyErr_Clear();
	}
      //PyGILState_Release(gstate);
    }

/////////////////////////////////////////////////////////////////////
    
#ifdef SIMX_USE_PRIME
    void PyService::receive( shared_ptr<PyRemoteInfo> info )
    {
      Logger::debug3() << "Pyservice.C: Service " << getName() 
		       << ": PyRemoteInfo received" << endl;
      try
	{
	  //theInfoManager().getUnpacker();
	  //theInfoManager().getUnpacker()( info->fPickledData );
	  fPyObj.attr("recv")( theInfoManager().getUnpacker()
			       ( info->fPickledData));
	}
      catch(...)
	{
	  Logger::error() << "Pyservice.C: Service " << getName()
			  << ": error in handling incoming info" << endl;
	  PyErr_Print();
	  PyErr_Clear();
	}
      
    }
#endif
    
  
    /////////////////////////////////////////////////////////////////////

    void PyService::sendPyInfo(const python::object& py_info,
			       const Time time,
			       const python::tuple& dest_ent,
			       const long dest_serv ) {

      Logger::debug3() << "PyService: (" << getName() <<  ") on Entity" 
		       << getEntityId() << "sending PyInfo" << endl;
      fPyEnt.sendPyInfo( py_info, time, dest_ent, dest_serv);
     //  return;

//       EntityID e_id = py2EntityId( dest_ent );
      
// #ifdef SIMX_USE_PRIME
//       if ( theEntityManager().findEntityLpId(e_id) !=
// 	   Control::getRank() )
// 	{
// 	  Logger::debug3() << "PyService on Entity " << getEntityId()
// 			   << " : Sending remotely, proceeding to pickle Python object" << endl;
// 	  shared_ptr<PyRemoteInfo> info;
// 	  theInfoManager().createInfo( info );
// 	  if ( ! info->pickleData( py_info ) )
// 	    {
// 	      Logger::error() << "PyService.C: Service on entity " << getEntityId()
// 			      << ": Error while pickling info. Not sending" << endl;
// 	      return;
// 	    }
// 	  else // succesful pickling, send it off
// 	    {
// 	      sendInfo( info, time, e_id, 
// 			static_cast<ServiceAddress>( dest_serv ) );
// 	    }
// 	}
//       else // destination entity lives on the same LP. Use regular PyInfo for sending
// 	{
// 	   Logger::debug3() << "PyService on Entity " << getEntityId()
// 			    << " : Sending locally" << endl;
// #endif
// 	  // either we are using SimEngine -- in which case sending local 
// 	  // and remote infos use the same procedure -- or
// 	  // we are using SSF and sending locally.


// 	  shared_ptr<PyInfo> info;
// 	  theInfoManager().createInfo( info );
// 	  info->setData( py_info );
// 	  sendInfo( info, time, e_id, 
// 		    static_cast<ServiceAddress>( dest_serv ) );
// #ifdef SIMX_USE_PRIME
// 	} //close-out else block
// #endif
      
//       //info->fData = boost::make_shared<boost::python::object>(py_info);
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
