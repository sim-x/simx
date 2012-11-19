//--------------------------------------------------------------------------
// File:    HelloWorld.C
// Module:  HelloWorld
// Author:  Nicholas Valler <nvaller@cs.ucr.edu>
// Created: August 10 2011
//
//
//--------------------------------------------------------------------------

#include "simx/Global/main.h"
#include "simx/LP.h"
#include "simx/control.h"    // simx object that provides basic 
                                // simulation structure.
#include "simx/EntityManager.h"
#include "simx/logger.h"



namespace HelloWorld {
    void registerAll(); 
}

simx::Time  simx::LP::LP::MINDELAY;

using namespace simx;

void Global::ModuleMain() {
    Control::init("HelloWorld");
    HelloWorld::registerAll();
    Control::prepareOutput();
    Control::prepareServices();
    Control::createEntities();
    Control::startSimulation();
}
