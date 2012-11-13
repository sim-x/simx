//--------------------------------------------------------------------------
// File:    StringFileData.C
// Module:  File
// Author:  Paula Stretz
// Created: March 4, 2003
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/File/StringFileData.h"
#include <ostream>
#include <istream>
#include <string>

using namespace std;

namespace File {

StringFileData::StringFileData() :
fString("")
{
}

StringFileData::~StringFileData()
{
}

void StringFileData::Read(istream& is)
{
	std::getline(is, fString);
}

void StringFileData::Print(ostream& os) const
{
	os << fString;
}

string& StringFileData::getString()
{
	return fString;
}

} // namespace
