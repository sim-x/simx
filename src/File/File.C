//--------------------------------------------------------------------------
// File:    File.C
// Module:  File
// Author:  Keith Bisset
// Created: August 27 2003
//
// @@COPYRIGHT@@
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
