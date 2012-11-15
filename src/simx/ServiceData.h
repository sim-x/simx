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
// File:    ServiceData.h
// Module:  simx
// Author:  Lukas Kroc
// Created: November 29 2004
//
// Description: Holds data from service data input file
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_SERVICEDATA
#define NISAC_SIMX_SERVICEDATA

#include "simx/type.h"
#include "simx/Service.h"
#include "simx/Input.h"

#include "File/UserFields.h"
#include "File/FileReader.h"

#include <iosfwd>
#include <sstream>

namespace simx {
class ProfileService;

/// Holds data from service data input file
class ServiceData : public File::UserFields<float>
{
    public:
	ServiceData();
	~ServiceData();

	ServiceName getName() const;
	Service::ClassType getClassType() const;
	ProfileID getProfileId() const;
	Input::DataSource& getData() const;

	void read(std::istream& is);
	void print(std::ostream& os) const;

	typedef File::FileReader<ServiceData, 3> Reader;

    protected:
    private:
	ServiceName 		fName;
	Service::ClassType 	fClassType;
	ProfileID 		fProfileId;
	boost::shared_ptr<std::stringstream>	fData;
};

//==========================================================

inline std::istream& operator>>(std::istream& is, ServiceData& dd)
{
    dd.read(is);
    return is;
}

inline std::ostream& operator<<(std::ostream& os, const ServiceData& dd)
{
    dd.print(os);
    return os;
}


} // namespace

#endif

