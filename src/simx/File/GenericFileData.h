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
// File:    GenericFileData.h
// Module:  File
// Author:  Keith Bisset
// Created: January  8 2003
// Time-stamp: "2003-09-09 11:47:33 kbisset"
//
// Description: Generic filereader data.  This is useful for giving to
// FileReader and specifing no fixed fields.  Then all columns can be
// accessed as user fields.  Ex:
//      FileReader<GenericFileReader, 0> reader("filename");
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_FILE_GENERICFILEDATA
#define NISAC_FILE_GENERICFILEDATA
#include "simx/File/UserFields.h"
#include <iosfwd>

namespace File {

/// \class GenericFileData GenericFileData.h "File/GenericFileData.h"
///
/// \brief Generic filereader data.
///
/// This is useful for giving to FileReader and specifing no fixed
/// fields.  Then all columns can be accessed as user fields.  Ex:
/// \code FileReader<GenericFileReader, 0> reader("filename"); \endcode
///
template<typename DATA>
class GenericFileData : public UserFields<DATA>
{
public:
  /// Constructor
  GenericFileData();
  ~GenericFileData();

  /// Read from a stream.
  void Read(std::istream& is);

  /// Write to a stream.
  void Print(std::ostream& os) const;

protected:
private:
};


template<typename UserData>
GenericFileData<UserData>::GenericFileData()
{
}

template<typename UserData>
GenericFileData<UserData>::~GenericFileData()
{
}

template<typename UserData>
void GenericFileData<UserData>::Read(std::istream& is)
{
  UserFields<UserData>::Read(is);
}

template<typename UserData>
void GenericFileData<UserData>::Print(std::ostream& os) const
{
  UserFields<UserData>::Print(os);
}

/// Generic filereader where all fields are accessed as user data.
template<class DataType, int NumFixedFields> class FileReader;

/// Generic filereader where all fields are accessed as user data (float).
typedef FileReader<GenericFileData<float>, 0> GenericFileReader;

/// Generic filereader where all fields are accessed as user data (float).
typedef FileReader<GenericFileData<float>, 0> GenericFileReaderFloat;

/// Generic filereader where all fields are accessed as user data (double).
typedef FileReader<GenericFileData<double>, 0> GenericFileReaderDouble;

/// Generic filereader where all fields are accessed as user data (char).
typedef FileReader<GenericFileData<char>, 0> GenericFileReaderChar;

/// Generic filereader where all fields are accessed as user data (short).
typedef FileReader<GenericFileData<short>, 0> GenericFileReaderShort;

/// Generic filereader where all fields are accessed as user data (int).
typedef FileReader<GenericFileData<int>, 0> GenericFileReaderInt;

/// Generic filereader where all fields are accessed as user data (long).
typedef FileReader<GenericFileData<long>, 0> GenericFileReaderLong;

/// Stream extraction operator
template<typename UserData>
inline std::istream& operator>>(std::istream& is, GenericFileData<UserData>& d)
{
  d.Read(is);
  return is;
}

/// Stream insertion operator
template<typename UserData>
inline std::ostream& operator<<(std::ostream& os, const GenericFileData<UserData>& d)
{
  d.Print(os);
  return os;
}

} // namespace
#endif // NISAC_FILE_GENERICFILEDATA
