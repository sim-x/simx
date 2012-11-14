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
// File:    register.C
// Module:  HelloWorld
// Author:  Nicholas Valler
// Created: August 11, 2011
//
// Description: registers all user-defined objects with the system
//
// @@
//
//--------------------------------------------------------------------------


#include "SimCore/EntityManager.h"
#include "SimCore/InfoManager.h"
#include "SimCore/ServiceManager.h"
#include "SimCore/userIO.h"

#include "SimCore/userIO.h"
#include "SimCore/EntityManager.h"
#include "SimCore/ServiceManager.h"
#include "SimCore/InfoManager.h"

#include "HelloWorld/constants.h"
#include "HelloWorld/HelloWorld.h"
#include "HelloWorld/Person.h"
#include "HelloWorld/HelloHandlerPerson.h"

using namespace Config;
using namespace SimCore;
using namespace std;

/// registering of all user-defined objects
namespace HelloWorld {
    void registerEntities() {
        // register the Person entity
      //theEntityManager().registerEntity<Person, PersonInput>("person");
    }

    void registerServices() {
      //theServiceManager().
      //  registerService<HelloHandlerPerson, Person, HelloHandlerPersonInput>
      //  ("HelloHandlerPerson");
    }

    void registerInfos() {
        // Constants for External Infos
        enum eEventType {
            HELLO_MESSAGE = 10101,
            REPLY_MESSAGE = 10102
        };

        theInfoManager().registerInfo< Hello >( HELLO_MESSAGE );
        theInfoManager().registerInfo< Reply >( REPLY_MESSAGE );
    }

    void registerServiceAddresses() {
        // Matching between ServiceAddress strings and number, used for input in operator >>
        UserIO::setPair("eAddr_HelloHandlerPerson", eAddr_HelloHandlerPerson);
    }

    void registerAll() {
      //registerEntities();
      //registerServices();
      //registerInfos();
      //registerServiceAddresses();
    }
} // namespace
