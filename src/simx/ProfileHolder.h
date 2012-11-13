//--------------------------------------------------------------------------
// File:    ProfileHolder.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 7 2005
//
// Description:
//	object that hold informaton stored in a profile
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_PROFILEHOLDER
#define NISAC_SIMX_PROFILEHOLDER


#include "simx/readers.h"
#include "simx/writers.h"
#include "simx/logger.h"

#include <sstream>
#include <map>

namespace simx {

/// object that hold informaton stored in a profile
class ProfileHolder
{
    public:
	typedef std::string KeyType;
	typedef std::string DataType;
    
	/// adds to the profile new item range
	template<class Iterator> void add(const Iterator& begin, const Iterator& end);
	
	/// adds to the profile one new item
	void add(const KeyType& key, const DataType& data);
	
	/// retrieves a data item, returns false iff it fails to find it or conversion fails
	template<class Result> bool get(const KeyType& key, Result& result) const;
    
    private:
	typedef std::map<KeyType, DataType> ProfileMap;
	ProfileMap	fProfileMap;
};

//=======================================================================

template<class Iterator> 
void ProfileHolder::add(const Iterator& begin, const Iterator& end)
{
    fProfileMap.insert(begin, end);
}

inline void ProfileHolder::add(const KeyType& key, const DataType& data)
{
    fProfileMap.insert(std::make_pair(key,data));
}

template<class Result> 
bool ProfileHolder::get(const KeyType& key, Result& result) const
{
    ProfileMap::const_iterator iter = fProfileMap.find(key);
    if( iter != fProfileMap.end() )
    {
	try {
	    std::stringstream sstr;
	    sstr << iter->second;
	    sstr >> result;
	    if( sstr.fail() || !std::ws(sstr).eof() )	///< the conversion must not fail, and nothing must remain in sstr
	    {
		throw std::exception();
	    }
	} catch(...)
	{
	    Logger::error() << "ProfileHolder: cannot convert '" << iter->second << "' to type "
		<< typeid(Result).name() << std::endl;
	    return false;
	}
	return true;
    }
    return false;
}

/// string is special, because it may have white spaces
template<> 
inline bool ProfileHolder::get<std::string>(const KeyType& key, std::string& result) const
{
    ProfileMap::const_iterator iter = fProfileMap.find(key);
    if( iter != fProfileMap.end() )
    {
	result = iter->second;
	return true;
    }
    return false;
}



} // namespace
#endif 

