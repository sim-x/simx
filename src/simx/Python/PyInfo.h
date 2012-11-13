// $Id$
//--------------------------------------------------------------------------
//
// File:        PyInfo.h
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     August 3 2012 
// Description: Base class for Infos in Python
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef SIMX_PY_INFO_H
#define SIMX_PY_INFO_H


#include "simx/Info.h"
#include <boost/python.hpp>
//#include <boost/serialization/access.hpp>
//#include <boost/make_shared.hpp>
#include "simx/PackedData.h"

//#include <boost/archive/text_iarchive.hpp>;

//#include <boost/archive/text_iarchive.hpp>
namespace simx {

  namespace Python {

    struct PyObjHolder {

      boost::python::object fObj;
    };

    struct PyInfo : public Info {
      
      
      //friend class boost::serialization::access;
      
      //template<class Archive>
      // void serialize(Archive & ar, const unsigned int version)
      // {
      //   ar & fData;
      // }


      PyInfo(); 
      virtual ~PyInfo();
      
      //void readData( Input::DataSource& );

      virtual void pack(simx::PackedData&) const;
      virtual void unpack(simx::PackedData&);

      void readData( const boost::python::object& data);
     
      void setData( const boost::python::object& data);

      const boost::python::object& getData() const;

      //boost::shared_ptr<boost::python::object> fData;
      //boost::python::str fPickledData;
      std::string fPickledData;
      bool fPickled;
    private:
      //const boost::python::object* fData;
      //boost::python::object fData2;
      boost::shared_ptr<PyObjHolder> fDataPtr;

    };

    inline const boost::python::object& PyInfo::getData() const
    {
      return fDataPtr->fObj;
    }
    
    inline void PyInfo::setData( const boost::python::object& data )
    {
      fDataPtr = boost::shared_ptr<PyObjHolder>(new PyObjHolder);
      fDataPtr->fObj = data;
    }
  } //namespace simx
} // namespace Python


#endif
