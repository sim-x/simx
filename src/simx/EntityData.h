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
// File:    EntityData.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 9 2005
//
// Description: Holds data from entity data input file
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_ENTITYDATA
#define NISAC_SIMX_ENTITYDATA

#include "simx/type.h"
#include "simx/Input.h"
#include "simx/Entity.h"

#include "File/UserFields.h"
#include "File/FileReader.h"

#include <sstream>
#include <iosfwd>

namespace simx {
class ProfileEntity;

/// Holds data from entity file
class EntityData : public File::UserFields<float>
{
    public:
	EntityData();
	EntityData(EntityID, Entity::ClassType, ProfileID);
	~EntityData();

	EntityID getEntityId() const;
	Entity::ClassType getClassType() const;
	ProfileID getProfileId() const;
	Input::DataSource& getData();

	void read(std::istream& is);
	void print(std::ostream& os) const;

	/// reader type for this data. Number of fixed fields 
	/// needs to be updated if needed.
	typedef File::FileReader<EntityData, 3> Reader;

    protected:
    private:
	EntityID 		fEntityId;
	Entity::ClassType	fEntityType;
	ProfileID 		fProfileId;
	boost::shared_ptr<std::stringstream>	fData;	///< ptr<> makes it possible not to copy content unnecessarily
};

//=======================================================================

inline std::istream& operator>>(std::istream& is, EntityData& dd)
{
    dd.read(is);
    return is;
}

inline std::ostream& operator<<(std::ostream& os, const EntityData& dd)
{
    dd.print(os);
    return os;
}


} // namespace

#endif 

