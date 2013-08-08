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
// File:        export.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     July 5 2012 
// Description: All functions to be exported to python should be listed here
//
// @@
//
//--------------------------------------------------------------------------

#include <boost/python.hpp>

//using namespace simx;

// TODO: (python, high): move all export function into
// simx::Python namespace

void export_SetConfigurationValue();
//void export_init_mpi();
void export_init();
void export_Service();
void export_Entity();
void export_pycontrol();
void export_PyEntityData();
void export_PyCreateEntity();
void export_PyInfoData() ;
void export_PyCreateInputInfo();
void export_PyRegister();
void export_PyEntity();
void export_EntityInput();
void export_LP();
void export_PyService();
//void export_PyInfo();
void export_PyEntityInput();
void export_PyServiceData();
void export_PyServiceInput();
void export_PyLogger();
void export_PyManager();
void export_PyOutput();

void export_Random();
//void export_X();

BOOST_PYTHON_MODULE(core)
{
  
  boost::python::scope().attr("__doc__") = "SimX Documentation";
  PyEval_InitThreads();
  export_SetConfigurationValue();
  export_init();
  export_Entity();
  export_Service();
  export_pycontrol();
  export_PyEntityData();
  export_PyCreateEntity();
  export_PyInfoData();
  export_PyCreateInputInfo();
  export_PyRegister();
  export_PyEntity();
  export_EntityInput();
  export_LP();
  export_PyService();
  //export_PyInfo();
  export_PyEntityInput();
  export_PyServiceInput();
  export_PyServiceData();
  export_PyLogger();
  export_Random();
  export_PyManager();
  export_PyOutput();
  //export_X();
}
