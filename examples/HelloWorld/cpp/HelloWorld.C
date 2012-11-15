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
// File:    HelloWorld.C
// Module:  HelloWorld
// Author:  Nicholas Valler <nvaller@cs.ucr.edu>
// Created: August 10 2011
//
//
//--------------------------------------------------------------------------

#include "SimCore/Global/main.h"
#include "SimCore/LP.h"
#include "SimCore/control.h"    // SimCore object that provides basic 
                                // simulation structure.
#include "SimCore/EntityManager.h"
#include "SimCore/logger.h"



namespace HelloWorld {
    void registerAll(); 
}

SimCore::Time  SimCore::LP::LP::MINDELAY;

using namespace SimCore;

void Global::ModuleMain() {
  Global::ModuleMain1();
  Global::ModuleMain2();
}
  

void Global::ModuleMain1() {
    Control::init("HelloWorld");
    HelloWorld::registerAll();
    Control::prepareOutput();
}

void Global::ModuleMain2() {
  // Control::prepareServices();
  //Control::createEntities();
  Control::startSimulation();
}
