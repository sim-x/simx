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
// $Id: configio.C,v 1.1.1.1 2011/08/18 22:19:44 nsanthi Exp $
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

#include "simx/Config/configio.h"

#include <cstring>

//-------------------------------------------------------------------------

namespace Config {

  /*  Read a record from a configuration file.  Comment specifies what
      a comment string is.  Return nonzero if the record was
      successfully read, or zero if not.
  */

  int ConfigRead(FILE* file, TConfigRecord* record, const char* comment)
  {
    static char buffer[MAX_LINE_LENGTH];
    char* c;    /* beginning of non-white space */
    char* d;    /* end of non-white space */
    size_t len;

    len = strlen(comment);
    buffer[0] = 0;

    record->fKey[0] = 0;
    record->fValue[0] = 0;
    record->fComment[0] = 0;

    if (fgets(buffer, MAX_LINE_LENGTH, file) == 0)
      return 0;

    /* Find beginning of 1st string (key) skipping all leading spaces
       and tabs */
    c = buffer;
    while (*c == ' ' || *c == '\t' || *c == '\n')
      ++c;

    /* If comment line, return */
    if (strncmp(c, comment, len) == 0) {
      /* Find end of third string (comment) */
      d = c;
      while (*d != '\n' && *d != 0)
        ++d;

      /* +len skips comment character, -1 removes newline */
      memcpy(record->fComment, c+len, d - c);
      record->fComment[d - c - 1] = 0;
      return 1;
    }

    /* Find end of first string (key) */
    d = c;
    while (*d != ' ' && *d != '\t' && *d != '\n' && *d != 0 && 
           (strncmp(d, comment, len) != 0))
      ++d;

    /* Key found so copy to data structure */
    if (strncmp(c, comment, len) != 0) {
      memcpy(record->fKey, c, d - c);
      record->fKey[d - c] = 0;
    }

    /* Find beginning of 2nd string (value) skipping all leading
       spaces and tabs */
    c = d;
    while (*c == ' ' || *c == '\t')
      ++c;

    /* Find end of second string (value) */
    /*   allows for blank spaces and tabs in value */
    d = c;
    while (*d != '\n' && *d != 0 && (strncmp(d, comment, len) != 0))
      ++d;
    if (d > c) {
      --d;
      while (*d == ' ' || *d == '\t')
        --d;
      ++d;
    }

    /* Value found so copy to data structure */
    if (strncmp(c, comment, len) != 0) {
      memcpy(record->fValue, c, d - c);
      record->fValue[d - c] = 0;
    }

    /* Find beginning of 3rd string (comment) skipping all leading
       spaces and tabs */
    c = d;
    while (*c == ' ' || *c == '\t')
      ++c;

    if (strncmp(c, comment, len) == 0) {
      /* Find end of third string (comment) */
      d = c;
      while (*d != '\n' && *d != 0)
        ++d;

      memcpy(record->fComment, c, d - c);
      record->fComment[d - c] = 0;
    }

    return 1;
  }

  //-------------------------------------------------------------------------

  /*  Write a record to a configuration file.  Return nonzero if the
      record was successfully written, or zero if not.
  */
  int ConfigWrite(FILE* file, const TConfigRecord* record)
  {
    return fprintf(file, "%s %s %s\n", record->fKey, record->fValue,
                   record->fComment) >= 0;
  }

} // namespace

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
