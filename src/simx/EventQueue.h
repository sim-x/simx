//--------------------------------------------------------------------------
// File:    EventQueue.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 25 2010
//
// Description: 
//     Event Queue for SimEngine
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_EVENTQUEUE
#define NISAC_SIMX_EVENTQUEUE

#include "simx/type.h"
#include "simx/logger.h"
#include "simx/EventInfo.h"

#include <map>

namespace simx {

/// hold events, can add, top, and pop
class EventQueue
{
    public:
	EventQueue()
	{
	}
	
	// is the queue empty?
	bool empty() const
	{
	    return fQ.empty();
	}
	
	// lets one have look at the top entry
	// MUST NOT BE EMPTY
	const EventInfo& top() const
	{
	    SMART_ASSERT( !fQ.empty() );
	    return fQ.begin()->second;
	}

	// removes the top entry
	// MUST NOT BE EMPTY
	void pop()
	{
	    SMART_ASSERT( !fQ.empty() );
	    fQ.erase( fQ.begin() );
	}
	
	void push( Time when, const EventInfo& e )
	{
	    fQ.insert( std::make_pair( when, e ) );
	}
	
	/// To be invoked at simulation wrap-up.
	/// clears out events from event q
	void finalize() {
	  fQ.clear();
	}


	void print( std::ostream& os) const
	{
	    os 	<< "EventQueue(size=" << fQ.size() << ", "
		<< "top=";
	    if( fQ.empty() )
		os << "EMPTY";
	    else
		os << fQ.begin()->first << ": " << fQ.begin()->second;
	    os << ")";
	}

    protected:
    private:
    
	typedef std::multimap< Time, EventInfo > QueueType;

	// the underlying queue implementation
	QueueType	fQ;
};


//============================================================================

inline std::ostream& operator<<(std::ostream& os, const EventQueue& e)
{
    e.print(os);
    return os;
}


} // namespace
#endif 


