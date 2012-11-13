// $Id$
//--------------------------------------------------------------------------
//
// File:        PyInfo.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     August 3 2012 
// Description: Base class for Infos in Python
//
// @@COPYRIGHT@@
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
      pd.add( /*python::extract<python::str>*/
	     //(theInfoManager().getPacker()(*fData)));
	     python::extract<string>
	     (theInfoManager().getPacker()(getData())));
      //pd.add(theInfoManager().getPacker()(fData));
      //PyGILState_Release(gstate);
      
      //pd.add( theInfoManager().getPacker()(fData) );
    }

    void PyInfo::unpack( PackedData& pd ) {
      //cout << "Inside unpack" << endl;
      fPickled = true;
      pd.get( fPickledData );
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
