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
