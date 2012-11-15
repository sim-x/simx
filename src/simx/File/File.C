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
// File:    File.C
// Module:  File
// Author:  Keith Bisset
// Created: August 27 2003
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/File/File.h"
#include "simx/Log/Logger.h"
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

using namespace std;

namespace File {
int gMod = Log::log().registerModule("File");

time_t ModificationTime(const std::string& filename)
{
  struct stat stat_buf;
  int ret = ::stat(filename.c_str(), &stat_buf);
  if (ret != 0)
  {
    cout << "Couldn't stat file " << filename << endl;
    perror("stat");
    return 0;
  }
  return stat_buf.st_mtime;
}

bool FileExists(const std::string& filename)
{
  struct stat statbuf;
  int ret = stat(filename.c_str(), &statbuf);
    // Should also check if a regular file
  return (ret == 0);
}

bool Delete(const std::string& filename)
{
  int ret = unlink(filename.c_str());
  return (ret == 0);
}

} // namespace
