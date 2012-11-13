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
// @@COPYRIGHT@@
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
