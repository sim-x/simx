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
