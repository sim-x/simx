//--------------------------------------------------------------------------
// File:    Exception.h
// Module:  simx
// Author:  Lukas Kroc
// Created: April 15 2005
//
// Description: 
//	Base for all exceptions in the simx system
//
// @@COPYRIGHT@@
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

