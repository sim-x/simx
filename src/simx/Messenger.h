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

// $Id: Messenger.h,v 1.2 2010/02/28 05:01:39 kroc Exp $
//--------------------------------------------------------------------------
// File:    Messenger.h
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: Oct 09 2008
//
//	Functions for handling out-of-band messaging
//
// @@
//
//--------------------------------------------------------------------------

#ifdef HAVE_MPI_H
#ifdef SIMX_USE_PRIME


#ifndef NISAC_SIMX_MESSENGER_H
#define NISAC_SIMX_MESSENGER_H

#include "simx/type.h"




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

#endif
#endif // SIMX_USE_PRIME
#endif //HAVE_MPI_H

