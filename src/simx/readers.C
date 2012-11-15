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
// File:    readers.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Apr 11 2005
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/readers.h"
#include "simx/userIO.h"
#include "simx/logger.h"

#include "boost/lexical_cast.hpp"

namespace simx {

//=======================================================================================
// simx::read-in functions

std::istream& operator>>(std::istream& is, ServiceAddress& addr)
{
    std::string item;
    is >> item;
    
    if( !UserIO::getValue(item, addr) )
    {   
	/// it is not in the user map
	/// try and see if it's a number
	try 
	{
	    // lexical_cast also uses operator>>, so we cannot use it directly
	    addr = static_cast<ServiceAddress>( boost::lexical_cast<int>(item) );
	}
	catch(boost::bad_lexical_cast &)
	{
	    Logger::error() << "Cannot convert '" << item << "' to a ServiceAddress" << std::endl;
	}
    }
    
    return is;
}

//======================================================================================


} // namespace

