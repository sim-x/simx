//--------------------------------------------------------------------------
// File:    writers.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Arp 11 2005
//
// Description: operators<< for various types
//
// @@COPYRIGHT@@
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

