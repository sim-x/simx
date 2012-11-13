/*
 * =====================================================================================
 *
 *       Filename:  constants.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/10/2011 14:26:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nicholas Valler <nvaller@cs.ucr.edu>
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef NISAC_HELLOWORLD_CONSTANTS
#define NISAC_HELLOWORLD_CONSTANTS

#include "SimCore/constants.h"

namespace HelloWorld {
    using SimCore::ServiceAddress;

    //------------------------------------------------------------------
    //Service Addresses
    const ServiceAddress eAddr_HelloHandlerPerson = ServiceAddress(11101);

    //------------------------------------------------------------------
    //Info Types -- only used if infos are read from a file
    const int Hello_InfoType         = 10101;
    const int Reply_InfoType         = 10102;

} //namespace

#endif
