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
// @@COPYRIGHT@@
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
