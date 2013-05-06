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
// File:    DassfEventInfoManager.h
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

#ifndef NISAC_SIMX_DASSFEVENTINFOMANAGER
#define NISAC_SIMX_DASSFEVENTINFOMANAGER

#include "simx/type.h"
#include "simx/DassfEvent.h"
#include "simx/EventInfoManager.h"

#ifdef SIMX_USE_PRIME


namespace simx {

class EventInfoManager;

/// Wrap around EventInfoManager to hide DaSSF internals
class DassfEventInfoManager : public DassfEvent
{
    SSF_DECLARE_EVENT(DassfEventInfoManager);

    public:
	/// Construct an event from a data stream message
	explicit DassfEventInfoManager(minissf::CompactDataType* dp);
	/// constructor from EventInfoManager
	explicit DassfEventInfoManager(const EventInfoManager&);

        virtual ~DassfEventInfoManager();

        /// Makes a copy
        virtual DassfEventInfoManager* clone() const;
  
	/// pack EventInfoManager
        virtual minissf::CompactDataType* pack(minissf::CompactDataType*);
  
  /// the real packing function for minissf
  virtual int pack(char* bug,  int bufsize);

	/// unpack EventInfoManager
        static minissf::Event* unpack(minissf::CompactDataType*);
  static minissf::Event* unpack(char* buf, int bufsize);
  

        /// Execute operation
        virtual void execute(LP& lp);

    protected:
    private:

	/// the actual EventInfoManager
	EventInfoManager 	fEventInfoManager;	
};

//==============================================================

inline DassfEventInfoManager::DassfEventInfoManager(minissf::CompactDataType* dp)
  : DassfEvent( dp ),
    fEventInfoManager()
{
    PackedData pd(dp);
    fEventInfoManager.unpack( pd );
}

inline DassfEventInfoManager::DassfEventInfoManager(const EventInfoManager& e)
  : DassfEvent(),
    fEventInfoManager( e )
{
}

inline DassfEventInfoManager::~DassfEventInfoManager()
{
}

inline DassfEventInfoManager* DassfEventInfoManager::clone() const
{
    return new DassfEventInfoManager(*this);
}

inline minissf::CompactDataType* DassfEventInfoManager::pack(minissf::CompactDataType* dp)
{
    PackedData pd(dp);
    fEventInfoManager.pack( pd );
    return dp;
}
  
  inline int DassfEventInfoManager::pack(char* buf, int bufsize)
  {
    SMART_ASSERT(false)("DassfEventInfoManager::pack should never be called");
    return 0;
    // PackedData pd(dp);
    // fEventInfoManager.pack( pd );
    // return dp;
  }

inline minissf::Event* DassfEventInfoManager::unpack(minissf::CompactDataType* dp)
{
    return new DassfEventInfoManager(dp);
}

  inline minissf::Event* DassfEventInfoManager::unpack(char* buf, int bufsize)
  {
    SMART_ASSERT(false)("DassfEventInfoManager::unpack should never be called");
    // return new DassfEventInfoManager(dp);
    return NULL;
  }

inline void DassfEventInfoManager::execute(LP& lp)
{
    fEventInfoManager.execute();
}


} // namespace

#endif //SIMX_USE_PRIME

#endif 


