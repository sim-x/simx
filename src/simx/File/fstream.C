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
// $Id: fstream.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    fstream.C
// Module:  File
// Author:  Keith Bisset
// Created: August 11 2003
//
// @@
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
