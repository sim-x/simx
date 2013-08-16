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
// File:        PyInfo.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     August 3 2012 
// Description: Base class for Infos in Python
//
// @@
//
//--------------------------------------------------------------------------


#include <boost/python.hpp>
#include "simx/Python/PyInfo.h"
#include "simx/InfoManager.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/mpi/python/serialize.hpp>

using namespace boost;
using namespace boost::python;
using namespace std;

//extern python::str dumps(python::object obj, int protocol = -1);
//extern python::object loads(python::str s);
// namespace {

// }

namespace simx {
  namespace Python {
    
    // class X{
    // public:
    //   X() {}
    // };


    // defined here instead of in PyInfo.h, since I kept changing
    // the internals of this struct, and did not want it triggering
    // a recompile each time while building. - ST.
    struct PyObjHolder {
      
      // PyObjHolder(const boost::python::object& data):
      // 	fObj( data) {}
      
      virtual ~PyObjHolder() {}
      
      //const boost::python::object* fObj;
      boost::python::object fObj;
      //PyObject* pop;
    };


    //TODO (critical) this needs to be inlined
    const boost::python::object PyInfo::getData() const
    {
      //assert(fDataPtr->fObj);
      //assert(fDataPtr->pop);
      //return *(fDataPtr->fObj);
      //boost::python::api::object( fDataPtr->pop);
      return fDataPtr->fObj;
    }
    
    //TODO (critical) this needs to be inlined 
    void PyInfo::setData( const boost::python::object& data )
    {
      //fDataPtr = boost::shared_ptr<PyObjHolder>(new PyObjHolder( data ));
      fDataPtr = boost::shared_ptr<PyObjHolder>(new PyObjHolder);
      //fDataPtr = new PyObjHolder;
     
      //fDataPtr->fObj = &data;
      fDataPtr->fObj = data;
      //Py_INCREF(data.ptr());
      //fDataPtr->pop = data.ptr();

      

    }
    

    PyInfo::PyInfo() {
      fPickled = false;
      //fData2 = python::api::object();
      //fData2 = python::api::object(shared_ptr<X>(new X));
    }

    

    PyInfo::~PyInfo() {
    }
    
    void PyInfo::readData(const python::object& data)
     {
	//assert(false);
	//Py_INCREF(data.ptr());
       //fData = &data;
       setData( data );
	//boost::shared_ptr<boost::python::object> data_ptr 
	//fData = boost::make_shared<boost::python::object>(data);
      }

   
  
  
    void PyInfo::pack( PackedData& pd ) const
    {
#ifdef SIMX_USE_PRIME
      SMART_ASSERT(false)
	("PyInfo::pack() method must never get called when using SSF. Only PyRemoteInfo may be used");
#else
      //cout << "inside pack" << endl;
      //std::ostringstream os;
      // save data to archive
      //boost::archive::text_oarchive oa(os);
      // write class instance to archive
      //      assert(false);
      //oa << fData;
      //fData.serialize(oa,1);)
      //assert(false);
      // archive and stream closed when destructors are called
      //boost::python::str py_string = boost::python::pickle::dumps(fData);

      // python::object pickler = python::import("cPickle");
      // python::str ps = python::extract<python::str>
      // 	(pickler.attr("dumps")(fData));

      // python::str ps = //python::extract<python::str>
      //	extract<str>( theInfoManager().getPacker()(fData) );
      //string s = python::extract<string>(ps);
      //pd.add(s);
  
      //pd.addAnything(fData);
      //boost::python::str ps = dumps(fData);
      //pd.add(ps);
      //PyGILState_STATE gstate;
      //gstate = PyGILState_Ensure();
      //Py_INCREF(fData.ptr());
      //assert(fData.ptr());
     
      pd.add( 
	     //python::extract<python::str>
	     //(theInfoManager().getPacker()(*fData)));
	     python::extract<string>
	     (theInfoManager().getPacker()(getData())));
      //pd.add( fPickledData );

      //pd.add(theInfoManager().getPacker()(fData));
      //PyGILState_Release(gstate);
      
      //pd.add( theInfoManager().getPacker()(fData) );
#endif
    }


    void PyInfo::unpack( PackedData& pd ) {
#ifdef SIMX_USE_PRIME
      SMART_ASSERT(false)
	("PyInfo::unpack() method must never get called when using SSF. Only PyRemoteInfo may be used");
#else
      //cout << "Inside unpack" << endl;
      fPickled = true;
      if(!pd.get( fPickledData ))
	assert( false);
      //pd.get(fData);
      //assert(pd.getAnything( fData ));

      /* // python::object pickler = python::import("cPickle");
      
      string s;
      pd.get( s );
      Logger::debug3() << "PyInfo: Inside unpack" <<  s << endl;
      //      assert(s);
      //fData = pickler.attr("loads")(s);
      //fData = theInfoManager().getUnpacker()(s);
      python::object up = theInfoManager().getUnpacker();
      assert(up);
      try {
      fData =  up(s);
      }

      catch(...) {
	PyErr_Print();
	PyErr_Clear();
      }
      //string name =  python::extract<string>( up.attr("__name__") );
      //assert(o);
      //cout << name << endl;*/
#endif
    }



  }
}

// not needed
// void export_PyInfo() {
  
//   //python::class_<simx::Python::PyInfo >("PyInfo",python::no_init )
    
//     ;
// }
using namespace boost;
using namespace boost::python;
using namespace simx;
using namespace simx::Python;

// void export_X() {

//   class_<X, boost::shared_ptr<X> >("X",python::init<>() );
// }
