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
// @@COPYRIGHT@@
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

