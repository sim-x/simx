// $Id: Messenger.h,v 1.2 2010/02/28 05:01:39 kroc Exp $
//--------------------------------------------------------------------------
// File:    Messenger.h
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: Oct 09 2008
//
//	Functions for handling out-of-band messaging
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_MESSENGER_H
#define NISAC_SIMX_MESSENGER_H

#include "simx/type.h"

#ifdef SIMX_USE_PRIME


namespace simx {

  class ControlInfoWrapper;

  namespace Messenger {

    bool sendMessage( LPID, LPID, ControlInfoWrapper& );

    void initCommunicator();

    bool MessengerActive();

    int getMessengerRank();
    
    void checkStatus();

    void finalize();


  } //namespace Messenger

} //namespace simx

#endif // SIMX_USE_PRIME

#endif
