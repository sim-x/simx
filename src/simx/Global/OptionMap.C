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

//-------------------------------------------------------------------------
// $Id: OptionMap.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//-------------------------------------------------------------------------
//
// File:    OptionMap.C
// Module:  Global
// Author:  bosetti
// Created: June 2004
// Description:
//
// @@
//
//-------------------------------------------------------------------------

// INCLUDES
#include "simx/Global/OptionMap.h"

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

namespace Global {

  // IMPLEMENTATION OF OptionMap_

  OptionMap_::OptionMap_()
  {}

  bool OptionMap_::Register(std::string name, OptionHandler func)
  {
    optmap[name] = func;
    return(true);
  }

  bool OptionMap_::Register(const char *name, OptionHandler func)
  {
    return(Register(std::string(name),func));
  }

  OptionHandler OptionMap_::GetHandler(std::string name)
  {
    return(optmap[name]);
  }

  OptionHandler OptionMap_::GetHandler(const char *name)
  {
    return(GetHandler(std::string(name)));
  }

} // namespace

//-------------------------------------------------------------------------
// End of OptionMap.C
//-------------------------------------------------------------------------
