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
// @@COPYRIGHT@@
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
