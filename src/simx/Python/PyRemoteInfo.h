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
// File:        PyRemoteInfo.h
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     May 5 2013
// Description: Only for SSF. This class will be used for Infos that have to be sent to
//              entitities living in a diffrent memory space
// @@
//
//--------------------------------------------------------------------------


#ifdef SIMX_USE_PRIME

#ifndef SIMX_PY_REMOTE_INFO_H
#define SIMX_PY_REMOTE_INFO_H

#include "simx/Info.h"
#include <boost/python.hpp>
#include "simx/PackedData.h"

/// We need a separate class for info's that are sent 
/// and received across memory spaces while using SSF.
/// In SSF, there is a main thread that performs the 
/// simulation, and depending on the MPI threading support 
/// available, there are one or more reader/writer threads.
/// 
/// If full multi-threaded MPI support is available 
/// (MPI_THREAD_MULTIPLE is enabled ins SSF) there is a separate 
/// writer and reader thread in SSF that deals with MPI sends and 
/// receives respectively
/// 
/// If MPI_THREAD_MULTIPLE is not available, there is just
/// one SSF writer/reader thread that handles MPI sends and receives
///
/// In either case, the writer thread (or the reader/writer) thread
/// deletes the EventInfo once it is packed and sent off. If we used 
/// the regular PyInfo object (that holds a reference to the boost::
/// python::object that is being sent), this would eventually call
/// the python object destructor, invoking calls to Py_INCREF and Py_DECREF
/// Since these Python calls are coming from non-Python threads
/// (i. the writer thread or rw thread), crashes result as the 
/// main Python thread (that is driving the simulation) is also running
/// at the same time. The thread-safety locking mechanims of 
/// PyGILState_Ensure() ... PyGILState_Release() do not work, as the 
/// main thread never really gives up the GIL; calling these before object
/// deletion would simply result in a hang.
///
/// The solution to this is to prevent the writer (or rw) thread from making
/// any python related calls. The way to do this is to determine, at sending
/// time if the info will be sent to a remote LP, in which case the python
/// object is immediately pickled into a string. PyRemoteInfo then only holds a
/// regular string; while deleting it, no python calls need to be made by the
/// deleting thread. 
///
/// If the info is being sent locally, we use regular PyInfo class that does
/// hold a  reference to a  Python object. Since no MPI is involved in this 
/// case, no reader/writer threads to worry about.

namespace simx {

  namespace Python {

    struct PyRemoteInfo : public Info {

      PyRemoteInfo();
      virtual ~PyRemoteInfo();
      virtual void pack(simx::PackedData&) const;
      virtual void unpack(simx::PackedData&);
      bool pickleData( const boost::python::object&);
      
      
      std::string fPickledData;
    };

  }
}

#endif
#endif
