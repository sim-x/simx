//--------------------------------------------------------------------------
// $Id: cfstreambuf.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    cfstreambuf.C
// Module:  File
// Author:  Randall Bosetti
// Created: June 07 2004
// Description:
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/File/cfstreambuf.h"
#include "simx/Global/util.h"

//--------------------------------------------------------------------------

namespace File
{
  newname_f cfstreambuf::sfNewname = Global::DecToAbc;
  std::string cfstreambuf::ssHeader = "#";
} // end namespace File

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
