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
// $Id: cfstreambuf.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    cfstreambuf.h
// Module:  File
// Author:  Randall Bosetti
// Created: June 07 2004
// Description:
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_FILE_CFSTREAMBUF
#define NISAC_FILE_CFSTREAMBUF

#include "simx/File/fstream.h"
#include "simx/Common/Exception.h"

#include <streambuf>
#include <iostream>
#include <fstream>
#include <string>

//--------------------------------------------------------------------------

namespace File {

  typedef std::string (*newname_f)(int num);

  class cfstreambuf : 
    public std::basic_streambuf<char, std::char_traits<char> >
  {

  public:

    cfstreambuf( int limit, std::string basename,
		 std::string header = "", newname_f = 0);

    ~cfstreambuf() 
    {
      sync(); 
      fStr.flush();
      fStr.close(); 
    }

  protected:

    int_type overflow(const int_type c = traits_type::eof());
    int sync();

  private:

    static newname_f sfNewname;
    static std::string ssHeader;
    File::ofstream  fStr;
    unsigned int       fLimit;
    int       fNumfiles;
    int       fCursize; // potential speedup
    std::string fHeader;
    std::string fBasename;
    newname_f fNewname;
    std::string fBuffer;

    cfstreambuf(const cfstreambuf&);
    cfstreambuf& operator=(const cfstreambuf&);
  };

  //--------------------------------------------------------------------------

  inline cfstreambuf::cfstreambuf( int limit,
				   std::string basename,
				   std::string header,
				   newname_f namefunc ) :
    fLimit(limit),
    fNumfiles(0),
    fBasename(basename)
  {

    if (!header.length())
    {
      fHeader = ssHeader;
    }
    else
    {
      fHeader = header;
    }

    if (!namefunc)
    {
      fNewname = sfNewname;
    }
    else
    {
      fNewname = namefunc;
    }

    fStr.open(fBasename.c_str());
    fStr << fHeader << std::endl;
    fCursize = fStr.tellp();
    fBuffer.erase();
  }

  //--------------------------------------------------------------------------

  inline int cfstreambuf::sync() {
    try {
      if ( (fCursize + fBuffer.length()) >= fLimit) 
      {
        if (fNumfiles == 0) 
	{
          std::string nnewname = fBasename + "." + 
				 fNewname(fNumfiles);
          std::rename(fBasename.c_str(),nnewname.c_str());
        }
    
	++fNumfiles;
        std::string newname = fBasename + "." + 
			      fNewname(fNumfiles);
        fStr.flush();
        fStr.close();
        fStr.open(newname);
        fCursize = fHeader.length() + 1;
        fStr << fHeader << std::endl;
      }

      fStr << fBuffer;
      fCursize += fBuffer.length();
      fStr.flush();
      fBuffer.erase();
      return(0);
    } // try
    catch (...) 
    {
      throw Common::Exception("Error in cfstreambuf::sync\()");
    }

  }

  //--------------------------------------------------------------------------

  inline int cfstreambuf::overflow(int c)
  {
    fBuffer.insert(fBuffer.end(), c);
    return c;
  }

} // end namespace File

//--------------------------------------------------------------------------
#endif // NISAC_FILE_CFSTREAMBUF
//--------------------------------------------------------------------------
