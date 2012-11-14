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
// $Id: ProcessStats.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    ProcessStats.h
// Module:  Common
// Author:  Keith Bisset
// Created: August 12 2003
//
// Description: 
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_COMMON_PROCESSSTATS
#define NISAC_COMMON_PROCESSSTATS

#ifdef __APPLE__
#include <malloc>
#else
#include <malloc.h>
#endif

#include <sstream>

//--------------------------------------------------------------------------

namespace Common {

  /// \class ProcessStats ProcessStats.h "Common/ProcessStats.h"
  ///
  /// \brief Return information about the current procress

  class ProcessStats
  {

  public:

    //--------------------------------------------------------------------------

    static inline std::string GetMemInfo()
    {
      //   int arena;    /* non-mmapped space allocated from system */
      //   int ordblks;  /* number of free chunks */
      //   int smblks;   /* number of fastbin blocks */
      //   int hblks;    /* number of mmapped regions */
      //   int hblkhd;   /* space in mmapped regions */
      //   int usmblks;  /* maximum total allocated space */
      //   int fsmblks;  /* space available in freed fastbin blocks */
      //   int uordblks; /* total allocated space */
      //   int fordblks; /* total free space */
      //   int keepcost; /* top-most, releasable (via malloc_trim) space */

      struct mallinfo info = mallinfo();
      std::ostringstream S;
      // The static_cast is to prevent negative memory usage reporting by
      // framework. Processes that use between 2 and 4 GB of memory suffer
      // from wrap-around because uordblks is a signed int in malloc.h
      // This fix will obviously not work when usage exceeds 4 GB.
     
      S << "MEMORY USAGE: " << static_cast<unsigned long>( info.uordblks ) << " ";
      return S.str();
    }

    //--------------------------------------------------------------------------

    /// Return the current memory usage, in bytes
    static inline int MemUsage()
    {
      struct mallinfo info = mallinfo();
      return info.uordblks;
    }

  };

} // namespace

//--------------------------------------------------------------------------
#endif // NISAC_COMMON_PROCESSSTATS
//--------------------------------------------------------------------------
