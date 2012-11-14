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
// File:    InfoData.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 5 2005
//
// @@
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

