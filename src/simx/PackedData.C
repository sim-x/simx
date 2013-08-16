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
// File:    PackedData.C
// Module:  simx
// Author:  Lukas Kroc, Sunil Thulasidasan
// Created: Feb 25 2010
//
// Description:
//	encapsulates data packing to detach it from particular simulation engine
//	ONLY FOR SIMENGINE
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/PackedData.h"

#include "simx/InfoManager.h"


#ifndef SIMX_USE_PRIME

using namespace boost;

namespace simx {

PackedData::PackedData()
{
    // start with some little memory
    fSize = 10;
    
    fMem = (char*)malloc( fSize );
    fNextWr = fNextRd = fMem;
    SMART_ASSERT( fMem );
    fFreeIt = true;
}

PackedData::PackedData(char* mem, size_t len)
{
    fMem = mem;
    fNextWr = mem;
    fNextRd = mem;
    fSize = len;
    fFreeIt = false;
}


PackedData::~PackedData()
{
    if( fFreeIt )
	free( fMem );
}


char* PackedData::getMem() const
{
    return fMem;
}

int PackedData::getLength() const
{
    size_t len  = fNextWr-fMem;
    SMART_ASSERT( len >= 0 )( len );
    SMART_ASSERT( len <= fSize )( len )( fSize );
    return len;
}

#include <iostream>
using namespace std;

void PackedData::makeSpace( size_t size )
{
  //cerr << "PackedData::makeSpace1 fSize=" << fSize << ", free=" << fSize-(fNextWr-fMem) << ", wanted=" << size << endl;
    if( size > fSize-(fNextWr-fMem) )
    {
	// must make some more space in the buffer
	size_t posWr = fNextWr-fMem;
	size_t posRd = fNextRd-fMem;

	size_t incr = (size > fSize) ? size : fSize;
	fSize += incr;
	fMem = (char*)realloc(fMem, fSize);
	SMART_ASSERT( fMem );

	fNextWr = fMem + posWr;
	fNextRd = fMem + posRd;
    }
    //cerr << "PackedData::makeSpace2 fSize=" << fSize << ", free=" << fSize-(fNextWr-fMem) << ", wanted=" << size << endl << endl;
}

//==========================================================
	
/// adds info into the data
void PackedData::add(const bool& x)
{ addAnything(x); }
void PackedData::add(const char& x)
{ addAnything(x); }
void PackedData::add(const double& x)
{ addAnything(x); }
void PackedData::add(const float& x)
{ addAnything(x); }
void PackedData::add(const int& x)
{ addAnything(x); }
void PackedData::add(const long& x)
{ addAnything(x); }
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
void PackedData::add(const long long& x)
{ addAnything(x); }
#endif
#ifdef HAVE_LONG_DOUBLE
void PackedData::add(const long double& x)
{ addAnything(x); }
#endif
void PackedData::add(const short& x)
{ addAnything(x); }
void PackedData::add(const unsigned char& x)
{ addAnything(x); }
void PackedData::add(const unsigned& x)
{ addAnything(x); }
void PackedData::add(const unsigned long& x)
{ addAnything(x); }
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
void PackedData::add(const unsigned long long& x)
{ addAnything(x); }
#endif
void PackedData::add(const unsigned short& x)
{ addAnything(x); }
void PackedData::add(const std::string& x)
{
    size_t len = x.length()+1;
    makeSpace( sizeof(size_t)+1 + sizeof(char) * len );
    addAnything( len );
    //cout << "string copying to position " << long(fNextWr-fMem) << " length: " << len << endl;
    strncpy( fNextWr, x.c_str(), len );    
    fNextWr += sizeof(char) * len;
    SMART_ASSERT( (size_t)(fNextWr-fMem) <= fSize )( fNextWr-fMem )( fSize )( sizeof(char) )( len );
}


void PackedData::add(const python::object& obj) {
  
  python::str py_str = python::extract<python::str>( obj );
  add( py_str );
}


void PackedData::add(const python::str& py_str) {
  
  int len = python::extract<int>( py_str.attr("__len__")() );
  const char* str = python::extract<const char*>(py_str);
  //cout << "length: " << len << " string: " << str;
  add_array( len, str );
}

void PackedData::add_array(const int l, const char* p)
{
    makeSpace( sizeof(int)+1 + sizeof(char) * l );
    addAnything( l );
    //cout << "mem copying to position " << long(fNextWr-fMem) << " bytes: " << sizeof(char)*l
    //	 << endl;
    memcpy( fNextWr, p, sizeof(char) * l );
    fNextWr += sizeof(char) * l;
    SMART_ASSERT( (size_t)(fNextWr-fMem) <= fSize )( fNextWr-fMem )( fSize )( l );
}

/// gets info from the data
bool PackedData::get(bool& x)
{ return getAnything(x); }
bool PackedData::get(char& x)
{ return getAnything(x); }
bool PackedData::get(double& x)
{ return getAnything(x); }
bool PackedData::get(float& x)
{ return getAnything(x); }
bool PackedData::get(int& x)
{ return getAnything(x); }
bool PackedData::get(long& x)
{ return getAnything(x); }
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
bool PackedData::get(long long& x)
{ return getAnything(x); }
#endif
#ifdef HAVE_LONG_DOUBLE
bool PackedData::get(long double& x)
{ return getAnything(x); }
#endif
bool PackedData::get(short& x)
{ return getAnything(x); }
bool PackedData::get(unsigned char& x)
{ return getAnything(x); }
bool PackedData::get(unsigned& x)
{ return getAnything(x); }
bool PackedData::get(unsigned long& x)
{ return getAnything(x); }
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
bool PackedData::get(unsigned long long& x)
{ return getAnything(x); }
#endif
bool PackedData::get(unsigned short& x)
{ return getAnything(x); }
bool PackedData::get(std::string& x)
{    
    size_t len;
    getAnything( len );
    char *str = (char*)malloc( sizeof(char) * len );
    if( !str )
	return false;
    strncpy( str, fNextRd, len );
    fNextRd += sizeof(char) * len;
    x = std::string(str);
    free(str);
    return true;
}

bool PackedData::get(python::object& obj) {
  //cout << "inside get python " << endl;
  //assert(false);
  int len;
  get( len );
  //cout << "length is " << len;
  char* str = (char*)malloc( sizeof(char) * len );
  //std::auto_ptr<char> str( new char[len]);
   if ( ! str ) 
     return false;
  //assert( str );
  //cout << "mem copying from position " << int(fNextRd-fMem)
   //   << " bytes " << sizeof(char)*len << endl;
  memcpy( str, fNextRd, sizeof(char) * len );
  fNextRd += sizeof(char) * len;
  //assert(false);
  boost::python::str py_str( str, len );
  //assert(false);
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  obj = theInfoManager().getUnpacker()(py_str);
  PyGILState_Release(gstate);
  //assert(false);
  free(str);
  return true;
}

  bool PackedData::get(python::str& py_str) {
    //cout << "inside get python " << endl;
    //assert(false);
    int len;
    get( len );
    //cout << "length is " << len;
    char* str = (char*)malloc( sizeof(char) * len );
    //std::auto_ptr<char> str( new char[len]);
    if ( ! str ) 
      return false;
    //assert( str );
    //cout << "mem copying from position " << int(fNextRd-fMem)
    //	 << " bytes " << sizeof(char)*len << endl;
    memcpy( str, fNextRd, sizeof(char) * len );
    fNextRd += sizeof(char) * len;
    //PyGILState_STATE gstate;
    //gstate = PyGILState_Ensure();
    py_str = boost::python::str( str,len);
    //PyGILState_Release(gstate);
    free( str );
    return true;
  }



bool PackedData::get_array(const int l, char* x)
{
    int check;
    getAnything( check );
    if( check > l )
	return false;
    memcpy( x, fNextRd, sizeof(char) * check );
    fNextRd += sizeof(char) * check;
    return true;
}

} // namespace

#endif // not SIMX_USE_PRIME

