//--------------------------------------------------------------------------
// $Id: ConfigurationFileReader.h,v 1.1.1.1 2011/08/18 22:19:44 nsanthi Exp $
//--------------------------------------------------------------------------
// Project: TRANSIMS
// File:    ConfigurationFileReader.h
// Module:  Config
// Author:
// Created:
// Description: 
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_CONFIG_CONFIGURATIONFILEREADER_H
#define NISAC_CONFIG_CONFIGURATIONFILEREADER_H

//--------------------------------------------------------------------------

#include <cstdio>
#include <string>

//--------------------------------------------------------------------------

namespace Config {

  ///  A configuration file reader reads a configuration key-value pairs from 
  ///  a file.
  class ConfigurationFileReader
  {

  public:

    ///  Construct a configuration file reader 
    ConfigurationFileReader (const std::string& filename,
			     const std::string& comment="#");

    ///  Destruct a configuration file reader
    ~ConfigurationFileReader();

    ///  Reset the iteration over the file
    void Reset();

    ///  Get the next record in the file
    void GetNextRecord();

    ///  Return whether there are any more records in the file
    bool MoreRecords() const;

    ///  Return whether the record is a comment record
    bool IsComment() const;

    ///  Return the attribute name (Key)
    const std::string& GetAttribute() const;
  
    ///  Return the attribute value
    std::string& GetValue();

    /// Return the current filename
    const std::string& GetFilename() const;
    
  private:

    //  Each reader has a file name
    const std::string fConfigFilename;

    //  Each reader has a file handle
    FILE*  fConfigFile;

    //  Each record in a config file conatins an Attribute name (Key)
    std::string fAttr;

    //  Each record in a config file conatins an Attribute value
    std::string fValue;

    //  Comment std::string - std::string used to denote a comment
    //  line in the config file
    std::string fComment;
  };

} // namespace

//--------------------------------------------------------------------------
#endif // NISAC_CONFIG_CONFIGURATIONFILEREADER_H
//--------------------------------------------------------------------------
