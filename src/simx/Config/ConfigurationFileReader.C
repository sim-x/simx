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
// $Id: ConfigurationFileReader.C,v 1.1.1.1 2011/08/18 22:19:44 nsanthi Exp $
//--------------------------------------------------------------------------
// Project: TRANSIMS
// File:    ConfigurationFileReader.C
// Module:  Config
// Author:
// Created:
// Description: 
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/Config/ConfigurationFileReader.h"
#include "simx/Config/configio.h"
#include "simx/Common/Exception.h"

#include <cstdio>
#include <cstring>

using std::string;

//--------------------------------------------------------------------------

namespace Config {

  //  Construct a configuration file reader 
  ConfigurationFileReader::ConfigurationFileReader (const string& filename, 
                                                    const string& comment)
    : fConfigFilename(filename),
      fAttr(comment),
      fValue(""),
      fComment(comment)
  { 
    fConfigFile = fopen(fConfigFilename.c_str(), "r");
    if (fConfigFile == 0) {
      std::string msg("Couldn't open file ");
      msg += fConfigFilename.c_str();
      throw Common::Exception(msg);
    }
  }

  //--------------------------------------------------------------------------

  //  Destruct a configuration file reader
  ConfigurationFileReader::~ConfigurationFileReader()
  { 
    if (fConfigFile != 0)
      fclose(fConfigFile);
  }

  //--------------------------------------------------------------------------

  //  Get the next record in the file
  void ConfigurationFileReader::GetNextRecord()
  {
    int lenKey;
    int lenValue;
    TConfigRecord record;

    if (ConfigRead(fConfigFile, &record, fComment.c_str()) == 0)
    { 
      fAttr = fComment;
      fValue = record.fComment;
    } else
    {
      lenKey = strlen(record.fKey);
      lenValue = strlen(record.fValue);
      if (lenKey == 0)
      {
	fAttr = fComment;
	fValue = record.fComment;
      } else if (lenValue == 0)
      {
	fAttr = fComment;
	fValue = "";
	if ((strcmp(record.fKey, "{") == 0) || (strcmp(record.fKey, "}") == 0))
	  fAttr = record.fKey;
      } else
      {
	fAttr = record.fKey;
	fValue = record.fValue;
      }
    }
    return;
  }

  //--------------------------------------------------------------------------

  //  Return whether there are any more records in the file
  bool ConfigurationFileReader::MoreRecords() const
  {
    return !feof(fConfigFile);
  }

  //--------------------------------------------------------------------------

  //  Return whether the record is a comment record
  bool ConfigurationFileReader::IsComment() const
  {
    return (fAttr == fComment) ? true : false;
  }

  //--------------------------------------------------------------------------

  //  Return the attribute name
  const string& ConfigurationFileReader::GetAttribute() const
  {
    return fAttr;
  }

  //--------------------------------------------------------------------------

  //  Return the attribute value
  string& ConfigurationFileReader::GetValue()
  {
    return fValue;
  }

  //--------------------------------------------------------------------------

  const std::string& ConfigurationFileReader::GetFilename() const
  {
    return fConfigFilename;
  }

} // namespace

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
