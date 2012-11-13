//--------------------------------------------------------------------------
// File:    InfoData.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 5 2005
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/InfoData.h"
#include "simx/readers.h"
#include "simx/logger.h"

#include <iostream>

using namespace std;

namespace simx {

InfoData::InfoData()
    : 	fTime(),
	fEntityId(),
	fServiceAddress(),
	fType(),
	fProfileId()
{
}

  InfoData::InfoData(const Time time, const EntityID& id, const ServiceAddress& address, 
		     const Info::ClassType& type, const ProfileID& pr_id)
    : 	fTime( time ),
	fEntityId( id ),
	fServiceAddress( address ),
	fType( type ),
	fProfileId( pr_id )
{
}



InfoData::~InfoData()
{
}

void InfoData::read(istream& is)
{

    is 	>> fTime
	>> fEntityId
	>> fServiceAddress
	>> fType
	>> fProfileId;

    readCustomData( is );
    if( fEntityId==EntityID() )
    {
	Logger::warn() << "InfoData: EntityId==" << EntityID() << " is invalid" << endl;
    }
    if( fServiceAddress==ServiceAddress() )
    {
      Logger::warn() << "InfoData: ServiceAddress==" << ServiceAddress() << " is invalid" << endl;
    }
}

  void InfoData::readCustomData( istream& is ) {
    fData.reset(new stringstream());
    if( is.good() )
      {
	// only read custom data if there is any
	is >> fData->rdbuf();
      }
    SMART_VERIFY( !is.fail() )( is.rdstate() );	// make sure that read-in was correct
  }



Time InfoData::getTime() const 
{ 
    return fTime; 
}

EntityID InfoData::getEntityId() const 
{ 
    return fEntityId; 
}

ServiceAddress InfoData::getServiceAddress() const 
{ 
    return fServiceAddress; 
}

Info::ClassType InfoData::getClassType() const 
{ 
    return fType; 
}

ProfileID InfoData::getProfileId() const
{ 
    return fProfileId; 
}

Input::DataSource& InfoData::getData() 
{ 
    SMART_ASSERT(fData); 
    return *fData; 
}


void InfoData::print(ostream& os) const
{
    os 	<< "("
	<< fTime << ","
	<< fEntityId << ","
        << fServiceAddress << ","
	<< fType << ","
	<< fProfileId << ","
	<< (fData ? fData->str() : "NULL") 
	<< ")" ;
}

} // namespace

