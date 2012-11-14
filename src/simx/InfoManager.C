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
// File:    InfoManager.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Jan 11 2005
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/InfoManager.h"
#include "simx/Info.h"
#include "simx/InfoData.h"
#include "simx/InfoHandler.h"
#include "simx/EventInfo.h"
#include "simx/LP.h"
#include "simx/EntityManager.h"
#include "simx/EventInfoManager.h"
#include "simx/writers.h"
#include "simx/constants.h"


#include <sstream>

using namespace std;
using namespace simx::Python;

namespace simx {

//====================================================================
// InfoWakeupInfoManager

void InfoWakeupInfoManager::pack(PackedData& dp) const
{
    Logger::error() << "InfoWakeupInfoManager::pack should not be called!";
}


void InfoWakeupInfoManager::unpack(PackedData& dp)
{
    Logger::error() << "InfoWakeupInfoManager::unpack should not be called!";
}


//====================================================================
// InfoManager



InfoManager::InfoManager()
    : 	fInputHandler("InfoProfile"),
	fTypeCounter( -1 )
{
//CANNOT    Logger::debug1() << "ServiceManager: in constructor" << endl;
  //TODO (Python, high) relocate
  //  Py_Initialize();
  fPickler = boost::python::import("cPickle");
  // fPickler = boost::python::import("marshal");
  //fPickler = boost::python::import("json");
  fPacker = fPickler.attr("dumps");
  fUnpacker = fPickler.attr("loads");
}

  const boost::python::object& InfoManager::getPacker() const {

    return fPacker;
  }

  const boost::python::object& InfoManager::getUnpacker() const {

    return fUnpacker;
  }


InfoManager::~InfoManager()
{
//CANNOT    Logger::debug1() << "ServiceManager: in destructor" << endl;

    /// destruct the file readers
    for(FileReaderContainer::iterator iter = fFileVector.begin();
	iter != fFileVector.end();
	++iter)
    {
	SMART_ASSERT( *iter );
	delete *iter;
    }
}

void InfoManager::createInfo(Info::ClassType type, boost::shared_ptr<Info>& ptr) const
{
    InfoHandlerMap::const_iterator iter = fInfoHandlerMap.find( type );
    if( iter == fInfoHandlerMap.end() )
    {
	Logger::error() << "InfoManager: cannot create unregistered info of type "
	    << type << ", registered infos: "
	    << fInfoHandlerMap << std::endl;
	SMART_VERIFY(false)("InfoManager: cannot create unregistered info");
	///TODO: add throwing instead of ASSERT
    }
    boost::shared_ptr<const InfoHandler> handler( iter->second );
    SMART_ASSERT( handler )( type )( fInfoHandlerMap );

    ptr = handler->create();
    SMART_ASSERT( ptr );
}

/// prepares all input files
void InfoManager::prepareDataFiles(const std::string& infoFiles)
{
    Logger::debug2() << "InfoManager: in prepareDataFiles, infoFiles= '" << infoFiles << "'" << endl;
    const Control::LpPtrMap& lpMap = Control::getLpPtrMap();
    Logger::debug3() << "InfoManager: 	lps= " << lpMap << endl;

    if( lpMap.empty() )
    {
	// if there are no LPs, do nothing
	return;
    }
    SMART_ASSERT( !lpMap.empty() );

    stringstream sstr;
    sstr << infoFiles;
    string fileName;
    // for each file, setup a reader
    while( sstr >> fileName )
    {
	fFileVector.push_back( new InfoData::Reader(fileName) );
    }
    if( fFileVector.size() == 0 )
    {
	Logger::warn() << "InfoManager: there were no info files found" << endl;
    }
    
    Logger::debug2() << "InfoManager: opened " << fFileVector.size() << " info files" << endl;

    // now start reading them all
    for(unsigned fid = 0; fid < fFileVector.size(); ++fid)
    {
	readDataFile( fid );
    }
}

/// reads info from input file <Time> <EntityID> <ServiceAddr> <InfoType> <info data .....>
/// until LP::getNow()+TIMECHUNK+MINDELAY is reached and schedules itself again for
/// LP::getNow()+TIMECHUNK
void InfoManager::readDataFile( int fid )
{
    /// these variables should be static because they will be reused may times
    static const Control::LpPtrMap& lpMap = Control::getLpPtrMap();
    SMART_ASSERT( lpMap.begin() != lpMap.end() );
    SMART_ASSERT( lpMap.begin()->second );
    static const LP& lp = *(lpMap.begin()->second); // get address of ANY lp on this computer node

    const Time TIMECHUNK = 1000*LOCAL_MINDELAY;	///< how much time ahead will it schedule events
    Time now = lp.getNow();
    
    Logger::debug3() << "InfoManager: in readDataFile at time " << now
	<< ", fid= " << fid << endl;

    SMART_ASSERT( fid >= 0);
    SMART_ASSERT( static_cast<unsigned>(fid) < fFileVector.size() );
    SMART_ASSERT( fFileVector[fid] );

    Time upToTime = now + TIMECHUNK;
    InfoData::Reader& reader = *fFileVector[fid];
    while ( reader.MoreData() && reader.ViewNextData().getTime() <= upToTime )
      {
        InfoData data( reader.ReadData() );
	createInfoFromInfoData( data );
	
	
      } // while(MoreData)
    
    if( reader.MoreData() )
      {
	// now schedule itself a wake-up just before the next info event expires:
	Time delay = max (LOCAL_MINDELAY, reader.ViewNextData().getTime() - now - 3*LOCAL_MINDELAY);
	EventInfoManager eventMng( fid, delay );
	lp.sendEventInfoManager( eventMng );
      }
}


  void InfoManager::createInfoFromInfoData( InfoData& data ) {

    static const Control::LpPtrMap& lpMap = Control::getLpPtrMap();
    static const LP& lp = *(lpMap.begin()->second); // get address of ANY lp on this computer node
    Time now = lp.getNow();
    // see if we will send the info:
    LPID lpId = theEntityManager().findEntityLpId( data.getEntityId() );
    Control::LpPtrMap::const_iterator lpIter = lpMap.find(lpId);
    if( lpIter != lpMap.end() )
      {
	// the destination resides on one of LPs in this Unix process
	// send the info:
	Logger::debug3() << "InfoManager: processing data: " << data << endl;

	// get info object and fill it with data
	boost::shared_ptr<Input> input( fInputHandler.createInput( data.getClassType(), data.getProfileId(), data.getData() ) );
	boost::shared_ptr<Info> info = boost::dynamic_pointer_cast<Info>(input);
	SMART_ASSERT( info );	/// the object MUST actually be a descendant of Info

	Logger::debug3() << "InfoManager: processing info: " << info << endl;
	
	// set EventInfo parameters:
	EventInfo event;
	event.setTo( data.getEntityId(), data.getServiceAddress() );
	if( data.getTime() < now )
	  Logger::warn() << "InfoManager: Sending event at time " << now
			 << " which should have been sent at " << data.getTime() 
			 << " : Check if input is time sorted " << endl;
	event.setDelay( max (LOCAL_MINDELAY, data.getTime() - now ));
	event.setInfo( info );
	// send it off directly to the right LP
	LP* lpPointer = lpIter->second;
	SMART_ASSERT( lpPointer );
	lpPointer->sendEventInfo(event);
	
      } // else don't schedule it, other Unix process will
  }

  void InfoManager::createPyInfoFromInfoData( PyInfoData& data ) {

    static const Control::LpPtrMap& lpMap = Control::getLpPtrMap();
    static const LP& lp = *(lpMap.begin()->second); // get address of ANY lp on this computer node
    Time now = lp.getNow();
    // see if we will send the info:
    LPID lpId = theEntityManager().findEntityLpId( data.getEntityId() );
    Control::LpPtrMap::const_iterator lpIter = lpMap.find(lpId);
    if( lpIter != lpMap.end() )
      {
	// the destination resides on one of LPs in this Unix process
	// send the info:
	Logger::debug3() << "InfoManager: processing data: " << data << endl;

	// get info object and fill it with data
	boost::shared_ptr<Input> input( fInputHandler.createInput( data.getClassType(), data.getProfileId(), data.getProfile(), data.getData() ) );
	boost::shared_ptr<Info> info = boost::dynamic_pointer_cast<Info>(input);
	SMART_ASSERT( info );	/// the object MUST actually be a descendant of Info

	Logger::debug3() << "InfoManager: processing info: " << info << endl;
	
	// set EventInfo parameters:
	EventInfo event;
	event.setTo( data.getEntityId(), data.getServiceAddress() );
	if( data.getTime() < now )
	  Logger::warn() << "InfoManager: Sending event at time " << now
			 << " which should have been sent at " << data.getTime() 
			 << " : Check if input is time sorted " << endl;
	event.setDelay( max (LOCAL_MINDELAY, data.getTime() - now ));
	event.setInfo( info );
	// send it off directly to the right LP
	LP* lpPointer = lpIter->second;
	SMART_ASSERT( lpPointer );
	lpPointer->sendEventInfo(event);
	
      } // else don't schedule it, other Unix process will
  }



/// [11/19/2008 by Guanhua Yan. ]
///
/// reads info from input file <Time> <EntityID> <ServiceAddr>
/// <InfoType> <info data .....> without any time constraints. Open a
/// file and then read all infos.
///≈
void InfoManager::readFullDataFile(const std::string& infoFiles )
{
  assert(false);
    /// these variables should be static because they will be reused may times
    static const Control::LpPtrMap& lpMap = Control::getLpPtrMap();
    SMART_ASSERT( lpMap.begin() != lpMap.end() );
    SMART_ASSERT( lpMap.begin()->second );
    static const LP& lp = *(lpMap.begin()->second); // get address of ANY lp on this computer node
    Time now = lp.getNow();
    
    Logger::debug3() << "InfoManager: in readFullDataFile at time " << now
		     << ", infoFiles= " << infoFiles << endl;

    stringstream sstr;
    sstr << infoFiles;
    string fileName;
    // for each file, setup a reader
    while( sstr >> fileName )
    {
      InfoData::Reader reader(fileName);
      while ( reader.MoreData()) {
        InfoData data( reader.ReadData() );
	
	// see if we will send the info:
	LPID lpId = theEntityManager().findEntityLpId( data.getEntityId() );
	Control::LpPtrMap::const_iterator lpIter = lpMap.find(lpId);
        if( lpIter != lpMap.end() )
	  {
	    // the destination resides on one of LPs in this Unix process
	    // send the info:
	    Logger::debug3() << "InfoManager: processing data: " << data << endl;
	    
	    // get info object and fill it with data
	    boost::shared_ptr<Input> input( fInputHandler.createInput( data.getClassType(), data.getProfileId(), data.getData() ) );
	    boost::shared_ptr<Info> info = boost::dynamic_pointer_cast<Info>(input);
	    SMART_ASSERT( info );	/// the object MUST actually be a descendant of Info

	    Logger::debug3() << "InfoManager: processing info: " << info << endl;

	    // set EventInfo parameters:
	    EventInfo event;
	    event.setTo( data.getEntityId(), data.getServiceAddress() );
	    event.setDelay( max (LOCAL_MINDELAY, data.getTime() - now ));
	    event.setInfo( info );
	    // send it off directly to the right LP
	    LP* lpPointer = lpIter->second;
	    SMART_ASSERT( lpPointer );
	    lpPointer->sendEventInfo(event);

	  } // else don't schedule it, other Unix process will

      } // while(MoreData)
    }
}

} // namespace

