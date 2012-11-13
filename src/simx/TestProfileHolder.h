//-------------------------------------------------------------------------
// File:    TestProfileHolder.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Apr 7 2005
// Description:
// 	Unit test of Profileholder
//
// @@COPYRIGHT@@
//
//-------------------------------------------------------------------------

#ifndef NISAC_SIMX_TESTPROFILEHOLDER
#define NISAC_SIMX_TESTPROFILEHOLDER

#include "simx/ProfileHolder.h"

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

namespace simx {

class TestProfileHolder : public CppUnit::TestFixture
{
    public:
	TestProfileHolder();

	void testNonexistent();
	void testWrongType();
	void testBasicType();
	void testContainerType();

    //-------------------------------------------------------------------------

    protected:
	CPPUNIT_TEST_SUITE( TestProfileHolder );
	CPPUNIT_TEST( testNonexistent );
	CPPUNIT_TEST( testWrongType );
	CPPUNIT_TEST( testBasicType );
	CPPUNIT_TEST( testContainerType );
	CPPUNIT_TEST_SUITE_END();

    //-------------------------------------------------------------------------

    private:
	ProfileHolder fProfile;
};

} // namespace

#endif 

