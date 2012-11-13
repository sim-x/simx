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
// @@COPYRIGHT@@
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
