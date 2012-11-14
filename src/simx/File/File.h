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
// File:    File.h
// Module:  File
// Author:  Keith Bisset
// Created: August 27 2003
//
// Description: Generic file functions
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_FILE_FILE
#define NISAC_FILE_FILE
#include <string>
#include <sys/types.h>

/// File I/O for text files in the NISAC file format.
namespace File {
/// Return the modification time of a file
time_t ModificationTime(const std::string& filename);

/// Return true if the file exists
bool FileExists(const std::string& filename);

/// Deletes a file. Returns false if the file cannot be deleted, or
/// does not exist
bool Delete(const std::string& filename);

extern int gMod;
} // namespace
#endif // NISAC_FILE_FILE
