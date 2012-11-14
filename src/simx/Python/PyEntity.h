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
// File:        PyEntity.h
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     July 31 2012 
// Description: Base class for entities in Python
//
// @@
//
//--------------------------------------------------------------------------

#ifndef SIMX_PY_ENTITY_H
#define SIMX_PY_ENTITY_H


#include "simx/Entity.h"
#include "simx/Python/PyEntityInput.h"
#include "simx/LP.h"
#include "simx/Python/PyUtility.h"
//#include "simx/Python/PyService.h"

namespace simx {

  namespace Python {

    //class PyService;
    class PyEntity : public Entity {
      
    public:
      PyEntity( const EntityID& id, LP& lp, const PyEntityInput& input,
		const boost::python::object& obj);
      
      //bool getbool() const { return false; }
      //EntityID getId() const { return Entity::getId();}
      /// return Python ID of this entity
      boost::python::tuple getPyId() const;
      void sendPyInfo(const boost::python::object& info,
				 const Time time,
				 const boost::python::tuple& dest_ent,
				 const long dest_serv);
      boost::python::object& getPyService(long serv_addr);
      void print_address() const;
      boost::python::object& getPyObj();
      void createPyServices( const PyEntityInput& input );
      virtual ~PyEntity() {}
      int i;

      // function to execute call-backs in python
      void pyCallBack( boost::tuple<std::string,
		       boost::python::tuple>& );
      
      // void pyDummyCallBack( boost::tuple<std::string,
      // 			    boost::python::tuple>& t )
      // {}
      

    private:
      boost::python::object fPyObj; // holds the python entity object.
      boost::python::tuple  fPyId;  /// Python-id of this entity

    };

  } //namespace 
} // namespace


#endif
