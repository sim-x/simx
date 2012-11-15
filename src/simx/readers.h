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

// $d$
//--------------------------------------------------------------------------
// File:    readers.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Arp 11 2005
//
// Description: operators>> for various types
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_READERS
#define NISAC_SIMX_READERS

//--------------------------------------------------------------------------

#include "simx/type.h"

#include <list>
#include <map>
#include <vector>
#include <set>
#include <string>

#include <iostream>

//--------------------------------------------------------------------------

/// \file readers.h
/// \brief Operators>> used to input things in simx

namespace std {
  /// namespace std used so that operators>> for std::containers can
  /// be found

  template<class ValueType> 
  inline std::istream& operator>>
  (std::istream& is, std::vector<ValueType>& vector)
  {
    if( !is || std::ws(is).peek()!='[' )
    {
      std::cerr 
	<< "operator>>: error reading in vector, missing '['"
	<< ", getting: " << is.peek() << std::endl;
      is.setstate(std::ios::failbit);
      return is;
    }

    is.get();	// the '['
    while( is && std::ws(is).peek()!=']' )
    {
      ValueType 	value;
      is >> value;
      vector.push_back(value);
    }

    if( !is || std::ws(is).peek()!=']' )
    {
      std::cerr
	<< "operator>>: error reading in vector, missing ']'"
	<< ", getting: " << is.peek() << std::endl;
      is.setstate(std::ios::failbit);
      return is;
    } 

    is.get();	// the ']'
    return is;
  }

  //--------------------------------------------------------------------------

  template<class ValueType> 
  inline std::istream& operator>>
  (std::istream& is, std::list<ValueType>& list)
  {
    if( !is || std::ws(is).peek()!='[' )
    {
      std::cerr
	<< "operator>>: error reading in list, missing '['"
	<< ", getting: " << is.peek() << std::endl;
      is.setstate(std::ios::failbit);
      return is;
    }

    is.get();	// the '['
    while( is && std::ws(is).peek()!=']' )
    {
      ValueType 	value;
      is >> value;
      list.push_back(value);
    }

    if( !is || std::ws(is).peek()!=']' )
    {
      std::cerr << "operator>>: error reading in list, missing ']'"
		<< ", getting: " << is.peek() << std::endl;
      is.setstate(std::ios::failbit);
      return is;
    } 

    is.get();	// the ']'
    return is;
  }

  //--------------------------------------------------------------------------

  template<class ValueType> 
  inline std::istream& operator>>
  (std::istream& is, std::set<ValueType>& set)
  {
    if( !is || std::ws(is).peek()!='[' )
    {
      std::cerr
	<< "operator>>: error reading in set, missing '['"
	<< ", getting: " << is.peek() << std::endl;
      is.setstate(std::ios::failbit);
      return is;
    }

    is.get();	// the '['
    while( is && std::ws(is).peek()!=']' )
    {
      ValueType 	value;
      is >> value;
      if( !set.insert(value).second )
      {
	std::cerr
	  << "operator>>: reading in duplicate entry for a set " 
	  << value << std::endl;
      }
    }

    if( !is || std::ws(is).peek()!=']' )
    {
      std::cerr
	<< "operator>>: error reading in set, missing ']'"
	<< ", getting: " << is.peek() << std::endl;
      is.setstate(std::ios::failbit);
      return is;
    } 

    is.get();	// the ']'
    return is;
  }

  template<class ValueType> 
  inline std::istream& operator>>
  (std::istream& is, std::multiset<ValueType>& set)
  {
    if( !is || std::ws(is).peek()!='[' )
    {
      std::cerr
	<< "operator>>: error reading in set, missing '['"
	<< ", getting: " << is.peek() << std::endl;
      is.setstate(std::ios::failbit);
      return is;
    }

    is.get();	// the '['
    while( is && std::ws(is).peek()!=']' )
    {
      ValueType 	value;
      is >> value;
      if( !set.insert(value).second )
      {
	std::cerr
	  << "operator>>: reading in duplicate entry for a set " 
	  << value << std::endl;
      }
    }

    if( !is || std::ws(is).peek()!=']' )
    {
      std::cerr
	<< "operator>>: error reading in set, missing ']'"
	<< ", getting: " << is.peek() << std::endl;
      is.setstate(std::ios::failbit);
      return is;
    } 

    is.get();	// the ']'
    return is;
  }



template<class KeyType, class ValueType> 
inline std::istream& operator>>(std::istream& is, std::map<KeyType,ValueType>& map)
{
    if( !is || std::ws(is).peek()!='[' )
    {
	std::cerr << "operator>>: error reading in map, missing '['"
	    << ", getting: " << is.peek() << std::endl;
	is.setstate(std::ios::failbit);
	return is;
    }
    is.get();	// the '['
    while( is && std::ws(is).peek()!=']' )
    {
	if( !is || std::ws(is).peek()!='<' )
	{
	    std::cerr << "operator>>: error reading in map, missing '<'"
		<< ", getting: " << is.peek() << std::endl;
	    is.setstate(std::ios::failbit);
	    return is;
	}
	is.get();	// the '<'
	
	KeyType		key;
	ValueType 	value;
	is >> key;
	is >> value;
	map.insert(std::make_pair(key,value));
	
	if( !is || std::ws(is).peek()!='>' )
	{
	    std::cerr << "operator>>: error reading in map, missing '>'"
		<< ", getting: " << is.peek() << std::endl;
	    is.setstate(std::ios::failbit);
    	    return is;
	} 
	is.get();	// the '>'
	
    }
    if( !is || std::ws(is).peek()!=']' )
    {
	std::cerr << "operator>>: error reading in map, missing ']'"
	    << ", getting: " << is.peek() << std::endl;
	is.setstate(std::ios::failbit);
	return is;
    } 
    is.get();	// the ']'
    return is;
}

template<class KeyType, class ValueType> 
inline std::istream& operator>>(std::istream& is, std::multimap<KeyType,ValueType>& map)
{
    if( !is || std::ws(is).peek()!='[' )
    {
	std::cerr << "operator>>: error reading in map, missing '['"
	    << ", getting: " << is.peek() << std::endl;
	is.setstate(std::ios::failbit);
	return is;
    }
    is.get();	// the '['
    while( is && std::ws(is).peek()!=']' )
    {
	if( !is || std::ws(is).peek()!='<' )
	{
	    std::cerr << "operator>>: error reading in map, missing '<'"
		<< ", getting: " << is.peek() << std::endl;
	    is.setstate(std::ios::failbit);
	    return is;
	}
	is.get();	// the '<'
	
	KeyType		key;
	ValueType 	value;
	is >> key;
	is >> value;
	map.insert(std::make_pair(key,value));
	
	if( !is || std::ws(is).peek()!='>' )
	{
	    std::cerr << "operator>>: error reading in map, missing '>'"
		<< ", getting: " << is.peek() << std::endl;
	    is.setstate(std::ios::failbit);
    	    return is;
	} 
	is.get();	// the '>'
	
    }
    if( !is || std::ws(is).peek()!=']' )
    {
	std::cerr << "operator>>: error reading in map, missing ']'"
	    << ", getting: " << is.peek() << std::endl;
	is.setstate(std::ios::failbit);
	return is;
    } 
    is.get();	// the ']'
    return is;
}



 //  template<class Type1, class Type2>
//   inline std::istream& operator>> (std::istream& is, std::pair<Type1, Type2>)
//   {
//     if (!is || std::ws(is).peek()!='<')
//       {
// 	std::cerr
// 	  << "operator>>: error reading in pair, missing '<'"
// 	  << ", getting: " << is.peek() << std::endl;
// 	is.setstate(std::ios::failbit);
// 	return is;
//       }
//     is.get(); // read in the '<'
//     Type1 first;
//     Type1 second;
//     is >> first;
//     is >> second;
    
    


} // std namespace

//--------------------------------------------------------------------------
/// read-in functions for ServiceName and ServiceAddress

namespace simx {

  /// special read-in for ServiceAddress (so that either text or
  /// number can be given)
  std::istream& operator>>(std::istream&, ServiceAddress&);

} // simx namespace

//--------------------------------------------------------------------------
#endif // NISAC_SIMX_READERS
//--------------------------------------------------------------------------
