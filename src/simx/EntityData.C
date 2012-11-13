//--------------------------------------------------------------------------
// File:    EntityData.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 9 2005
//
// @@COPYRIGHT@@
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

