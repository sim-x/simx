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
 *       Filename:  HelloHandlerInfo.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/11/2011 13:11:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nicholas Valler <nvaller@cs.ucr.edu>
 *        Company:  Los Alamos National Laboratory
 *
 * =====================================================================================
 */


#ifndef NISAC_HELLOWORLD_HELLOHANDLERINFO
#define NISAC_HELLOWORLD_HELLOHADNLERINFO

#include "HelloWorld/Person.h"
#include "HelloWorld/types_HelloWorld.h"


#include "SimCore/Info.h"

using namespace std;
using namespace SimCore;

namespace HelloWorld {
    struct Hello : public Info {
        virtual void print(std::ostream& os) const {
            os << "SendingHello()";
        }
        virtual void readData(Input::DataSource& inp) {
            inp >> fSrcPersonID;
        }
        virtual void readProfile(Input::DataSource& inp) {}

        PersonID    fSrcPersonID;
        PersonID    fDstPersonID;
        };

    struct Reply : public Info {
        virtual void print(std::ostream& os) const {
            os << "ReplyingToHello";
        }
        PersonID    fSrcPersonID;      ///Sender
        PersonID    fDstPersonID;   ///Destination
    };
}//end namespace
#endif
