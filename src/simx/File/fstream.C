//--------------------------------------------------------------------------
// $Id: fstream.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    fstream.C
// Module:  File
// Author:  Keith Bisset
// Created: August 11 2003
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/File/fstream.h"
#include "simx/File/File.h"
#include "simx/Log/Logger.h"
#include <iostream>

using namespace std;

//--------------------------------------------------------------------------

namespace {

  // since ifstream and ofstrean don't inherit from a common base, the
  // type of the stream must be a template parameter.

  template < class Stream >
  inline void CheckFileState(Stream& file, const std::string filename)
  {
    if (file.bad() || file.fail())
    {
      std::string msg("Couldn't open file ");
      msg += filename;
      Log::log().Failure(File::gMod, msg);
      //throw Global::Exception(msg);
    }
  }

} // namespace

//--------------------------------------------------------------------------

namespace File {

  template < class charT, class traits >
  basic_filebuf<charT, traits>::~basic_filebuf() {}

  //--------------------------------------------------------------------------

  template < class charT, class traits >
  std::basic_filebuf<charT,traits>*
  basic_filebuf<charT, traits>::open(const std::string& filename,
				     std::ios_base::openmode mode)
  {
    return std::basic_filebuf<charT,traits>::open(filename.c_str(), mode);
  }

  //--------------------------------------------------------------------------

  template < class charT, class traits>
  basic_ifstream<charT, traits>::basic_ifstream()
  {}

  //--------------------------------------------------------------------------

  template < class charT, class traits>
  basic_ifstream<charT, traits>::basic_ifstream(const std::string& filename,
						std::ios_base::openmode mode)
    : std::basic_ifstream<charT,traits>(filename.c_str(), mode)
  {
    CheckFileState(*this, filename);
  }

  //--------------------------------------------------------------------------

  template < class charT, class traits>
  basic_ifstream<charT, traits>::~basic_ifstream()
  {}

  //--------------------------------------------------------------------------

  template < class charT, class traits>
  void basic_ifstream<charT, traits>::open(const std::string& filename,
					   std::ios_base::openmode mode)
  {
    std::basic_ifstream<charT,traits>::open(filename.c_str(), mode);
    CheckFileState(*this, filename);
  }

  //--------------------------------------------------------------------------

  template < class charT, class traits>
  basic_ofstream<charT, traits>::basic_ofstream()
  {}

  //--------------------------------------------------------------------------

  template < class charT, class traits>
  basic_ofstream<charT, traits>::basic_ofstream(const std::string& filename,
						std::ios_base::openmode mode)
    : std::basic_ofstream<charT,traits>(filename.c_str(), mode)
  {
    CheckFileState(*this, filename);
  }

  //--------------------------------------------------------------------------

  template < class charT, class traits>
  basic_ofstream<charT, traits>::~basic_ofstream()
  {}

  //--------------------------------------------------------------------------

  template < class charT, class traits>
  void basic_ofstream<charT, traits>::open(const std::string& filename,
					   std::ios_base::openmode mode)
  {
    std::basic_ofstream<charT,traits>::open(filename.c_str(), mode);
    CheckFileState(*this, filename);
  }

  //--------------------------------------------------------------------------

  template < class charT, class traits >
  basic_fstream<charT, traits>::basic_fstream()
  {}

  //--------------------------------------------------------------------------

  template < class charT, class traits>
  basic_fstream<charT, traits>::basic_fstream(const std::string& filename,
					      std::ios_base::openmode mode)
    : std::basic_fstream<charT,traits>(filename.c_str(), mode)
  {
    CheckFileState(*this, filename);
  }

  //--------------------------------------------------------------------------

  template < class charT, class traits>
  basic_fstream<charT, traits>::~basic_fstream()
  {}

  //--------------------------------------------------------------------------

  template < class charT, class traits>
  void basic_fstream<charT, traits>::open(const std::string& filename,
					  std::ios_base::openmode mode)
  {
    std::basic_fstream<charT,traits>::open(filename.c_str(), mode);
    CheckFileState(*this, filename);
  }
} // namespace

//--------------------------------------------------------------------------

// Explicit instantiation on char
template class File::basic_filebuf<char>;
template class File::basic_ifstream<char>;
template class File::basic_ofstream<char>;
template class File::basic_fstream<char>;

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
