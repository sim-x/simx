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
