//--------------------------------------------------------------------------
// File:    DassfEvent.h
// Module:  simx
// Author:  K. Bisset
// Created: June 25 2004
//
// Description:
//	Base for all DassfEvents
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_EVENT
#define NISAC_SIMX_EVENT

#include "simx/type.h"
#include "simx/PackedData.h"

#ifdef SIMX_USE_PRIME

#include "ssf.h"

#include <iosfwd>

namespace simx {
class LP;

/// \class DassfEvent DassfEvent.h "simx/DassfEvent.h"
///
/// \brief Base for all DassfEvents
///
/// 
class DassfEvent : public prime::ssf::Event
{
    public:
	/// Default constructor
	DassfEvent();
	/// Copy constructor (required)
	explicit DassfEvent(const DassfEvent&);
	/// Construct from PackedData
	explicit DassfEvent(prime::ssf::ssf_compact*);

	/// Destroy an DassfEvent
	virtual ~DassfEvent();

	/// Copy an DassfEvent (required)
	virtual prime::ssf::Event* clone() const = 0;
	
	/// Pack an DassfEvent (required)
	virtual prime::ssf::ssf_compact* pack();
	virtual prime::ssf::ssf_compact* pack(prime::ssf::ssf_compact*);
	/// Unpack an DassfEvent (required) - MUST be overriden
	static prime::ssf::Event* unpack(prime::ssf::ssf_compact*);

	/// Print an event (empty in DassfEvent base)
	virtual void print(std::ostream& os) const;

	/// called when event is executed at destination at the proper time
	virtual void execute(LP& lp) = 0;

    protected:
    private:
};

//===================================================================================

} // namespace

#endif // SIMX_USE_PRIME

#endif // NISAC_SIMX_EVENT

