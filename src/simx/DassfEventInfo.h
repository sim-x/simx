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
// File:    DassfEventInfo.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 8 2005
//
// Description: 
//	Event for Info deliveries, DaSSF internals wrap
//
// @@
//
//--------------------------------------------------------------------------

#ifdef SIMX_USE_PRIME

#ifndef NISAC_SIMX_DASSFEVENTINFO
#define NISAC_SIMX_DASSFEVENTINFO

#include "simx/type.h"
#include "simx/DassfEvent.h"
#include "simx/EventInfo.h"




namespace simx {

class EventInfo;

/// Wrap around EventInfo to hide DaSSF internals
  class DassfEventInfo : public DassfEvent
  {
    SSF_DECLARE_EVENT(DassfEventInfo);

  public:
    /// Construct an event from a data stream message
    explicit DassfEventInfo(minissf::CompactDataType* dp);
    /// constructor from EventInfo
    explicit DassfEventInfo(const EventInfo&);

    virtual ~DassfEventInfo();

    /// Makes a copy
    virtual DassfEventInfo* clone() const;
  
    /// pack EventInfo
    virtual minissf::CompactDataType* pack(minissf::CompactDataType*);
    /// the real packing function
    virtual int pack(char* buf, int bufsize);
  

    /// unpack EventInfo
    //static minissf::Event* unpack(minissf::CompactDataType*);
    static minissf::Event* unpack(char* buf, int bufsize);

    /// Execute operation
    virtual void execute(LP& lp);

  protected:
  private:

    /// the actual EventInfo
    EventInfo 	fEventInfo;
 
  };

//==============================================================

inline DassfEventInfo::DassfEventInfo(minissf::CompactDataType* dp)
  : DassfEvent( dp ),
    fEventInfo()
{
    PackedData pd(dp);
    fEventInfo.unpack( pd );
}

inline DassfEventInfo::DassfEventInfo(const EventInfo& e)
  : DassfEvent(),
    fEventInfo( e )
{
}

inline DassfEventInfo::~DassfEventInfo()
{
}

inline DassfEventInfo* DassfEventInfo::clone() const
{
    return new DassfEventInfo(*this);
}

inline minissf::CompactDataType* DassfEventInfo::pack(minissf::CompactDataType* dp)
{
  Logger::debug3() << "DassfEventInfo: packing" << std::endl;
    PackedData pd(dp);
    fEventInfo.pack( pd );
    return dp;
}

  /// the real packing function
  inline int DassfEventInfo::pack(char* buf, int bufsize)
  {
#ifdef HAVE_MPI_H
    //int debug_wait = 1;
    //while(debug_wait);
    //Logger::debug3() << "DassfEventInfo::pack buffer size is " << bufsize << std::endl;
    // PackedData pd(buf, bufsize);
    minissf::CompactDataType* cdata = new minissf::CompactDataType; //create a byte stream
    PackedData pd(cdata);
    fEventInfo.pack(pd);
    return cdata->pack_and_delete(buf, bufsize); //pack into buffer and reclaim memory
#else
    SMART_ASSERT(false).
      msg("DassfEventInfo::pack should never be called when MPI is not in use");
#endif
  }

  //inline minissf::Event* DassfEventInfo::unpack(minissf::CompactDataType* dp)
  inline minissf::Event* DassfEventInfo::unpack(char* buf, int bufsize)
  {
#ifdef HAVE_MPI_H
    Logger::debug3() << "DassfEventInfo: unpacking" << std::endl;
    minissf::CompactDataType* cdata = new minissf::CompactDataType; //create a byte stream
    
    cdata->unpack(buf, bufsize);
    return new DassfEventInfo(cdata);
#else
    SMART_ASSERT(false).
      msg("DassfEventInfo::unpack should never be called when MPI is not in use");
#endif
  }

inline void DassfEventInfo::execute(LP& lp)
{
    fEventInfo.execute();
}


} // namespace



#endif 
#endif // SIMX_USE_PRIME

