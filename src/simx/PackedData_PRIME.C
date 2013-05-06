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
// File:    PackedData_PRIME.C
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 7 2005
//
// Description:
//	encapsulates data packing to detach it from particular simulation engine
//	ONLY FOR DaSSF
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/PackedData.h"

#ifdef SIMX_USE_PRIME

#include "ssf.h"

namespace simx {

PackedData::PackedData(minissf::CompactDataType* dp)
    :	fData( dp )
{
  // int debug_wait = 1;
  //while(debug_wait);
  SMART_ASSERT( fData );
}


//=========================================================
// these are here only to be compatible with the header
PackedData::PackedData()
{
    SMART_ASSERT( false );
}

PackedData::PackedData(char* mem, size_t len)
{
    SMART_ASSERT( false );
}


PackedData::~PackedData()
{
}


char* PackedData::getMem() const
{
    return 0;
}

int PackedData::getLength() const
{
    return 0;
}


void PackedData::makeSpace( size_t size )
{
}

//=========================================================

/// adds info into the data
void PackedData::add(const bool& x)
{ 
    char chr = (x) ? 1 : 0;
    fData->add_char(chr); 
}
void PackedData::add(const char& x)
{ fData->add_char(x); }
void PackedData::add(const double& x)
{ fData->add_double(x); }
void PackedData::add(const float& x)
{ fData->add_float(x); }
void PackedData::add(const int& x)
{ fData->add_int(x); }
void PackedData::add(const long& x)
{ fData->add_long(x); }
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
void PackedData::add(const long long& x)
{ fData->add_long_long(x); }
#endif
// #ifdef HAVE_LONG_DOUBLE
// void PackedData::add(const long double& x)
// { fData->add_long_double(x); }
// #endif
void PackedData::add(const short& x)
{ fData->add_short(x); }
void PackedData::add(const unsigned char& x)
{ fData->add_unsigned_char(x); }
void PackedData::add(const unsigned& x)
{ fData->add_unsigned_int(x); }
void PackedData::add(const unsigned long& x)
{ fData->add_unsigned_long(x); }
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
void PackedData::add(const unsigned long long& x)
{ fData->add_unsigned_long_long(x); }
#endif
void PackedData::add(const unsigned short& x)
{ fData->add_unsigned_short(x); }
void PackedData::add(const std::string& x)
{
   fData->add_string(x.c_str());
}

void PackedData::add_array(const int l, const char* p)
{
    /// NOTE: not nice, but does no harm, DaSFF doesn't do 'consts'
    /// but the content of the memory is NOT changed in add_char_array
    char* pp = const_cast<char*>(p);	
    
    SMART_ASSERT(pp); 
    fData->add_char_array(l,pp);
}

/// gets info from the data
bool PackedData::get(bool& x)
{ 
    char chr; 
    bool ret = fData->get_char(&chr);
    x = (chr==1);
    return ret;
}
bool PackedData::get(char& x)
{ return fData->get_char(&x); }
bool PackedData::get(double& x)
{ return fData->get_double(&x); }
bool PackedData::get(float& x)
{ return fData->get_float(&x); }
bool PackedData::get(int& x)
{ return fData->get_int(&x); }
bool PackedData::get(long& x)
{ return fData->get_long(&x); }
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
bool PackedData::get(long long& x)
{ return fData->get_long_long(&x); }
#endif
// #ifdef HAVE_LONG_DOUBLE
// bool PackedData::get(long double& x)
// { return fData->get_long_double(&x); }
// #endif
bool PackedData::get(short& x)
{ return fData->get_short(&x); }
bool PackedData::get(unsigned char& x)
{ return fData->get_unsigned_char(&x); }
bool PackedData::get(unsigned& x)
{ return fData->get_unsigned_int(&x); }
bool PackedData::get(unsigned long& x)
{ return fData->get_unsigned_long(&x); }
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
bool PackedData::get(unsigned long long& x)
{ return fData->get_unsigned_long_long(&x); }
#endif
bool PackedData::get(unsigned short& x)
{ return fData->get_unsigned_short(&x); }
bool PackedData::get(std::string& x)
{
   char *str = fData->get_string();
   if(str != 0) {
      x = std::string(str);
   }
   return (str != 0);
}


bool PackedData::get_array(const int l, char* x)
{ return fData->get_char(x,l)==l; }


} // namespace

#endif // SIMX_USE_PRIME

