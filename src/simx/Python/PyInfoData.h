//--------------------------------------------------------------------------
// $Id$
//--------------------------------------------------------------------------
//
// File:        PyInfoData.h
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     July 10 2012 
// Description: info data structure for simx infos created from python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef SIMX_PY_INFO_DATA
#define SIMX_PY_INFO_DATA

#include "simx/type.h"
#include <boost/python.hpp>
#include "simx/Info.h"
#include "simx/InfoData.h"


namespace simx {
  
  namespace Python {
    
    struct PyInfoData  : public InfoData {
      
      //TODO: custom data (fData) should really be a python object
      // TODO: can(should) we pass references to the constructor instead?
    PyInfoData (  Time time, 
		 boost::python::tuple& ent_id, 
		 long address,
		 Info::ClassType type,
		 ProfileID pr_id,
		   PyProfile& profile,
		  boost::python::object& data);
		  //std::string& data);

      // fTime( time ),
      // fEntityId( py2EntityId(ent_id) ),
      // fServiceAddress( address ),
      // fType( type ), 
      // fProfileId( pr_id ),
      // fData( data ) 

      // InfoData( time, py2EntityId(ent_id), static_cast<ServiceAddress>( address ),
      // 		type, pr_id )

      // {}
      
      //PyInfoData() {}
      
      boost::python::object& getData() { return fData; }
      const PyProfile& getProfile() { return fProfile; }

    private:
      
    //   Time fTime;
    //   boost::python::tuple fEntityId;
    //   long      fServiceAddress;
    //   Info::ClassType      fType;
    //   ProfileID              fProfileId;
      PyProfile                    fProfile;
      boost::python::object        fData;

    //   std::string& fData;
    };
      
  } // namespace
} //namespace
#endif
