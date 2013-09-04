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
// File:        PyMain.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     Oct 05 2012 
// Description: Initialization hooks for python
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/Global/main.h"
#include "simx/LP.h"
#include "simx/control.h"
#include "simx/Python/PyInfo.h"
#include "simx/EntityManager.h"

//TODO (high): why is this declared here?
simx::Time simx::LP::LP::MINDELAY;

namespace simx {
  namespace Python {
    void registerInfos();
  }
}

using namespace simx;

void Global::initEnv() {
    Control::init("Python");
    Control::prepareOutput();
    simx::Python::registerInfos();
    // won't need this. Will create a controller that is a PyEntity
    // from within Python.
    //simx::theEntityManager().createController();
}

void Global::startSimulation() {
  // Control::prepareServices();
  //Control::createEntities();
  Control::startSimulation();
}
