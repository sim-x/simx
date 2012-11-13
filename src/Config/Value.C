//--------------------------------------------------------------------------
// File:    Value.C
// Module:  Config
// Author:  Keith Bisset
// Created: October 15 2002
// Time-stamp: "2004-04-13 17:14:14 keith"
//
// @@COPYRIGHT@@
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
