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
// File:    StringFileData.h
// Module:  File
// Author:  Paula Stretz
// Created: March 4 2003
//
// Description: Reads all of the fields in a file as a single string.
// Can be used as the data class for a FileReader specifying no
// fixed fields.
// Ex:
//      FileReader<StringFileData, 0> reader("filename");
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_FILE_STRINGFILEDATA
#define NISAC_FILE_STRINGFILEDATA
#include <istream>
#include <ostream>
#include <iosfwd>
#include <string>

#include "simx/File/Header.h"

namespace File {
/// \class StringFileData StringFileData.h "File/StringFileData.h"
///
/// \brief Reads all of the fields in a file as a single string.
///
/// Can be used as the data class for a FileReader specifying no
/// fixed fields.
/// Ex:
///      FileReader<StringFileData, 0> reader("filename");
class StringFileData
{
public:
  StringFileData();
  ~StringFileData();

/// Set the file header.
  ///
  /// This is useful for createing a file, the header to be output can
  /// be set.
  void SetHeader(HeaderPtr header);

  /// Read record from stream
  void Read(std::istream& is);

  /// Write record to stream
  void Print(std::ostream& os) const;

  /// Get the record
  std::string& getString();

protected:
private:
	std::string  fString;
	HeaderPtr fHeader;
};

inline void StringFileData::SetHeader(HeaderPtr header)
{
	fHeader = header;
}

/// Stream extraction operator
inline std::istream& operator>>(std::istream& is, StringFileData& d)
{
  d.Read(is);
  return is;
}

/// Stream insertion operator
inline std::ostream& operator<<(std::ostream& os, const StringFileData& d)
{
  d.Print(os);
  return os;
}
} // namespace
#endif // NISAC_FILE_STRINGFILEDATA
