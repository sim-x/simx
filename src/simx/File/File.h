//--------------------------------------------------------------------------
// File:    File.h
// Module:  File
// Author:  Keith Bisset
// Created: August 27 2003
//
// Description: Generic file functions
//
// @@COPYRIGHT@@
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
