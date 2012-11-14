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

//-*-C++-*-----------------------------------------------------------------
// $Id: OptionMap.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//-------------------------------------------------------------------------
//
// File:    OptionMap.h
// Module:  Global
// Author:  bosetti
// Created: June 2004
// Description:
//
// @@
//
//-------------------------------------------------------------------------

#ifndef NISAC_GLOBAL_OPTIONMAP
#define NISAC_GLOBAL_OPTIONMAP

// INCLUDES

#include "simx/Global/OptionManager.h"
#include "simx/loki/Singleton.h"
#include <map>

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

namespace Global {

  class OptionMap_
  {

  public:

    OptionMap_();

    bool Register(std::string name, OptionHandler func);
    bool Register(const char *name, OptionHandler func);

    OptionHandler GetHandler(std::string name);
    OptionHandler GetHandler(const char *name);

  private:

    // DATA

    std::map< std::string, OptionHandler > optmap;

    // NOT IMPLEMENTED
    OptionMap_( const OptionMap_& );
    OptionMap_& operator = ( const OptionMap_& );

  };

  typedef Loki::SingletonHolder<  OptionMap_,
				  Loki::CreateStatic,
				  Loki::NoDestroy > OptionMap;

} // namespace

#endif // NISAC_GLOBAL_OPTIONMAP

//-------------------------------------------------------------------------
// End of OptionMap.h
//-------------------------------------------------------------------------
