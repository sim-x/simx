//--------------------------------------------------------------------------
// File:    userIO.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Arp 13 2005
//
// Description: map between string and values for operators>> for various types
//
// @@COPYRIGHT@@
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


