//-------------------------------------------------------------------------
// File:    TestPackedData.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Apr 7 2005
// Description:
// 	Unit test of Profileholder
//
// @@COPYRIGHT@@
//
//-------------------------------------------------------------------------

#ifndef NISAC_SIMX_TESTPACKEDDATA
#define NISAC_SIMX_TESTPACKEDDATA

#include "simx/PackedData.h"

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

namespace simx {

class TestPackedData : public CppUnit::TestFixture
{
    public:
	TestPackedData();

	void testBasicType();
	void testContainerType();

    //-------------------------------------------------------------------------

    protected:
	CPPUNIT_TEST_SUITE( TestPackedData );
	CPPUNIT_TEST( testBasicType );
	CPPUNIT_TEST( testContainerType );
	CPPUNIT_TEST_SUITE_END();

    //-------------------------------------------------------------------------

    private:
	PackedData fPackedData;
};

} // namespace

#endif 

