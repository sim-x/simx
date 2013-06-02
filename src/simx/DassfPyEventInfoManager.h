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
// File:    DassfPyEventInfoManager.h
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: April 29 2013
//
// Description: 
//	Event for Info deliveries, DaSSF internals wrap
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_DASSF_PY_EVENT_INFO_MANAGER
#define NISAC_SIMX_DASSF_PY_EVENT_INFO_MANAGER

#include "simx/type.h"
#include "simx/DassfEvent.h"
#include "simx/PyEventInfoManager.h"

#ifdef SIMX_USE_PRIME


namespace simx {

class PyEventInfoManager;

/// Wrap around PyEventInfoManager to hide DaSSF internals
class DassfPyEventInfoManager : public DassfEvent
{
    SSF_DECLARE_EVENT(DassfPyEventInfoManager);

    public:
	/// Construct an event from a data stream message
	explicit DassfPyEventInfoManager(minissf::CompactDataType* dp);
	/// constructor from PyEventInfoManager
	explicit DassfPyEventInfoManager(const PyEventInfoManager&);

        virtual ~DassfPyEventInfoManager();

        /// Makes a copy
        virtual DassfPyEventInfoManager* clone() const;
  
	/// pack PyEventInfoManager
        virtual minissf::CompactDataType* pack(minissf::CompactDataType*);
        virtual int pack(char* buf, int bufsize);
  
	/// unpack PyEventInfoManager
        static minissf::Event* unpack(minissf::CompactDataType*);
        static minissf::Event* unpack(char* buf, int bufsize);
        /// Execute operation
        virtual void execute(LP& lp);

    protected:
    private:

	/// the actual PyEventInfoManager
	PyEventInfoManager 	fPyEventInfoManager;	
};

//==============================================================

inline DassfPyEventInfoManager::DassfPyEventInfoManager(minissf::CompactDataType* dp)
  : DassfEvent( dp ),
    fPyEventInfoManager()
{
    PackedData pd(dp);
    fPyEventInfoManager.unpack( pd );
}

inline DassfPyEventInfoManager::DassfPyEventInfoManager(const PyEventInfoManager& e)
  : DassfEvent(),
    fPyEventInfoManager( e )
{
}

inline DassfPyEventInfoManager::~DassfPyEventInfoManager()
{
}

inline DassfPyEventInfoManager* DassfPyEventInfoManager::clone() const
{
    return new DassfPyEventInfoManager(*this);
}

inline minissf::CompactDataType* DassfPyEventInfoManager::pack(minissf::CompactDataType* dp)
{
    PackedData pd(dp);
    fPyEventInfoManager.pack( pd );
    return dp;
}

  inline int DassfPyEventInfoManager::pack(char* buf, int bufsize)
  {
    SMART_ASSERT(false)("DassfPyEventInfoManager::pack should never be called");
    return 0;
  }


inline minissf::Event* DassfPyEventInfoManager::unpack(minissf::CompactDataType* dp)
{
    return new DassfPyEventInfoManager(dp);
}

  inline minissf::Event* DassfPyEventInfoManager::unpack(char* buf, int bufsize)
  {
    SMART_ASSERT(false)("DassfEventInfoManager::unpack should never be called");
    // return new DassfEventInfoManager(dp);
    return NULL;
  }


inline void DassfPyEventInfoManager::execute(LP& lp)
{
    fPyEventInfoManager.execute();
}


} // namespace

#endif //SIMX_USE_PRIME

#endif 
