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
 *       Filename:  HelloHanlderPerson.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/10/2011 13:07:09
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nicholas Valler <nvaller@cs.ucr.edu>
 *        Company:  
 *
 * =====================================================================================
 */


#ifndef NISAC_HELLOWORLD_HELLOHANDLERPERSON
#define NISAC_HELLOWORLD_HELLOHANDLERPERSON

#include "HelloWorld/HelloHandlerInfo.h"

#include "SimCore/Service.h"
#include "SimCore/Info.h"
#include "SimCore/logger.h"

using namespace SimCore;

namespace HelloWorld {

    struct HelloHandlerPersonInput : public ServiceInput {
        void readProfile(ProfileSource&);
        virtual void print( std::ostream& ) const;
    };

    

    class HelloHandlerPerson : 
        public Service, 
        public InfoRecipient<Hello>,
        public InfoRecipient<Reply> {
        public:
            HelloHandlerPerson( const ServiceName&, 
                                Person&, 
                                const HelloHandlerPersonInput& );
        
            virtual ~HelloHandlerPerson(void);

            ///InfoHandlers
            virtual void receive( boost::shared_ptr< Hello > );
            virtual void receive( boost::shared_ptr< Reply > );
            virtual void print(std::ostream&) const;

        private:
            Person&     fPerson;
    };
}//namespace
#endif
