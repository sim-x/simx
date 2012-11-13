//--------------------------------------------------------------------------
// File:    GenericFileData.C
// Module:  SessionGen
// Author:  Keith Bisset
// Created: January  8 2003
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/File/GenericFileData.h"
using namespace std;

namespace File {
template class GenericFileData<float>;
template class GenericFileData<double>;
template class GenericFileData<char>;
template class GenericFileData<short>;
template class GenericFileData<int>;
template class GenericFileData<long>;

} // namespace
