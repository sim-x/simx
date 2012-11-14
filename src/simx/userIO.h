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
// File:    userIO.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Arp 13 2005
//
// Description: map between string and values for operators>> for various types
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_USERIO
#define NISAC_SIMX_USERIO


#include <iostream>
#include <string>

#include "loki/AssocVector.h"

namespace simx {


/// declaration
namespace UserIO {

    template<typename T> bool setPair(const std::string& key, const T& value);
    template<typename T> bool getValue(const std::string& key, T& value);

    namespace Private {
	/// partially specialized container for string->value pairs
	template<typename T> class StringValueContainer : public Loki::AssocVector<std::string, T> {};

	template<typename T> StringValueContainer<T>& getStringValueContainer(T*);	///< T* is juch dummy to have different signatures
    } // Private namespace

} // UserIO namespace


/// implementation
namespace UserIO {

template<typename T>
inline bool setPair(const std::string& key, const T& value)
{
    T* tPtr = 0;
    std::pair<typename Private::StringValueContainer<T>::iterator, bool> retVal = 
	Private::getStringValueContainer(tPtr).insert( std::make_pair( key, value ) );
    if( !retVal.second && retVal.first->second != value ) 
    {
        std::cerr << "UserIO::setPair: cannot overwrite key " << key
            << " with value " << retVal.first->second << " with a different one "
            << value << std::endl;
        return false;
    }
    return true;
}

template<typename T>
inline bool getValue(const std::string& key, T& value)
{
    T* tPtr = 0;
    typename Private::StringValueContainer<T>::const_iterator iter = 
	Private::getStringValueContainer(tPtr).find( key );
    if( iter == Private::getStringValueContainer(tPtr).end() )
    {
        return false;
    }
    value = iter->second;
    return true;
}


    namespace Private {
	template<typename T>
	inline StringValueContainer<T>& getStringValueContainer(T*)
	{
	    static StringValueContainer<T> container;
	    return container;
	}
    } //Private namespace


} // UserIO namespace


} //simx namespace

#endif


