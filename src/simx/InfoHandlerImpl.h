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
// $Id: InfoHandlerImpl.h,v 1.15 2008/10/30 19:53:15 kroc Exp $
//--------------------------------------------------------------------------
// File:    InfoHandlerImpl.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 12 2005
//
// Description: 
//	implementation of functions in InfoHandler
//	Exists because of circular inclusion with InfoManager.h
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_INFOHANDLERIMPL
#define NISAC_SIMX_INFOHANDLERIMPL

#include "simx/InfoHandler.h"
#include "simx/InfoManager.h"
#include "simx/InfoRecipient.h"
#include "simx/ControlInfoRecipient.h"
#include "simx/Service.h"

namespace simx {


  //================================================================
  // InfoHandlerWrapper

  template<class InfoClass>
  InfoHandlerWrapper<InfoClass>::InfoHandlerWrapper() 
    : InfoClass() 
  {
  }

  template<class InfoClass>
  InfoHandlerWrapper<InfoClass>::InfoHandlerWrapper(const InfoClass& x)
    : InfoClass(x) 
  {
  }

  template<class InfoClass>
  InfoHandlerWrapper<InfoClass>::~InfoHandlerWrapper() 
  {
  }

  template<class InfoClass>
  const InfoHandler& InfoHandlerWrapper<InfoClass>::getInfoHandler() const 
  {
    static const InfoHandler& ih = theInfoManager().getInfoHandler<InfoClass>();
    return ih;
  }

  //================================================================
  // InfoHandler

  inline InfoHandler::~InfoHandler()
  {
  }

  //================================================================
  // InfoHandlerDerived

  template<class InfoClass>
  InfoHandlerDerived<InfoClass>::InfoHandlerDerived(const Info::ClassType type)
    :	fClassType( type ) 
  {
  }

  template<class InfoClass>
  InfoHandlerDerived<InfoClass>::~InfoHandlerDerived()
  {
  }


  template<class InfoClass>
  boost::shared_ptr<Info> InfoHandlerDerived<InfoClass>::create() const
  {
    return boost::shared_ptr<Info>( new InfoHandlerWrapper<InfoClass>() );
  }

  template<class InfoClass>
  boost::shared_ptr<Info> InfoHandlerDerived<InfoClass>::create(const Info& source) const
  {
    const InfoClass& src = dynamic_cast<const InfoClass&>(source);
    return boost::shared_ptr<Info>( new InfoHandlerWrapper<InfoClass>(src) );
  }

  template<class InfoClass>
  void InfoHandlerDerived<InfoClass>::execute(boost::shared_ptr<Service> baseService, boost::shared_ptr<Info> baseInfo, bool doError) const
  {
    SMART_ASSERT( baseService );
    boost::shared_ptr<InfoRecipient<InfoClass> > recipient = 
      boost::dynamic_pointer_cast<InfoRecipient<InfoClass> >(baseService);
    if( !recipient )
    {
      if( doError )
      {
	Logger::warn() << "InfoHandler: invalid recipient, expected " 
		       << typeid(InfoRecipient<InfoClass>).name() 
		       << ", received " << typeid(*baseService).name() << std::endl;
      }
      // else only a trial was made, and it failed
      return;
    }
	    
    boost::shared_ptr<InfoClass> info = boost::dynamic_pointer_cast<InfoClass>(baseInfo);
    if( !info )
    {
      Logger::warn() << "InfoHandler: invalid info, expected " << typeid(boost::shared_ptr<InfoClass>).name() 
		     << ", received " << typeid(baseInfo).name() << std::endl;
      return;
    }
    baseInfo.reset();	///< so that it no longer points to the Info

    SMART_ASSERT( recipient );
    SMART_ASSERT( info );

    /// we want the receive Arg to be the only Ptr to the Info if possible
    recipient->receive( giveup_smart_ptr(info) );
}

  //for processing out-of-band messages ( ControlInfo's)
 template<class InfoClass>
  void InfoHandlerDerived<InfoClass>::executeControl(boost::shared_ptr<Service> baseService, boost::shared_ptr<Info> baseInfo, bool doError) const
  {
    SMART_ASSERT( baseService );
    boost::shared_ptr<ControlInfoRecipient<InfoClass> > recipient = 
      boost::dynamic_pointer_cast<ControlInfoRecipient<InfoClass> >(baseService);
    if( !recipient )
    {
      if( doError )
      {
	Logger::warn() << "InfoHandler: invalid ControlInfo recipient, expected " 
		       << typeid(ControlInfoRecipient<InfoClass>).name() 
		       << ", received " << typeid(*baseService).name() << std::endl;
      }
      return;
    }
	    
    boost::shared_ptr<InfoClass> info = boost::dynamic_pointer_cast<InfoClass>(baseInfo);
    if( !info )
    {
      Logger::warn() << "InfoHandler: invalid info, expected " << typeid(boost::shared_ptr<InfoClass>).name() 
		     << ", received " << typeid(baseInfo).name() << std::endl;
      return;
    }
    baseInfo.reset();	///< so that it no longer points to the Info

    SMART_ASSERT( recipient );
    SMART_ASSERT( info );

    recipient->receiveControl( giveup_smart_ptr(info) );
}




  template<class InfoClass>
  Info::ClassType InfoHandlerDerived<InfoClass>::getClassType() const
  {
    return fClassType;
  }

  template<class InfoClass>
  size_t InfoHandlerDerived<InfoClass>::getByteSize() const
  {
    return sizeof(InfoClass);
  }


} //namespace

//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
