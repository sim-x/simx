//--------------------------------------------------------------------------
// File:    ControlInfoRecipient.h
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: Oct 09 2008
//
// Description:
//	Abstract Base class for anything capable of receiving a Control-Info message
//	(off-band message) of a particular type
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_CONTROL_INFO_RECIPIENT_H
#define NISAC_SIMX_CONTROL_INFO_RECIPIENT_H

#include "simx/type.h"
#include "boost/shared_ptr.hpp"


namespace simx {

  template<class InfoClass>
  class ControlInfoRecipient
  {
  public:
    virtual ~ControlInfoRecipient() {}
    virtual void receiveControl(boost::shared_ptr<InfoClass> ) = 0;
  };

}//namespace

 #endif

