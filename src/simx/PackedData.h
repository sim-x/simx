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
// File:    PackedData.h
// Module:  simx
// Author:  Lukas Kroc, Sunil Thulasidasan
// Created: Feb 7 2005
//
// Description:
//	encapsulates data packing to detach it from particular simulation engine
//	(e.g. DaSSF). In case it is needed, tuples with more items can be added
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_PACKEDDATA
#define NISAC_SIMX_PACKEDDATA

#include "simx/type.h"

#include "Common/Assert.h"

#include <map>
#include <vector>
#include <list>
#include <set>
#include <string.h>

#include <boost/python.hpp>

// namespace prime {
//    namespace ssf {
//       class CompactDataType;	// the DaSSF object
//    }
// }


namespace minissf {
  class CompactDataType;
}

namespace simx {

/// encapsulates data packing to detach it from particular simulation engine
class PackedData
{
    public:
	/// constructor from ssf minissf::CompactDataType
#ifdef SIMX_USE_PRIME
	explicit PackedData(minissf::CompactDataType*);
#endif
	PackedData();
	PackedData( char*, size_t );	//< ocnstructor for unpacking
	~PackedData();
	
	char* getMem() const;
	int getLength() const;
	
	
	/// adds info into the data
	void add(const bool&);
	void add(const char&);
	void add(const double&);
	void add(const float&);
	void add(const int&);
	void add(const long&);
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
    void add(const long long&);
#endif
// #ifdef HAVE_LONG_DOUBLE
// 	void add(const long double&);
// #endif
	void add(const short&);
	void add(const unsigned char&);
	void add(const unsigned&);
	void add(const unsigned long&);
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
	void add(const unsigned long long&);
#endif
	void add(const unsigned short&);
	void add(const std::string&);

	/// for python objects
	void add(const boost::python::object& );
  void add(const boost::python::str& );

	template<typename A1, typename A2> void add(const std::pair<A1,A2>&);	
	template<typename A1> void add(const boost::tuple<A1>&);
	template<typename A1, typename A2> void add(const boost::tuple<A1,A2>&);
	template<typename A1, typename A2, typename A3> void add(const boost::tuple<A1,A2,A3>&);

	void add_array(const int, const char*);
	
	/// std::container adds
	template<typename Type1, typename Type2> void add(const std::map<Type1,Type2>&);
	template<typename Type> void add(const std::vector<Type>&);
	template<typename Type> void add(const std::list<Type>&);
	template<typename Type> void add(const std::set<Type>&);



	/// gets info from the data
	bool get(bool&);
	bool get(char&);
	bool get(double&);
	bool get(float&);
	bool get(int&);
	bool get(long&);
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
    bool get(long long&);
#endif
// #ifdef HAVE_LONG_DOUBLE
// 	bool get(long double&);
// #endif
	bool get(short&);
	bool get(unsigned char&);
	bool get(unsigned&);
	bool get(unsigned long&);
#ifndef PRIME_SSF_ARCH_X86_64_LINUX
	bool get(unsigned long long&);
#endif
	bool get(unsigned short&);
	bool get(std::string&);

	/// for python objects
	bool get(boost::python::object&);
  bool get(boost::python::str&);

	template<typename A1, typename A2> bool get(std::pair<A1,A2>&);	
	template<typename A1> bool get(boost::tuple<A1>& x);
	template<typename A1, typename A2> bool get(boost::tuple<A1,A2>& x);
	template<typename A1, typename A2, typename A3> bool get(boost::tuple<A1,A2,A3>& x);

	bool get_array(const int, char*);

	/// std::container gets
	template<typename Type1, typename Type2> bool get(std::map<Type1,Type2>&);
	template<typename Type> bool get(std::vector<Type>&);
	template<typename Type> bool get(std::list<Type>&);
	template<typename Type> bool get(std::set<Type>&);

	
	
    private:
#ifdef SIMX_USE_PRIME
	minissf::CompactDataType*	fData;
#endif

	// generic function for copying x byte-by-byte
	template<typename T> void addAnything(const T& x);
	template<typename T> bool getAnything(T& x);

	// makes space in fData for size bytes, if needed:
	void makeSpace( size_t size );

	char*				fMem;
	char*				fNextWr;	//< next empty byte
	char*				fNextRd;	//< next byte not read yet
	size_t				fSize;	//< how much memory is allocated
	bool				fFreeIt; //< whether to free fMem in destructor
    
};

//=======================================================================


template<typename T> void PackedData::addAnything(const T& x)
{
    makeSpace( sizeof(T) + 1);
    // now we are sure we have enough space
    
    //std::cout << "Writing X to positon " << long(fNextWr-fMem) << std::endl;
    *fNextWr = 'X'; fNextWr++; //< fluff field separator. For debugging
    //std::cout << "mem copying to position " << long(fNextWr-fMem) << " bytes: " << sizeof(T)
    //	      << std::endl;
    memcpy( fNextWr, &x, sizeof(T) );
    fNextWr += sizeof(T);
}

template<typename T> bool PackedData::getAnything(T& x)
{
    SMART_ASSERT( fNextRd );
    if( (size_t)(fNextRd-fMem + sizeof(T)+1) > fSize )
	return false;
    SMART_ASSERT( *fNextRd == 'X' )( (char)*fNextRd )((char)*(fNextRd-1))
      ((char)*(fNextRd+1));
    //std::cout << "X present at position " << long(fNextRd-fMem) << std::endl;
    fNextRd++;
    //std::cout << "mem copying from position " << long(fNextRd-fMem) << " bytes " << sizeof(T) << std::endl;
    memcpy( &x, fNextRd, sizeof(T) );
    fNextRd += sizeof(T);
    return true;
}


//=======================================================================
// adds

template<typename A1, typename A2> 
void PackedData::add(const std::pair<A1,A2>& x)
{
    add(x.first);
    add(x.second);
}

template<typename A1> 
void PackedData::add(const boost::tuple<A1>& x)
{
    add(x.get<0>());
}

template<typename A1, typename A2> 
void PackedData::add(const boost::tuple<A1,A2>& x)
{
    add(x.get<0>());
    add(x.get<1>());
}

template<typename A1, typename A2, typename A3> 
void PackedData::add(const boost::tuple<A1,A2,A3>& x)
{
    add(x.get<0>());
    add(x.get<1>());
    add(x.get<2>());
}

template<typename Type1, typename Type2>
void PackedData::add(const std::map<Type1,Type2>& m)
{
    unsigned int size = m.size();
    add(size);
    for(typename std::map<Type1,Type2>::const_iterator iter = m.begin();
	iter != m.end();
	++iter)
    {
	add(iter->first);
	add(iter->second);
    }
}

template<typename Type>
void PackedData::add(const std::vector<Type>& v)
{
    unsigned int size = v.size();
    add(size);
    for(typename std::vector<Type>::const_iterator iter = v.begin();
	iter != v.end();
	++iter)
    {
	add(*iter);
    }
}

template<typename Type>
void PackedData::add(const std::list<Type>& l)
{
    unsigned int size = l.size();
    add(size);
    for(typename std::list<Type>::const_iterator iter = l.begin();
	iter != l.end();
	++iter)
    {
	add(*iter);
    }
}

template<typename Type>
void PackedData::add(const std::set<Type>& s)
{
    unsigned int size = s.size();
    add(size);
    for(typename std::set<Type>::const_iterator iter = s.begin();
	iter != s.end();
	++iter)
    {
	add(*iter);
    }
}

//====================
// gets

template<typename A1, typename A2> 
bool PackedData::get(std::pair<A1,A2>& x)
{
    return get(x.first) && get(x.second);
}

template<typename A1> 
bool PackedData::get(boost::tuple<A1>& x)
{
    return get(x.get<0>());
}

template<typename A1, typename A2> 
bool PackedData::get(boost::tuple<A1,A2>& x)
{
    return get(x.get<0>()) && get(x.get<1>());
}

template<typename A1, typename A2, typename A3> 
bool PackedData::get(boost::tuple<A1,A2,A3>& x)
{
    return get(x.get<0>()) && get(x.get<1>()) && get(x.get<2>());
}

template<typename Type1, typename Type2> 
bool PackedData::get(std::map<Type1,Type2>& m)
{
    bool ret = true;
    unsigned int size;
    get(size);
    while( size-- )
    {
	Type1 val1;
	ret = ret && get(val1);
	Type2 val2;
	ret = ret && get(val2);
	
	bool ins = m.insert(std::make_pair(val1,val2)).second;
	SMART_ASSERT( ins )(val1)(val2);
    }
    return ret;
}

template<typename Type> 
bool PackedData::get(std::vector<Type>& v)
{
    bool ret = true;
    unsigned int size;
    get(size);

    while( size-- )
    {
	Type val;
	ret = ret && get(val);
	v.push_back( val );
    }
    return ret;
}

template<typename Type> 
bool PackedData::get(std::list<Type>& l)
{
    bool ret = true;
    unsigned int size;
    get(size);
    while( size-- )
    {
	Type val;
	ret = ret && get(val);
	l.push_back( val );
    }
    return ret;
}

template<typename Type> 
bool PackedData::get(std::set<Type>& s)
{
    bool ret = true;
    unsigned int size;
    get(size);
    while( size-- )
    {
	Type val;
	ret = ret && get(val);

	bool ins = s.insert(val).second;
	SMART_ASSERT( ins )(val);
    }
    return ret;
}

} // namespace

#endif 

