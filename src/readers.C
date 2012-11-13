//--------------------------------------------------------------------------
// File:    readers.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Apr 11 2005
//
// @@COPYRIGHT@@
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

