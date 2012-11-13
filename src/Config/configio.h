//-------------------------------------------------------------------------
// $Id: configio.h,v 1.1.1.1 2011/08/18 22:19:44 nsanthi Exp $
//-------------------------------------------------------------------------
//
// File:    configio.h
// Module:  Config
// Author:  
// Created: 7/14/99
// Description: configuration file reader from TRANSIMS
//
// @@COPYRIGHT@@
//
//-------------------------------------------------------------------------

#ifndef CONFIG_CONFIG_IO
#define CONFIG_CONFIG_IO

#include <cstdio>

//-------------------------------------------------------------------------

namespace Config {

  static const int MAX_LINE_LENGTH( 1024 );

  /*  Structure for configuration file records. */
  typedef struct
  {
    /*  The key, if the record has one. */
    char fKey[MAX_LINE_LENGTH];

    /*  The value, if the record has one. */
    char fValue[MAX_LINE_LENGTH];

    /*  The comment, if the record has one. */
    char fComment[MAX_LINE_LENGTH];
  } TConfigRecord;

  ///  Read a record from a configuration file.  return nonzero if
  /// the record was successfully read, or zero if not.
  int ConfigRead(FILE* file, TConfigRecord* record, const char* comment);

  /// Write a record to a configuration file.  return nonzero if the
  /// record was successfully written, or zero if not.
  int ConfigWrite(FILE* file, const TConfigRecord* record);

} // namespace

//-------------------------------------------------------------------------
#endif /* CONFIG_CONFIG_IO */
//-------------------------------------------------------------------------
