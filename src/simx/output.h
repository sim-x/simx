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
// File:    output.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 13 2005
//
// Description: Manages output functionality
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_OUTPUT
#define NISAC_SIMX_OUTPUT

#include "simx/type.h"
#include "simx/control.h"
#include <fstream>

namespace simx {

class Entity;
class Service;

/// Manages output functionality
namespace Output
{
    /// structure used to output anything
    struct OutputStream 
    {
	~OutputStream();
	std::ofstream fStream; 
    };
	
    /// initializaton function
    void init(const std::string& fileName);
	
    /// returns an OutputStream object that can be used for outputtting things
    OutputStream& output(const Entity&, const OutputRecordType, Control::eSimPhase = Control::kPhaseRun );
    OutputStream& output(const Service&, const OutputRecordType, Control::eSimPhase = Control::kPhaseRun );
		
    //=====================================================================

    /// generic operator<< function for output purposes
    template<class Type>
    Output::OutputStream& operator<<(Output::OutputStream& os, const Type& obj)
    {
	os.fStream << "\t";
	os.fStream << obj;
	return os;
    }
    
} // Output namespace


} // simx namespace

#endif

