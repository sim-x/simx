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
// @@COPYRIGHT@@
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
    simx::theEntityManager().createController();
}

void Global::startSimulation() {
  // Control::prepareServices();
  //Control::createEntities();
  Control::startSimulation();
}
