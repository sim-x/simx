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
// File:    Exception.h
// Module:  simx
// Author:  Lukas Kroc
// Created: April 15 2005
//
// Description: 
//	Base for all exceptions in the simx system
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_EXCEPTION
#define NISAC_SIMX_EXCEPTION

#include <string>

namespace simx {

class Exception
{
    public:
	/// how serious the exception is:
	/// INFO = just informative, may well happen
	/// WARN = shouldn't happen, but not a big deal if it does, and should work
	/// ERROR = really shouldn't happen, but the rest may somewhat work
	/// FATAL = cannot even function after this
	enum Level { kINFO, kWARN, kERROR, kFATAL };
    
	Exception() throw();
	virtual ~Exception() throw() = 0;
    
	/// retrieves the level of the exception, as described above
	virtual Level getLevel() const throw() = 0;
	/// gets a human-readable description of the exception
	virtual const std::string& getDescription() const throw() = 0;
};

//=========================================================

inline Exception::Exception() throw()
{
}

inline Exception::~Exception() throw()
{
}

} // simx namespace

#endif

