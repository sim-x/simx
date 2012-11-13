/*
 * =====================================================================================
 *
 *       Filename:  Person.h
 *
 *    Description:  Person entity definition
 *
 *        Version:  1.0
 *        Created:  08/10/2011 11:30:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nicholas Valler <nvaller@cs.ucr.edu>
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef NISAC_HELLOWORLD_PERSON
#define NISAC_HELLOWORLD_PERSON

#include "HelloWorld/HelloWorld.h"
#include "HelloWorld/constants.h"
#include "HelloWorld/types_HelloWorld.h"

#include "SimCore/Entity.h"
#include "SimCore/Info.h"

using namespace SimCore;

namespace HelloWorld {
    
    /// information taken as input in Person Creation
    struct PersonInput : public EntityInput {
        void readData(DataSource&);
        void readProfile(ProfileSource&);
        void print(std::ostream&) const;
    
        ///Profile Entries

        ///Data Entries
        std::vector<PersonID> fNeighborList;
    };

    /// Person Object
    class Person : public Entity {
        public:
            Person(const PersonID, LP&, const PersonInput&);               ///< Constructor
            virtual ~Person();                          ///< Destructor
            virtual void print(std::ostream&) const;

            ///Information Methods
            Time getNow() const;

        private:
            std::vector<PersonID> fNeighborList;
            PersonID fID;
    };

//inline PersonID Person::getID() const {
//    return fID; }

}//end HelloWorld Namespace


#endif
