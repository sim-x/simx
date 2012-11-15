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
// File:    Header.C
// Module:  File
// Author:  Keith Bisset
// Created: July  2 2002
// Time-stamp: "2002-07-30 12:40:28 kbisset"
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/File/Header.h"
#include <sstream>
#include <string>

using namespace std;

  namespace File
{
  Header::Header(int numFixedFields)
    : fNumFixedFields(numFixedFields),
  fHeader(""), numAddedUserFields(0)
  {
  }

  Header::Header(const char* names[], int numFixedFields)
    : fNumFixedFields(numFixedFields),
  fHeader(""), numAddedUserFields(0)
  {
    int i=0;
    while (names[i] != 0)
    {
      fIndex[names[i]] = i;
      fIgnore[names[i]] = false;
      if (i > 0)
        fHeader += " ";
      fHeader += names[i];
      i++;
    }
  }

  Header::Header(const std::string header_line, int numFixedFields) :
    fNumFixedFields(numFixedFields), 
  fHeader(""), numAddedUserFields(0)
  {
    std::string scratch(header_line);
    size_t index;
    index = scratch.find('#');
    // Remove the # character from the string if it exists.
    if (index != string::npos)
    {
      scratch.erase(index, 1);
    }
    stringstream str;
    str << scratch;
    int i = 0;
    while (str.good())
    {
      string val("");
      str >> val;
      fIndex[val] = i;
      fIgnore[val] = false;
      if (i > 0)
        fHeader += " ";
      fHeader += val;
      i++;
    }
  }

  int Header::NumberFields() const
  {
    return fIndex.size();
  }

  int Header::NumberFixedFields() const
  {
    return fNumFixedFields;
  }

  int Header::NumberUserFields() const
  {
    return NumberFields() - NumberFixedFields();
  }

  bool Header::Read(std::istream& is)
  {
    getline(is, fHeader);
    stringstream s;
    s << fHeader;
    int pos=0; 
    while (s)
    {
      std::string name;
      s >> name;
      if (s)
      {
        fIndex[name] = pos;
        fIgnore[name] = false;
      }
      pos++;
    }
    return is;
  }

  /*************************************************************************************
   *  Wanna know what's up with this funky Print function?  Here's my algorithm: We only
   *  want to print out the fields that are not ignored.  Also, we want to print them
   *  out in the order received, that is 0,1,2,..,n  Map order, however, is alphabetical.
   *  Use of two maps is necessary here as each key has two values: an index and if it is
   *  ignored.  So for each number 0 to total number of fields, we iterate through the 
   *  IndexMap.  If that element's value in the IgnoreMap is true, AND that same element 
   *  has the correct index in IndexMap, then we print it out.  It's that simple!
   *************************************************************************************/ 
  
  void Header::Print(std::ostream& os) const
  {
    for(int x = 0; x < NumberFields(); x++)
    {
      for (IndexMap::const_iterator it = fIndex.begin(); it != fIndex.end(); ++it)
      {
        if((!fIgnore.find(it->first)->second) && (it->second == x))
        {
          os << it->first << " ";
        }
      }
    }
  }


  const std::string& Header::GetHeader() const
  {
    return fHeader;
  }

  int Header::GetIndex(std::string field) const
  {
    IndexMap::const_iterator it = fIndex.find(field);
    if (it == fIndex.end())
      return -1;
    else
      return it->second;
  }


  const std::string& Header::GetFieldName(int index) const
  {
    static std::string empty("");
    for (IndexMap::const_iterator it = fIndex.begin(); it != fIndex.end(); ++it)
      if (it->second == index)
        return it->first;
    return empty;
  }

  int Header::AddHeaderField(const std::string& field)
  {
    //check to be sure the field does NOT exist before adding it
    if(GetIndex(field) == -1)
    {
      //Since fIndex is zero based, we can just use the number
      //of fields to obtain our index.
      int idx = NumberFields();
      fIndex[field] = idx;
      fIgnore[field] = false;
      fHeader += " " + field;
      ++numAddedUserFields;

      //the index number of the field we just added
      return idx;
    }
    else
    {
      //it's already there, so just return the field's index
      return GetIndex(field);
    }
  }

  void Header::IgnoreHeaderField(const std::string& field)
  {
    //check to be sure the field exists before ignoring it
    if(GetIndex(field) != -1)
    {
      fIgnore[field] = true;
    }
    else
    {
      //there was an error: the field does not exist
    }
  }

  bool Header::IsIgnored(const std::string& field)
  {
    return fIgnore[field];
  }


  /*********************************************************************
   * This function is a bit similar to the Print function in that we 
   * have to use two maps to get what we want.  Each map has
   * the same keys, but different values.  In other words, each key has
   * two values.
   * *******************************************************************/

  bool Header::IsIgnoredIndex(int index)
  {
    //have to compensate for userfields to all-fields conversion
    index += fNumFixedFields;
    
    for(IndexMap::const_iterator m_itr = fIndex.begin(); m_itr != fIndex.end(); m_itr++)
    {
      if(index == m_itr->second)
      {
        std::string field = m_itr->first;
        return IsIgnored(field);
      } 
    }
    return true;  //default of true seems reasonable, but we should never get here.
  }

  int Header::GetNumAddedUserFields()
  {
    return numAddedUserFields;
  }
} // namespace
