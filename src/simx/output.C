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
// File:    output.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 9 2005
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/output.h"
#include "simx/Entity.h"
#include "simx/Service.h"
#include "simx/logger.h"

#include <fstream>

using namespace std;

namespace {
    using namespace simx;

    /// where stuff will be output (just one for now for all OutputRecordTypes)
    Output::OutputStream	fOutputStream;

} //namespace

namespace simx {

namespace Output {

    void init(const std::string& fileName)
    {
        Logger::debug2() << "Output: opening OutputStream '" << fileName << "'" << endl;
        SMART_ASSERT( !fOutputStream.fStream.is_open() );

        // try to open the file
        fOutputStream.fStream.open(fileName.c_str());
        
        if( !fOutputStream.fStream )
        {
    	    Logger::error() << "Output: cannot open OutputStream '" 
    	        << fileName << "', will output to /dev/null" << endl;
	    fOutputStream.fStream.clear();
	    fOutputStream.fStream.open("/dev/null");
        } else
        {
    	    fOutputStream.fStream << "Time\tEntityId\tServiceName\tType\tCustomData";
        }
	SMART_ASSERT( fOutputStream.fStream.good() )( fileName ).msg("failed to open output file");
    }

    OutputStream& output(const Entity& ent, const OutputRecordType type, Control::eSimPhase phase)
    {
	///NOTE: could possibly have different output streams for different types
	OutputStream& os = fOutputStream;
   if(!fOutputStream.fStream.is_open()) {
      Logger::failure("simx::Output: not initialized");
   }
   else {
     os.fStream 	<< endl;
     if ( phase != Control::kPhaseRun )
       {
	 if ( phase == Control::kPhaseWrapUp )
	   //TODO: get initial and final time
	   os.fStream << "FINAL";
	 else
	   os.fStream << "INIT";
	       
       }
     else
       { 
	 os.fStream << ent.getNow();
       }
     os.fStream << "\t" 
		<< ent.getId() 
		<< "\t" 
		<< 0 
		<< "\t" 
		<< type;
   }
   return os;
    }

    OutputStream& output(const Service& serv, const OutputRecordType type, Control::eSimPhase phase )
    {
	///NOTE: could possibly have different output streams for different types
	OutputStream& os = fOutputStream;
   if(!fOutputStream.fStream.is_open()) {
      Logger::failure("simx::Output: not initialized");
   }
   else {   
     os.fStream 	<< endl;
     if ( phase != Control::kPhaseRun )
       {
	 if ( phase == Control::kPhaseWrapUp )
	   //TODO: get initial and final time
	   os.fStream << "FINAL";
	 else
	   os.fStream << "INIT";
	 
       }
     else
       { 
	 os.fStream << serv.getNow();
       }
     os.fStream	<< "\t" 
			<< serv.getEntityId() 
			<< "\t" 
		<< serv.getName() 
			<< "\t" 
			<< type;
   }
	return os;
    }


    ///========================================================
    OutputStream::~OutputStream()
    {
      //Logger::debug2() << "Output Destructor" << endl;
	if( fStream )
	{
	    fStream << endl;
	}
	fStream.close();
    }

} // Output namespace

} // simx namespace
