//--------------------------------------------------------------------------
// File:    ServiceData.C
// Module:  simx
// Author:  Lukas Kroc
// Created: November 29 2004
//
// @@COPYRIGHT@@
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
