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

/*
 * =====================================================================================
 *
 *       Filename:  HelloServicePerson.C
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/10/2011 13:06:52
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nicholas Valler <nvaller@cs.ucr.edu>
 *        Company:  
 *
 * =====================================================================================
 */

#include "HelloWorld/HelloHandlerPerson.h"
#include "HelloWorld/Person.h"
#include "HelloWorld/constants.h"
#include "HelloWorld/HelloWorld.h"

#include "SimCore/InfoManager.h"
#include "SimCore/logger.h"
#include "SimCore/LP.h"
#include "SimCore/output.h"

#include "SimCore/Common/Assert.h"

#include <ostream>

using namespace std;
using namespace SimCore;

namespace HelloWorld {

    void HelloHandlerPersonInput::readProfile(Input::ProfileSource& p) {}

    ///Print for debug purposes.
    void HelloHandlerPersonInput::print( std::ostream& os) const {
        os << "HelloHandlerInput( ) ";
    }

    ///Constructor
    HelloHandlerPerson::HelloHandlerPerson( 
            const ServiceName& name,
            Person& person,
            const HelloHandlerPersonInput& input )
        :   Service( name, person, input),
            fPerson( person ) {
            
        Logger::debug2() << "Service in constructor " << name << person << input << endl;
    } 

    ///Destructor
    HelloHandlerPerson::~HelloHandlerPerson() {}

    void HelloHandlerPerson::receive( boost::shared_ptr< Hello > info ) {
        SMART_ASSERT( info );
        Logger::debug3() << "HelloHandlerPerson::receive( Hello ): " << *info;

	Output::output(*this, 1) << "Received a hello!";
        
        /// Create Reply Object
        boost::shared_ptr< Reply > reply;
        theInfoManager().createInfo( reply );

        Logger::debug3() << "HelloHandlerPerson::receive( Hello ): Created Reply: " << *reply;
        /// Fill with data
        reply->fSrcPersonID = getEntityId();
        reply->fDstPersonID = info->fSrcPersonID;
        sendInfo( reply, LP::MINDELAY, info->fSrcPersonID, eAddr_HelloHandlerPerson);
    }
    
    void HelloHandlerPerson::print(std::ostream& os) const {
        os << "HelloHandlerPerson("
           << "EntityId = " << getEntityId() << ", ";
           Service::print(os);
        os << ") ";
    }
    

    void HelloHandlerPerson::receive( boost::shared_ptr< Reply > info ) {
        Logger::debug3() << "HelloHandlerPerson::receive( Hello ): " << *info << std::endl;

	Output::output(*this, 1) << "Received a reply!";

    }
}

