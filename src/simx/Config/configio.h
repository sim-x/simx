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
// @@
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
