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

#include "simx/Service.h"
#include "simx/Info.h"
#include "simx/logger.h"

using namespace simx;

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
