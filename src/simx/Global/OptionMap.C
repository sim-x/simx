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
// @@COPYRIGHT@@
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
