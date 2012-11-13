//--------------------------------------------------------------------------
// File:    EntityData.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 9 2005
//
// Description: Holds data from entity data input file
//
// @@COPYRIGHT@@
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

