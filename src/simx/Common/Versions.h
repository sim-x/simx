//-*-C++-*-----------------------------------------------------------------
// $Id: Versions.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//-------------------------------------------------------------------------
//
// File:    Version.h
// Module:  Common
// Author:  gam
// Created: Tue Feb  8 07:44:49 2005
// Description: Declares "write_versions()", which is called when a NISAC
// program is called with a "-version" argument.
//
// @@COPYRIGHT@@
//
//-------------------------------------------------------------------------

#ifndef NISAC_COMMON_VERSION
#define NISAC_COMMON_VERSION

//-------------------------------------------------------------------------

#ifdef HAVE_VERSION_H
#include "simx/Config/Versions.h"
#else

namespace Common
{
  void write_versions();
}

#endif // HAVE_VERSION_H

//-------------------------------------------------------------------------

#endif // NISAC_COMMON_VERSION

//-------------------------------------------------------------------------
// End of Version.h
//-------------------------------------------------------------------------
