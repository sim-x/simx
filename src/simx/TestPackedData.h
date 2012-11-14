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

//-------------------------------------------------------------------------
// File:    TestPackedData.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Apr 7 2005
// Description:
// 	Unit test of Profileholder
//
// @@
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

