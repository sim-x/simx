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
// File:    Value.C
// Module:  Config
// Author:  Keith Bisset
// Created: October 15 2002
// Time-stamp: "2004-04-13 17:14:14 keith"
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/Config/Value.h"
#include <iostream>
using namespace std;

namespace Config {
  Value::Value(std::string value)
    : fValue(value),
      fExpandedValue(value)
  {
    static char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789{_";
    string::size_type pos = value.find("$");
    if (pos == string::npos)
    {
      fHasVariable = false;
      fVarBegin = string::npos;
      fVarEnd = string::npos;
      fVarName = "";
    }
    else
    {
      fHasVariable = true;
      fVarBegin = fValue.find_first_of("$");
      fVarEnd = fValue.find_first_not_of(alphanum, fVarBegin+1);
      if (fVarEnd == string::npos)
        fVarName = fValue.substr(fVarBegin);
      else
        fVarName = fValue.substr(fVarBegin, fVarEnd-fVarBegin);
      if (fVarName[1] == '{')
      {
        fVarName.erase(1, 1);
        fVarEnd++;
      }
    }  
  }

  bool Value::HasVariable() const
  {
    return fHasVariable;
  }
  
  std::string Value::VariableName() const
  {
    return fVarName;
  }

  std::string Value::GetValue() const
  {
    return fValue;
  }

  std::string Value::GetExpandedValue() const
  {
    return fExpandedValue;
  }

  void Value::ReplaceVariable(std::string value)
  {
    if (fHasVariable)
    {
      fExpandedValue.replace(fVarBegin, fVarEnd-fVarBegin, value);
    }
  }

} // namespace
