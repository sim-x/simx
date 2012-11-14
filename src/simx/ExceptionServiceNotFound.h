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
// File:    ExceptionServiceNotFound.h
// Module:  simx
// Author:  Lukas Kroc
// Created: April 15 2005
//
// Description: 
//	Exception raised when a required service is not found
//	on an entity
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_EXCEPTIONSERVICENOTFOUND
#define NISAC_SIMX_EXCEPTIONSERVICENOTFOUND

#include "simx/Exception.h"
#include "simx/type.h"

#include <sstream>

namespace simx {

/// Exception raised when a required service is not found
/// on an entity
class ExceptionServiceNotFound : public Exception
{
    public:
	/// \param entityID where the service was supposed to be
	/// \param address of the non-existend service
	/// \param string representing the type of the service that was expected
	ExceptionServiceNotFound(
	    const EntityID& entId, 
	    const ServiceAddress& addr, 
	    const std::string& type) throw();
	virtual ~ExceptionServiceNotFound() throw();
    
	virtual Level getLevel() const throw();

	virtual const std::string& getDescription() const throw();
    
    private:
	std::string	fDesc;
};

//=========================================================

inline ExceptionServiceNotFound::ExceptionServiceNotFound(
    const EntityID& entId, 
    const ServiceAddress& addr, 
    const std::string& type) throw()
{
    std::stringstream sstr;
    sstr << "cannot find service of type " << type << " on entity " << entId
	<< " at address " << addr;
    fDesc = sstr.str();
}

inline ExceptionServiceNotFound::~ExceptionServiceNotFound() throw()
{
}

inline Exception::Level ExceptionServiceNotFound::getLevel() const throw()
{
    return kERROR;
}

inline const std::string& ExceptionServiceNotFound::getDescription() const throw()
{
    return fDesc;
}


} // simx namespace

#endif

