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
// File:    Value.h
// Module:  Config
// Author:  Keith Bisset
// Created: October 15 2002
// Description: 
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_CONFIG_VALUE
#define NISAC_CONFIG_VALUE
#include <string>

namespace Config {
/// Hold the value of a configuration key.
/// Also handles values which are or conatin variables.
/// A variable starts with a '$' and ends with a character that is not
/// a letter or number.  The name of the variable can also be
/// contained in {} (e.g., ${var}).
  class Value
  {
  public:
    /// Construct a value from the string found in the config file
    Value(std::string fValue);

    /// Does this value contain a variable
    bool HasVariable() const;

    /// Get the original value
    std::string GetValue() const;

    /// Expand variables in the value.
    /// If the value has a variable and ReplaceVariable() has been
    /// called, return the value with the variable replaced, otherwise
    /// return the original value
    std::string GetExpandedValue() const;

    /// Get the name of the variable, or "" if not variable.
    std::string VariableName() const;

    /// Replace the variable with the string.
    void ReplaceVariable(std::string replacement);
    
  protected:
  private:
    // Value as read from configuration file
    std::string fValue;

    // Does fValue contain a variable
    bool fHasVariable;

    // Where does the variable name start and end
    std::string::size_type fVarBegin;
    std::string::size_type fVarEnd;

    // Canonical name of variable ({} removed)
    std::string fVarName;

    // The expanded value
    std::string fExpandedValue;

  };
} // namespace
#endif // NISAC_CONFIG_VALUE
