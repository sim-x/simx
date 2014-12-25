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
// File:    EventQueue.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 25 2010
//
// Description: 
//     Event Queue for SimEngine
//
// @@
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
	  fNumEvents = 0;
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
	    fNumEvents++;
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

  uint64_t getNumEvents() const
  {
    return fNumEvents;
  }
  

    protected:
    private:
    
	typedef std::multimap< Time, EventInfo > QueueType;
	
	// the underlying queue implementation
	QueueType	fQ;
  //unsigned int fNumEvents;
  uint64_t fNumEvents;
};


//============================================================================

inline std::ostream& operator<<(std::ostream& os, const EventQueue& e)
{
    e.print(os);
    return os;
}


} // namespace
#endif 


