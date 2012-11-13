//--------------------------------------------------------------------------
// File:    InfoRecipient.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Jan 11 2005
//
// Description:
//	Base class for anything capable of receiving an Info message
//	of a particular type
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------
 
#ifndef NISAC_SIMX_INFORECIPIENT
#define NISAC_SIMX_INFORECIPIENT

#include "simx/type.h"

#include "boost/shared_ptr.hpp"

namespace simx {

/// Base class for anything capable of receiving an Info message
/// of a particular type
template<class InfoClass>
class InfoRecipient
{
    public:
	/// method that handles the incoming Info
    virtual ~InfoRecipient() {}
	virtual void receive(boost::shared_ptr<InfoClass>) = 0;
};

//==========================================================================

} // namespace
#endif 

