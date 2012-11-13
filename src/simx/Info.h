//--------------------------------------------------------------------------
// File:    Info.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Jan 12 2005
//
// Description: base class for Infos that
//	carry information between Entities
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_INFO
#define NISAC_SIMX_INFO

#include "simx/type.h"
#include "simx/PackedData.h"
#include "simx/Input.h"
#include "simx/logger.h"

#include <iosfwd>

/// base class to derive real Info classes from
///	- either both pack and unpack are overloaded, or neither (this is checked at register time)
///	- must have a default constructor and a copy constructor
///		- copy constructor should do DEEP copies, the object is only copied when necessary
///	- Plain Old Data can be un/packed using the functions provided
///	    with this Info (they don't need overloading), but all machines involved
///	    must have the same data type sizes
///	- the default getSize function returns (sizeof(derived
///	    Info Type)-sizeof(Info))  (to substract VMT size)
///	- if wished to be initiable from Input record, must have readData and readProfile overloaded
///	- it is made descendant of Input so that it can read input directly, and not through Input constructor

namespace simx {

class InfoHandler;

/// Base class for Info objects
/// Allow simple info exchange between InfoRecipients
/// All Info-derived obects need to be registered with theInfoManager().registerInfo(...)
class Info : public Input
{
    public:
	typedef int ClassType;	///< type to use to identify a specific descendant class

	virtual ~Info() = 0;

	/// functions to pack/unpack the info
	virtual void pack(PackedData&) const;
	virtual void unpack(PackedData&);	///< unpacks onto ITSELF

	/// functions that provide input from a file, give warning if used unoverloaded
	virtual void readData(Input::DataSource&);
	virtual void readProfile(Input::ProfileSource&);

	/// function returning the "virtual" content's size (for output purposes)
	/// doesn't need to match "byte-wise" size
	/// may be overriden, by default is equal to byteSize()-sizeof(BaseInfo)
	virtual Size getSize() const;

	/// for debug printing, used by operator>>
	virtual void print(std::ostream&) const;

	/// virtual method implemented automatically in InfoHandlerWrapper
	/// returns object that does things with Info
	virtual const InfoHandler& getInfoHandler() const = 0;

    private:
	
	/// these must NOT be confused with the correct ones (const wise)!
	/// - conflicting return types on purpose, so that it would not compile
	virtual int pack(PackedData&);		///< USE void pack(PackedData&) const;
	virtual int unpack(PackedData&) const;	///< USE void unpack(PackedData&);
	
	/// we don't want operator=, use InfoManager::duplicateInfo for that
	/// left unimplemented
	Info& operator=(const Info&);
};

//===============================================================================

inline std::ostream& operator<<(std::ostream& os, const Info& i)
{
    i.print(os);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Info* const i)
{
    if(i) { i->print(os); }
    else  { os << "NULL"; }
    return os;
}

} //namespace

#endif

