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
// File:        PyService.h
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     August 1 2012 
// Description: Base class for services in Python
//
// @@
//
//--------------------------------------------------------------------------

#ifndef SIMX_PY_SERVICE_H
#define SIMX_PY_SERVICE_H


#include "simx/Service.h"
#include "simx/Python/PyEntity.h"
#include "simx/Python/PyServiceInput.h"
#include "simx/Python/PyInfo.h"
#include <boost/python.hpp>

namespace simx {

  namespace Python {

    //class PyServiceInput;

    class PyService : public Service, 
		      public InfoRecipient<PyInfo> 
    {
      
    public:
      PyService( const ServiceName& name, PyEntity& ent, 
		 const PyServiceInput& input, const boost::python::object& serv_obj )
	: Service(name, ent, input),
	fPyEnt(ent), fPyObj( serv_obj ){

	    Logger::debug3() << "I'm a c++ PyService" << std::endl;
	    i = 9374;
	//print_i();
	//fPyObj( name, ent, input );
	//print_i();
	
      }
    // PyService( const ServiceName& name, PyEntity& ent, 
    // 		 const PyServiceInput& input )
    // 	: Service(name, ent, input),
    // 	fPyEnt( ent)
    //   {
    // 	//this function should not get called
    // 	assert(false);
    // 	Logger::debug3() << "I'm a regular c++ PyService" << std::endl;
    //   }
      virtual ~PyService() {}
      virtual void receive( boost::shared_ptr<PyInfo> );

      void sendPyInfo( const boost::python::object& info,
		       const Time time,
		       const boost::python::tuple& dest_ent,
		       const long dest_serv );

      void print_i();
      int i;
      
      const PyEntity& getEntity() const
      {
	return fPyEnt;
      }

      boost::python::object& getPyObject()
      {
	return fPyObj;
      }

      boost::python::tuple getPyEntityId() const;


    private:
      PyEntity& fPyEnt;
      boost::python::object fPyObj; // Python service object

    };

  } //namespace 
} // namespace


#endif
