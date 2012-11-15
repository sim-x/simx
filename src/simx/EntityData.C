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

//--------------------------------------------------------------------------
// File:    EntityData.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 9 2005
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/EntityData.h"
#include "simx/type.h"
#include "simx/logger.h"

#include <iostream>

using namespace std;

namespace simx {

EntityData::EntityData()
    : 	fEntityId(),
	fEntityType(),
	fProfileId(),
	fData()
{
}

EntityData::EntityData(EntityID eid, Entity::ClassType etype, ProfileID eprof)
    :  fEntityId(eid),
       fEntityType(etype),
       fProfileId(eprof),
       fData()
{
    fData = boost::shared_ptr<stringstream>(new stringstream(""));
}

EntityData::~EntityData()
{
}

void EntityData::read(istream& is)
{
    fData.reset(new stringstream());
    is	>> fEntityId
	>> fEntityType
	>> fProfileId;
    if( is.good() )
    {
	// only read custom data if there is any
	is >> fData->rdbuf();
    }
    SMART_VERIFY( !is.fail() )( is.rdstate() );	// make sure that read-in was correct

    if( fEntityId==EntityID() )
    {
	Logger::warn() << "EntityData: EntityId==" << EntityID() << " is invalid" << endl;
    }
}

void EntityData::print(ostream& os) const
{
    os	<< "("
	<< fEntityId << ","
	<< fEntityType << ","
	<< fProfileId << ","
	<< (fData ? fData->str() : "NULL") 
	<< ")" ;
}

EntityID EntityData::getEntityId() const 
{
    return fEntityId;
}

Entity::ClassType EntityData::getClassType() const 
{
    return fEntityType;
}

ProfileID EntityData::getProfileId() const 
{
    return fProfileId;
}

Input::DataSource& EntityData::getData() 
{
    SMART_ASSERT(fData); 
    return *fData;
}


} // namespace

