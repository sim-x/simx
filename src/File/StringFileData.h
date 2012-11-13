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
// @@COPYRIGHT@@
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
