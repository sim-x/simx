//--------------------------------------------------------------------------
// File:    DassfEventInfoManager.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 8 2005
//
// Description: 
//	Event for Info deliveries, DaSSF internals wrap
//
// @@COPYRIGHT@@
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
	explicit DassfEventInfoManager(prime::ssf::ssf_compact* dp);
	/// constructor from EventInfoManager
	explicit DassfEventInfoManager(const EventInfoManager&);

        virtual ~DassfEventInfoManager();

        /// Makes a copy
        virtual DassfEventInfoManager* clone() const;
  
	/// pack EventInfoManager
        virtual prime::ssf::ssf_compact* pack(prime::ssf::ssf_compact*);
	/// unpack EventInfoManager
        static prime::ssf::Event* unpack(prime::ssf::ssf_compact*);

        /// Execute operation
        virtual void execute(LP& lp);

    protected:
    private:

	/// the actual EventInfoManager
	EventInfoManager 	fEventInfoManager;	
  
};

//==============================================================

inline DassfEventInfoManager::DassfEventInfoManager(prime::ssf::ssf_compact* dp)
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

inline prime::ssf::ssf_compact* DassfEventInfoManager::pack(prime::ssf::ssf_compact* dp)
{
    PackedData pd(dp);
    fEventInfoManager.pack( pd );
    return dp;
}

inline prime::ssf::Event* DassfEventInfoManager::unpack(prime::ssf::ssf_compact* dp)
{
    return new DassfEventInfoManager(dp);
}

inline void DassfEventInfoManager::execute(LP& lp)
{
    fEventInfoManager.execute();
}


} // namespace

#endif //SIMX_USE_PRIME

#endif 


