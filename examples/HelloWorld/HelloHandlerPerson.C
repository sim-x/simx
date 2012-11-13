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

