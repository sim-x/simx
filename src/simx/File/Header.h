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
// File:    Header.h
// Module:  File
// Author:  Keith Bisset
// Created: July  2 2002
// Time-stamp: "2003-03-03 12:02:12 kbisset"
// Description: 
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_FILE_HEADER
#define NISAC_FILE_HEADER
#include <boost/shared_ptr.hpp>
#include <istream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace File {
  /// \class Header Header.h "File/Header.h"
  ///
  /// \brief Header for a file in the NISAC format.
  class Header
  {
    public:
      /// Construct a header with a given number of (empty) fixed fields.
      explicit Header(int numFixedFields);

      /// Construct a header with a given number of fixed fields, with
      /// given names.
      Header(const char* names[], int numFixedFields);

      /// Construct a header with a given number of fixed fields, with
      /// names parsed from the header line.
      Header(const std::string header_line, int numFixedFields);

      /// Read a header from a stream
      bool Read(std::istream& is);

      /// Write a header to a stream
      void Print(std::ostream& os) const;

      /// Return the entire header as a string
      const std::string& GetHeader() const;

      /// Get the field number for a header with a given name.  Returns
      /// -1 if the field name is not found.
      int GetIndex(std::string field) const;

      /// Get the field name of a particular field
      const std::string& GetFieldName(int index) const;

      /// Return the total number of fields
      int NumberFields() const;

      /// Return the number of fixed fields
      int NumberFixedFields() const;

      /// Return the number of user fields
      int NumberUserFields() const;

      /// Add a field to the header - returns index of that field, -1 if field already exists
      int AddHeaderField(const std::string& field);

      /// Mark a field as ignored so that we don't print it out
      void IgnoreHeaderField(const std::string& field);

      /// check if a field is ignored by string
      bool IsIgnored(const std::string& field);

      /// check if a field is ignored by index (needed for userfields::Print)
      bool IsIgnoredIndex(int index);

      /// Return the number of added user fields
      int GetNumAddedUserFields();

    protected:
    private:
      typedef std::map<std::string, int> IndexMap; 
      IndexMap fIndex;
      int fNumFixedFields;
      std::string fHeader;

      //this is to keep track of fields we want to ignore
      typedef std::map<std::string, bool> IgnoreMap;
      IgnoreMap fIgnore;

      int numAddedUserFields;  //keep track of fields added (used in UF::Read)  
  };

  /// A smart pointer to a header object.
  typedef boost::shared_ptr<Header> HeaderPtr;

  /// Header extraction operator
  inline std::istream& operator>>(std::istream& is, File::Header& h)
  {
    h.Read(is);
    return is;
  }

  /// Header insertion operator
  inline std::ostream& operator<<(std::ostream& os, File::Header& h)
  {
    h.Print(os);
    return os;
  }
} // namespace
#endif // NISAC_FILE_HEADER
