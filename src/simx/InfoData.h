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
// File:    InfoData.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 5 2005
//
// Description: Holds data from info file
//	format: <Time> <EntityID> <ServiceAddr> <InfoType> <ProfileID> <info data .....>
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_INFODATA
#define NISAC_SIMX_INFODATA

#include "simx/type.h"
#include "simx/Input.h"
#include "simx/Info.h"

#include "File/UserFields.h"
#include "File/FileReader.h"

#include <sstream>
#include <iosfwd>

namespace simx {
class ProfileInfo;

/// Holds data from info file
class InfoData : public File::UserFields<int>
{
    public:
	InfoData();
	InfoData(const Time, const EntityID&, const ServiceAddress&, 
		 const Info::ClassType&, const ProfileID&);
	~InfoData();

	Time getTime() const;
	EntityID getEntityId() const;
	ServiceAddress getServiceAddress() const;
	Info::ClassType getClassType() const;
	ProfileID getProfileId() const;
	Input::DataSource& getData();

	void read(std::istream& is);
	void readCustomData( std::istream& is );
	void print(std::ostream& os) const;

	/// reader type for this data. Number of fixed fields 
	/// needs to be updated if needed.
	typedef File::FileReader<InfoData, 5> Reader;

    protected:
    private:
	Time			fTime;
	EntityID		fEntityId;
	ServiceAddress		fServiceAddress;
	Info::ClassType		fType;
	ProfileID		fProfileId;
	boost::shared_ptr<std::stringstream>	fData;	///< Ptr<> makes it possible not to copy content unnecessarily
};

//=======================================================================

inline std::istream& operator>>(std::istream& is, InfoData& dd)
{
    dd.read(is);
    return is;
}

inline std::ostream& operator<<(std::ostream& os, const InfoData& dd)
{
    dd.print(os);
    return os;
}


} // namespace
#endif // NISAC_SIMX_INFODATA

