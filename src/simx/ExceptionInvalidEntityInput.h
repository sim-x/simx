//--------------------------------------------------------------------------
// File:    ExceptionInvalidEntityInput.h
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

#ifndef NISAC_SIMX_EXCEPTIONINVALIDENTITYINPUT
#define NISAC_SIMX_EXCEPTIONINVALIDENTITYINPUT

#include "simx/Exception.h"
#include "simx/type.h"

#include <sstream>

namespace simx {

/// Exception raised when a required service is not found
/// on an entity
class ExceptionInvalidEntityInput : public Exception
{
    public:
	/// \param entityID where the service was supposed to be
	/// \param address of the non-existend service
	/// \param string representing the type of the service that was expected
	ExceptionInvalidEntityInput() throw();
	virtual ~ExceptionInvalidEntityInput() throw();
    
	virtual Level getLevel() const throw();

	virtual const std::string& getDescription() const throw();
    
    private:
	std::string	fDesc;
};

//=========================================================

inline ExceptionInvalidEntityInput::ExceptionInvalidEntityInput() throw()
    : fDesc("Cannot convert Entity input to expected type")
{
}

inline ExceptionInvalidEntityInput::~ExceptionInvalidEntityInput() throw()
{
}

inline Exception::Level ExceptionInvalidEntityInput::getLevel() const throw()
{
    return kERROR;
}

inline const std::string& ExceptionInvalidEntityInput::getDescription() const throw()
{
    return fDesc;
}


} // simx namespace

#endif

