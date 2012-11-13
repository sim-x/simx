//--------------------------------------------------------------------------
// File:    ServiceData.h
// Module:  simx
// Author:  Lukas Kroc
// Created: November 29 2004
//
// Description: Holds data from service data input file
//
// @@COPYRIGHT@@
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

