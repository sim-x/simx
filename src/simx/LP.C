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
// File:    LP.C
// Module:  simx
// Author:  K. Bisset
// Created: June 25 2004
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/LP.h"
#include "simx/DassfLP.h"

#include "simx/EventInfo.h"
#include "simx/DassfEvent.h"
#include "simx/DassfEventInfo.h"
#include "simx/DassfEventInfoManager.h"
#include "simx/DassfPyEventInfoManager.h"
#include "simx/EntityManager.h"
#include "simx/PackedData.h"
#include "simx/logger.h"
#include "simx/config.h"
#include "simx/InfoManager.h"

#include "Common/backtrace.h"

#include "boost/lexical_cast.hpp"

#include "simx/ControlInfoWrapper.h"

#include "simx/simEngine.h"

#ifdef SIMX_USE_PRIME
    #include "simx/Messenger.h"
#endif

#include "simx/simEngine.h"

#include <boost/python.hpp>
#include "simx/PyEventInfoManager.h"
#include "simx/EventInfoManager.h"
#include "simx/constants.h"

using namespace std;
using namespace boost;

#ifdef SIMX_SST
// Original code initializes this static in the main of each application.
// It makes linking with SST more difficult if the static is not self-contained.
// Only initialize here for use with SST to avoid breaking other code.

// Initialize static variable MINDELAY to zero. 
// Correct value will by set by LP constructor from config file.
simx::Time  simx::LP::MINDELAY = 1000;
#endif

namespace simx {

LP::LP(LPID id)
  : fId(id),
#ifdef SIMX_USE_PRIME
    fDassfLP( new DassfLP(id, *this) ),
#endif
    fRandom(id)
{
//  SMART_ASSERT( fDassfLP );

  Config::gConfig.GetConfigurationValueRequired( ky_MINDELAY, MINDELAY );
  Logger::debug3() << "LP.C setting mindelay to " << MINDELAY << endl;
#ifdef SIMX_USE_PRIME
  fDassfLP->mapChannels();
#endif
}

LP::~LP()
{
#ifdef SIMX_USE_PRIME
    SMART_ASSERT( fDassfLP );
    delete fDassfLP;
#endif
}


LPID LP::getId() const 
{
    return fId;
}

Random::TRandom& LP::getRandom() const
{
    return fRandom;
}

Time LP::getNow() const
{
#ifdef SIMX_USE_PRIME
    SMART_ASSERT( fDassfLP );
    return fDassfLP->now();
#else
    return SimEngine::getNow();
#endif
}

void LP::sendEventInfo(EventInfo& e) const
{
    EntityID entityID = e.getDestEntity();
    Time delay = e.getDelay();
    LPID destLP = theEntityManager().findEntityLpId( entityID );
    
    if ( destLP == Control::getRank() )
      {
	if (delay < LOCAL_MINDELAY )
	  {
	    Logger::error() << "LP.C on LP " << getId() << ": too late sending event with delay "
			    << delay << " at time " << getNow() << ", will be delivered later" << endl;
	    Logger::error() << "setting delay to LOCAL_MINDELAY of " << LOCAL_MINDELAY;
	    delay = LOCAL_MINDELAY;
	  }
      }
    else { // detLP is different from this LP
      if( delay < LP::MINDELAY )
	{
     
	  Logger::error() << "LP.C on LP " << getId() << ": too late sending event with delay "
			  << delay << " at time " << getNow() << ", will be delivered later" << endl;
	  Logger::error() << "setting delay to LP::MINDELAY of " << LP::MINDELAY;
	  delay = LP::MINDELAY;
	}
    }
    
    // set the time the event is supposed to execute at
    Time eventTime = getNow() + delay;
    e.setTime(eventTime);
  
    Logger::debug2() << "LP " << fId << ": sending EventInfo: " << e 
	<< " to entity " << entityID << " with delay " << delay << endl;

#ifdef SIMX_USE_PRIME
    fDassfLP->sendDassfEvent(destLP, new DassfEventInfo(e), delay);
#else
    SimEngine::sendEventInfo( destLP, e );
#endif
}

void LP::sendEventInfoManager(EventInfoManager& e) const
{
    LPID destLP = fId;
  
    Logger::debug2() << "LP " << fId << ": sending EventInfoManager: " << e
	<< " with delay " << e.getDelay()  << endl;

    // set the time the event is supposed to execute at
    Time delay = e.getDelay();
    Time eventTime = getNow() + delay;
    e.setTime(eventTime);

#ifdef SIMX_USE_PRIME
    fDassfLP->sendDassfEvent(destLP, new DassfEventInfoManager(e), e.getDelay() );
#else
    // simEngine handles this differently: uses an event to Controller
    // TODO: this should really be happining in InfoManager when it is scheduling this
    //   for itself.
    EventInfo ei;
    ei.setTo( EntityID('!', Control::getRank() ), ServiceAddress() );
    ei.setDelay( delay );
    ei.setTime( eventTime );
    boost::shared_ptr<InfoWakeupInfoManager> info;
    theInfoManager().createInfo( info );
    SMART_ASSERT( info );
    info->fFileId = e.getFileId();
    ei.setInfo( info );
    SimEngine::sendEventInfo( destLP, ei );
#endif
}

//TODO: code repetition from function above. refactor.
void LP::sendPyEventInfoManager(PyEventInfoManager& e) const
{
    LPID destLP = fId;
  
    Logger::debug2() << "LP " << fId << ": sending PyEventInfoManager: " << e
	<< " with delay " << e.getDelay()  << endl;

    // set the time the event is supposed to execute at
    Time delay = e.getDelay();
    Time eventTime = getNow() + delay;
    e.setTime(eventTime);

#ifdef SIMX_USE_PRIME
    fDassfLP->sendDassfEvent(destLP, new DassfPyEventInfoManager(e), e.getDelay() );
#else
    // simEngine handles this differently: uses an event to Controller
    // TODO: this should really be happining in InfoManager when it is scheduling this
    //   for itself.
    EventInfo ei;
    ei.setTo( EntityID('!', Control::getRank() ), ServiceAddress() );
    ei.setDelay( delay );
    ei.setTime( eventTime );
    boost::shared_ptr<InfoWakeupInfoManager> info;
    theInfoManager().createInfo( info );
    SMART_ASSERT( info );
    info->fPyEvent = true;
    ei.setInfo( info );
    SimEngine::sendEventInfo( destLP, ei );
#endif
}

void LP::sendControlInfo( ControlInfoWrapper& cinfo )
{
#ifdef HAVE_MPI_H
#ifdef SIMX_USE_PRIME
  EntityID entityID = cinfo.getDestEntity();
  //Time delay = cinfo.getDelay();
  //TODO: parameter to send directly to LP specified by user.
  LPID destLP = theEntityManager().findEntityLpId( entityID );
  cinfo.setSentTime( fDassfLP->now() );
  cinfo.setSrcLP( fId );
  cinfo.setDestLP( destLP );
  Logger::debug2() << "LP " << fId << ": sending ControlInfo: " << cinfo 
		   << " to entity " << entityID << endl;
  //  bool res;
  if (!( Messenger::sendMessage( fId, destLP, cinfo )) )
    {
      Logger::error() << "simx::LP : Sending ControlInfo failed "
		      << " from Src LP " << fId << " to Dest LP " << destLP
		      << endl;
    }
#else
    Logger::error() << "simx::LP : out-of-band messaging not supported without PRIME" << endl;
#endif
#endif
}

} // namespace


void export_LP() {

  python::class_<simx::LP,boost::noncopyable>("LP",python::no_init)
    .def("get_id",&simx::LP::getId )
    .def("get_now",&simx::LP::getNow )
    ;
}
