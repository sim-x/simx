//-------------------------------------------------------------------------
// $Id: Versions.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//-------------------------------------------------------------------------
//
// File:    Version.C
// Module:  Common
// Author:  gam
// Created: Tue Feb  8 07:44:49 2005
// Description: Defines "write_versions()".  The definition depends on
// whether or not HAVE_VERSION_H is defined.
//
// @@COPYRIGHT@@
//
//-------------------------------------------------------------------------

#include "simx/Common/Versions.h"

//-------------------------------------------------------------------------

#ifndef HAVE_VERSION_H
// use the following if we don't have config/versions.h

#include <iostream>

namespace Common
{
  void write_versions()
  {
    std::cout << "version information not available" << std::endl;
  }
} // namespace

#endif // not HAVE_VERSION_H

//-------------------------------------------------------------------------
// End of Version.C
//-------------------------------------------------------------------------
