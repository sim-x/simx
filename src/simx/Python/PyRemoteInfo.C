// Copyright (c) 2012, 2013 Los Alamos National Security, LLC. 

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
// File:        PyRemoteInfo.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     May 5 2013
// Description: Only for SSF. This class will be used for Infos that have to be sent to
//              entitities living in a diffrent memory space
// @@
//
//--------------------------------------------------------------------------

#ifdef SIMX_USE_PRIME

#include "simx/Python/PyRemoteInfo.h"
#include "simx/InfoManager.h"

using namespace boost;
using namespace boost::python;
using namespace std;

namespace simx {
  
  namespace Python {
    
    PyRemoteInfo::PyRemoteInfo() {}
    
    PyRemoteInfo::~PyRemoteInfo() {}
    
    void PyRemoteInfo::pack( PackedData& pd ) const
    {
      pd.add( fPickledData );
    }
    
    
    void PyRemoteInfo::unpack(PackedData& pd ) 
    {
      pd.get( fPickledData );
      assert( fPickledData );
    }
    
    
    bool PyRemoteInfo::pickleData( const python::object& py_obj )
    {
      try 
	{
	  fPickledData = python::extract<string>(theInfoManager().
						 getPacker()(py_obj));
	  return true;
	}
      catch(...)
	{
	  PyErr_Print();
	  PyErr_Clear();
	  return false;
	}
	
    }


  }
}

#endif
