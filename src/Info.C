//--------------------------------------------------------------------------
// File:    Info.C
// Module:  simx
// Author:  Lukas Kroc
// Created: November 3  2004
//
// Description: base class for objects to send as Infos
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Info.h"
#include "simx/InfoManager.h"

#include "Common/Assert.h"

namespace simx {

Info::~Info()
{
}

// automatic packing, does simple data correctly, need not be overloaded for simple cases
void Info::pack(PackedData& dp) const
{
    // get the size of the structure (w/o VMT):
    unsigned infoSize = getInfoHandler().getByteSize() - sizeof(Info);
    Logger::debug3() << "Info: automatic packing of " << getInfoHandler().getClassType() << " with size=" << infoSize << std::endl;
    SMART_ASSERT( infoSize>=0 );

    // cast the object as a char array and skip the VMT stuff (everythig in Info):
    const char* data = reinterpret_cast<const char*>(this) + sizeof(Info);	// cast it as a char array

    // and now put it into dp:
    dp.add( infoSize );
    dp.add_array(infoSize, data);
}

// automatic unpacking, does simple data, need not be overloaded for simple cases
void Info::unpack(PackedData& dp)
{
    // we assume that the size of the structure comes first:
    unsigned infoSize;
    dp.get( infoSize );
    Logger::debug3() << "Info: automatic unpacking of " << getInfoHandler().getClassType() << " with size=" << infoSize << std::endl;
    SMART_VERIFY( infoSize == getInfoHandler().getByteSize()-sizeof(Info) )(infoSize)(getInfoHandler().getByteSize())(sizeof(Info));

    // now fill in the space appropriately
    dp.get_array(infoSize, reinterpret_cast<char*>(this)+sizeof(Info));	// get the data part from "dp"
}

void Info::readData(Input::DataSource&)
{
    Logger::warn() << "Info: empty readData used in type " << getInfoHandler().getClassType() << std::endl;
}

void Info::readProfile(Input::ProfileSource&)
{
    Logger::warn() << "Info: empty readProfile used in type " << getInfoHandler().getClassType() << std::endl;
}

Size Info::getSize() const
{
    Size infoSize = static_cast<Size>( getInfoHandler().getByteSize() - sizeof(Info) );
    SMART_ASSERT( infoSize>=0 );

    return infoSize;
}

void Info::print(std::ostream& os) const
{
    os << "Info";
}

//===============================================================================

/// these must NOT be confused with the correct ones (const wise)!
/// - conflicting return types on purpose, so that it would not compile
/// - they must be virtual, and therefore must have an implementation
int Info::pack(PackedData&)
{
    SMART_ASSERT( false );
    return 0;
}

int Info::unpack(PackedData&) const
{
    SMART_ASSERT( false );
    return 0;
}


} //namespace

