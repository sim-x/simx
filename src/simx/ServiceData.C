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
// File:    ServiceData.C
// Module:  simx
// Author:  Lukas Kroc
// Created: November 29 2004
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/ServiceData.h"
#include "simx/type.h"
#include "simx/readers.h"
#include "simx/logger.h"

#include <iostream>

using namespace std;

namespace simx {

ServiceData::ServiceData()
    : 	fName(),
	fClassType(),
	fProfileId(),
	fData()
{
}

ServiceData::~ServiceData()
{
}

void ServiceData::read(istream& is)
{
    fData.reset(new stringstream());
    is 	>> fName
	>> fClassType
        >> fProfileId;
    if( is.good() )
    {
	// only read custom data if there is any
	is >> fData->rdbuf();
    }
    SMART_VERIFY( !is.fail() )( is.rdstate() );	// make sure that read-in was correct

    if( fName==ServiceName() )
    {
	Logger::warn() << "ServiceData: ServiceName id==" << ServiceName() 
	    << " is invalid" << std::endl;
    }
}

ServiceName ServiceData::getName() const 
{
    return fName;
}

Service::ClassType ServiceData::getClassType() const 
{
    return fClassType;
}

ProfileID ServiceData::getProfileId() const 
{
    return fProfileId;
}

Input::DataSource& ServiceData::getData() const 
{
    SMART_ASSERT(fData); 
    return *fData;
}


void ServiceData::print(ostream& os) const
{
    os 	<< "("
	<< fName << ","
	<< fClassType << ","
        << fProfileId << ","
	<< (fData ? fData->str() : "NULL") 
	<< ")";
}

} // namespace
