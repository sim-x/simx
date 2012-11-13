//--------------------------------------------------------------------------
// File:    register.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Apr 13 2005
//
// Description: registers things simx works with
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Controller.h"

#include "simx/InfoManager.h"
#include "simx/constants.h"
#include "simx/userIO.h"


/// registering of service names and addresses strings
namespace simx {

void registerAll()
{
    
    theInfoManager().registerInfo<InfoControllerModifyEntity>();
    theInfoManager().registerInfo<InfoWakeupInfoManager>();

}

}
