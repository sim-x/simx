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
// File:    writers.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Arp 11 2005
//
// Description: operators<< for various types
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_WRITERS
#define NISAC_SIMX_WRITERS

#include "simx/type.h"

#include "loki/AssocVector.h"

#include <list>
#include <map>
#include <vector>
#include <set>
#include <string>

#include <iostream>


/// \file writers.h
/// \brief Operators<< used to output things in simx

namespace std {
/// namespace std used so that operators<< for std::containers can be found

template<class IndexType, class ValueType> 
inline std::ostream& operator<<(std::ostream& os, const std::map<IndexType,ValueType>& m)
{
    os << "Map=[";
    for(typename std::map<IndexType,ValueType>::const_iterator iter=m.begin(); 
	iter!=m.end(); 
	++iter)
    {
	os << iter->first << "->" << iter->second << "  ";
    }
    os << "]";
    return os;
}

template<class IndexType, class ValueType> 
inline std::ostream& operator<<(std::ostream& os, const std::multimap<IndexType,ValueType>& m)
{
    os << "Map=[";
    for(typename std::multimap<IndexType,ValueType>::const_iterator iter=m.begin(); 
	iter!=m.end(); 
	++iter)
    {
	os << iter->first << "->" << iter->second << "  ";
    }
    os << "]";
    return os;
}

template<class ValueType> 
inline std::ostream& operator<<(std::ostream& os, const std::vector<ValueType>& vector)
{
    os << "Vector=[";
    for(typename std::vector<ValueType>::const_iterator iter=vector.begin(); 
	iter!=vector.end(); 
	++iter)
    {
        os << *iter << " ";
    }
    os << "]";
    return os;
}

template<class ValueType> 
inline std::ostream& operator<<(std::ostream& os, const std::list<ValueType>& list)
{
    os << "List=[";
    for(typename std::list<ValueType>::const_iterator iter=list.begin(); 
	iter!=list.end(); 
	++iter)
    {
        os << *iter << " ";
    }
    os << "]";
    return os;
}

template<class ValueType> 
inline std::ostream& operator<<(std::ostream& os, const std::set<ValueType>& set)
{
    os << "Set=[";
    for(typename std::set<ValueType>::const_iterator iter=set.begin(); 
	iter!=set.end(); 
	++iter)
    {
        os << *iter << " ";
    }
    os << "]";
    return os;
}

template<class ValueType> 
inline std::ostream& operator<<(std::ostream& os, const std::multiset<ValueType>& set)
{
    os << "Set=[";
    for(typename std::multiset<ValueType>::const_iterator iter=set.begin(); 
	iter!=set.end(); 
	++iter)
    {
        os << *iter << " ";
    }
    os << "]";
    return os;
}

} // std namespace

namespace Loki {

template<class IndexType, class ValueType> 
inline std::ostream& operator<<(std::ostream& os, const Loki::AssocVector<IndexType,ValueType>& map)
{
    os << "AssocVector=[";
    for(typename Loki::AssocVector<IndexType,ValueType>::const_iterator iter=map.begin(); 
	iter!=map.end(); 
	++iter)
    {
	os << iter->first << "->" << iter->second << "  ";
    }
    os << "]";
    return os;
}

} // Loki namespace



//=======================================================================================



#endif // NISAC_SIMX_WRITERS

