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
// $Id: InfoManager.h,v 1.19 2010/08/25 17:45:58 kroc Exp $
//--------------------------------------------------------------------------
// File:    InfoManager.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Jan 11 2005
//
// Description: Registeres and create Infos
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_INFOMANAGER
#define NISAC_SIMX_INFOMANAGER

#include "simx/control.h"
#include "simx/type.h"
#include "simx/InfoData.h"
#include "simx/InputHandler.h"
#include "simx/InfoHandler.h"
#include "simx/logger.h"
#include "simx/Info.h"

#include "loki/Singleton.h"
#include "loki/AssocVector.h"
#include "loki/TypeInfo.h"

#include "Common/Exception.h"

// TODO (python). can we make infomanager oblivious to python?
#include "simx/Python/PyInfoData.h"

namespace simx {

  class LP;
  class InfoHandler;


    /// wakes up manager when time comes to read more input
    struct InfoWakeupInfoManager : public Info
    {
	int	fFileId;	///< file id that has new data available
	// pack and unpack these are NOT needed here, as the info must never travel to a different LP

	virtual void pack(PackedData&) const;
	virtual void unpack(PackedData&);
    };


  /// Registeres and create Infos
  class InfoManager
  {
    friend InfoManager& theInfoManager();
    friend class Loki::CreateUsingNew<InfoManager>;

  public:
    /// function to register Info types, to be done in .C files for the derived Info objects
    /// - if ClassType is not specified, then it cannot be read from input, otherwise it's ok
    template<class InfoClass> void registerInfo( const Info::ClassType&  = Info::ClassType() );

    /// functions creating a new Info
    /// if returns, the pointer is NOT 0 (convenient, since we don't have to test for it all the time)
    /// if it fails, it does ASSERT
    void createInfo(Info::ClassType type, boost::shared_ptr<Info>&) const;
    template<class InfoClass> void createInfo(boost::shared_ptr<InfoClass>&) const;

    /// creates a duplicate of a given Info (uses the Infos copy constructor)
    /// copy will always be != 0
    /// \param Original Info
    /// \param Destination pointer (where the copy should be made)
    template<class InfoClass> void duplicateInfo(const InfoClass& orig, boost::shared_ptr<InfoClass>& copy) const;
    /// the same for copying from boost::shared_ptr<const InfoClass>
    template<class InfoClass> void duplicateInfo(const InfoClass& orig, boost::shared_ptr<const InfoClass>& copy) const;

    /// returns a handler for a particular info type
    /// should only be used by InfoHandlerWrapper and internally
    template<class InfoClass> const InfoHandler& getInfoHandler() const;

    /// prepares to read from input files <Time> <EntityID> <ServiceAddr> <InfoType> <info data .....>
    /// only schedules those events as have destinations in one of services on one of LPs in this Unix process
    void prepareDataFiles(const std::string& infoFiles);

    /// reads records from file at filId position until some time in the input is reached
    void readDataFile(int fileId);

    /// [11/19/2008 by Guanhua Yan] reads all info records from the given files
    void readFullDataFile(const std::string& infoFiles );

    /// creates an event info from input data
    void createInfoFromInfoData( InfoData& );

    /// creates a python event info from python input.
    // TODO (Python) can we make infomanager oblivious to python?
    void createPyInfoFromInfoData( Python::PyInfoData& data );

    /// provides access to InputHandler for Infos.... to be able to create infos from files in Controller
    InputHandler<Info::ClassType>& getInputHandler() { return fInputHandler; }


    //TODO (Python, high): relocate
    const boost::python::object& getPacker() const;
    const boost::python::object& getUnpacker() const;
    boost::python::object fPickler, fPacker, fUnpacker;

  protected:
  private:

    //TODO (Python, high): relocate

    

    /// factory to create info objects (NOT a loki factory)
    typedef Loki::AssocVector<Info::ClassType, boost::shared_ptr<const InfoHandler> >	InfoHandlerMap;
    InfoHandlerMap		fInfoHandlerMap;
    typedef Loki::AssocVector<Loki::TypeInfo, boost::shared_ptr<const InfoHandler> >	InfoHandlerMapTypeInfo;	
    //TODO: uses "const std::type_info" which may not be unique, possible cause of problems??
    InfoHandlerMapTypeInfo	fInfoHandlerMapTypeInfo;
	
    /// produces Inputs that are Infos created from file
    InputHandler<Info::ClassType>	fInputHandler;
	
    typedef std::vector<InfoData::Reader*> FileReaderContainer;
    FileReaderContainer 			fFileVector;	///< all info data input files

    Info::ClassType fTypeCounter;		///< counter for automatic Info identification

    /// private so that only singleton (friend) can create it
    InfoManager();
    ~InfoManager();
    /// unimplemented
    InfoManager(const InfoManager&);
    InfoManager& operator=(const InfoManager&);
	
    /// singleton object holding the InfoManager
    typedef Loki::SingletonHolder<InfoManager,
				  Loki::CreateUsingNew,
				  Loki::DefaultLifetime> sInfoManager;

  };


  inline InfoManager& theInfoManager()
  {
    return InfoManager::sInfoManager::Instance();
  }

} //namespace

//======================================================================================

// is here to solve problem with circular inclusion
#include "simx/InfoHandlerImpl.h"

namespace simx {

  template<class InfoClass>
  void InfoManager::registerInfo( const Info::ClassType& userType )
  {
    //TODO: add a check (compile time at best) that Type is derived from Info

    /// check if the given userType is correct
    if( userType < 0 )
    {
      std::cerr << "ERROR: InfoManager: user info type '" << userType
		<< "' is invalid, must be positive" << std::endl;
      throw Common::Exception("Error info registration");
    }

    /// give it an automatic type if user did not specify
    const Info::ClassType type = (userType != Info::ClassType() ) ? userType : fTypeCounter--;
    
    /// register the type
    if( fInfoHandlerMap.find(type) != fInfoHandlerMap.end() )
    {
      std::cerr << "ERROR: InfoManager: info '" << type
		<< "' is already registered" << std::endl;
      throw Common::Exception("Error info registration");
    }

    /// register the type_info of the object
    const Loki::TypeInfo typeInfo( typeid(InfoClass) );
    if( fInfoHandlerMapTypeInfo.find(typeInfo) != fInfoHandlerMapTypeInfo.end() )
    {
      std::cerr << "ERROR: InfoManager: type_info '" << typeInfo.name()
		<< "' is already registered" << std::endl;
      throw Common::Exception("Error info registration");
    }

    // register as the Input object: (needs to be put into the wrapper to have getInfoHandler method implemented)
    fInputHandler.registerInput<InfoHandlerWrapper<InfoClass> >(type);

    const boost::shared_ptr<const InfoHandler> handler( new InfoHandlerDerived<InfoClass>(type) );
    
    // insert into Info::ClassType -> handler map
    fInfoHandlerMap.insert( InfoHandlerMap::value_type(type, handler) );
    // insert into TypeInfo -> handler map
    fInfoHandlerMapTypeInfo.insert( InfoHandlerMapTypeInfo::value_type(typeInfo, handler) );

  }

  template<class InfoClass> 
  void InfoManager::createInfo(boost::shared_ptr<InfoClass>& ptr) const
  {
    const InfoHandler& handler = getInfoHandler<InfoClass>();

    ptr = boost::dynamic_pointer_cast<InfoClass>( handler.create() );
    SMART_ASSERT( ptr );
  }

  template<class InfoClass> 
  void InfoManager::duplicateInfo(const InfoClass& source, boost::shared_ptr<InfoClass>& copy) const
  {
      const InfoHandler& handler = source.getInfoHandler();
      copy = boost::dynamic_pointer_cast<InfoClass>( handler.create(source) );
      SMART_ASSERT( copy );
  }

  template<class InfoClass> 
  void InfoManager::duplicateInfo(const InfoClass& source, boost::shared_ptr<const InfoClass>& copy) const
  {
      const InfoHandler& handler = source.getInfoHandler();
      copy = boost::dynamic_pointer_cast<const InfoClass>( handler.create(source) );
      SMART_ASSERT( copy );
  }


  template<class InfoClass> 
  const InfoHandler& InfoManager::getInfoHandler() const
  {
    InfoHandlerMapTypeInfo::const_iterator iter = fInfoHandlerMapTypeInfo.find( typeid(InfoClass) );
    SMART_VERIFY( iter!=fInfoHandlerMapTypeInfo.end() )
			( typeid(InfoClass).name() )( fInfoHandlerMap )
			.msg("InfoManager: requested a handler for unregistered Info class");
    const boost::shared_ptr<const InfoHandler> handler( iter->second );
    SMART_ASSERT( handler )( typeid(InfoClass).name() )( fInfoHandlerMap );
    ///TODO: add throwing if InfoHandler not found
    
    return *handler;
  }

} // namespace

//--------------------------------------------------------------------------
#endif // NISAC_SIMX_INFOMANAGER
//--------------------------------------------------------------------------
