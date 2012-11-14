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

//--------------------------------------------------------------------------
// File:    UserFields.C
// Module:  File
// Author:  Keith Bisset
// Created: July  1 2002
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/File/UserFields.h"
#include <sstream>
#include <string>
#include <vector>
using namespace std;

namespace File {

// create instantaitions of reasonable types
template class UserFields<float>;
template class UserFields<double>;
template class UserFields<char>;
template class UserFields<short>;
template class UserFields<int>;
template class UserFields<long>;
template class UserFields<string>;




} // namespace
