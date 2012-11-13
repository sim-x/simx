//--------------------------------------------------------------------------
// File:    UserFields.C
// Module:  File
// Author:  Keith Bisset
// Created: July  1 2002
//
// @@COPYRIGHT@@
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
