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

#include "simx/control.h"
#include "simx/logger.h"
#include "simx/InfoManager.h"
#include "simx/LP.h"

#include <iostream>
#include "simx/Common/Assert.h"   

using namespace std;
using namespace simx;

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

    Logger::debug2() << "Person " << id << ": is being created with input=" 
		     << input << " at time " << getNow() << endl;
    
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
