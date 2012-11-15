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
// File:    EventInfo.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Jan 14 2005
//
// Description: 
//	Event for Info deliveries
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_EVENTINFO
#define NISAC_SIMX_EVENTINFO

#include "simx/type.h"
#include "simx/Info.h"
#include "simx/InfoManager.h"
#include "simx/logger.h"

namespace simx {

/// \class EventInfo EventInfo.h "simx/EventInfo.h"
///
/// \brief Event for Info deliveries.
///
/// Holds information about an Info delivery (FROM, TO, WHEN, WHAT)
/// When en event gets send (e.g. LP::sendInfo), the content of fInfo is NOT copied,
/// so changing the content subsequently is undefined (whether the changes get send or not)
class EventInfo
{
    public:
	/// default constructor
	/// default copy constructor and operator= make a *shallow* copy (fInfo ptr is copied)
	EventInfo();
	virtual ~EventInfo();
   

	/// pack EventInfo
        virtual void pack(PackedData&) const;
	/// unpack EventInfo
        virtual void unpack(PackedData&);

        /// Print EventInfo
	virtual void print(std::ostream& os) const;

        /// Execute operation
        virtual void execute(void);

    // GETTERS:
        // TO:
        const EntityID& getDestEntity() const;
        const ServiceAddress& getDestService() const;
        // WHEN:
        Time getDelay() const;
	Time getTime() const;

    // SETTERS:
        // TO:
        void setTo(const EntityID entId, const ServiceAddress servAddr);
        // WHEN:
	void setDelay(const Time time);
	void setTime(const Time time);
        // WHAT:
        void setInfo(const boost::shared_ptr<const Info> info);	///< info must not be NULL

    protected:
    private:
	// TO:
	EntityID	fDestEntity;	///< Destination entityID
	ServiceAddress fDestService;	///< Destination servicel address
  
	// WHEN:
	Time		fDelay;		///< time delay when event is to be delivered
	Time      fTime;      /// The time the event will get executed
	
	// WHAT:
	/// the fInfo is boost::shared_ptr<*const*> because we want to be able so send such objects
	/// but it will have to be const_casted in execute
	/// (but by that time, we make sure that we only have one pointer to the Info,
	/// so the const_cast is safe)
	boost::shared_ptr<const Info>	fInfo;		///< actual information send to the dest 

};


//============================================================================

inline std::ostream& operator<<(std::ostream& os, const EventInfo& e)
{
    e.print(os);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const EventInfo* ep)
{
    if(ep) { ep->print(os); } 
    else  { os << "NULL"; }
    return os;
}

} // namespace
#endif 


