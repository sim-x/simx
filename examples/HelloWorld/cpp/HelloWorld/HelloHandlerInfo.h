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


#include "simx/Info.h"

using namespace std;
using namespace simx;

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
