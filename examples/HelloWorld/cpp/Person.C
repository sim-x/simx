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
 *       Filename:  Person.C
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/10/2011 11:30:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Nicholas Valler <nvaller@cs.ucr.edu> 
 *        Company:  
 *
 * =====================================================================================
 */

#include "HelloWorld/Person.h"
#include "HelloWorld/constants.h"
#include "HelloWorld/types_HelloWorld.h"

#include "SimCore/control.h"
#include "SimCore/logger.h"
#include "SimCore/InfoManager.h"
#include "SimCore/LP.h"

#include <iostream>
#include "SimCore/Common/Assert.h"   

using namespace std;
using namespace SimCore;

namespace HelloWorld {


//====================================================================
// PersonInput

void PersonInput::readData(Input::DataSource& i) {
    EntityInput::readData(i);
    i >> fNeighborList;
}

void PersonInput::readProfile(ProfileSource& p) {
    EntityInput::readProfile(p);
}

void PersonInput::print(ostream& os) const {
    os  << "PersonInput("
        << "fNeighborList = " << fNeighborList << ")";
}

//====================================================================
// Person

// static variables

Person::Person( const PersonID id, 
                LP& lp, 
                const PersonInput& input) 
        : Entity( id, lp, input),
          fNeighborList( input.fNeighborList) {

    Logger::debug2() << "Person " << id << ": is being created with input=" << input << endl;
    
    createServices(*this, input.fServices);
    Logger::debug3() << "Person " << id << ": done: " << *this << endl;
}

Person::~Person() {
    //Logger::debug2() << "Person " << id << ": in destructor" << endl;
}

void Person::print(std::ostream& os) const {
    os << "Person("
       << "Neighbors = " << fNeighborList;
    Entity::print( os );
    os << " )";
}


} //namespace
