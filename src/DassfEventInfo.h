//--------------------------------------------------------------------------
// File:    DassfEventInfo.h
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

#ifndef NISAC_SIMX_DASSFEVENTINFO
#define NISAC_SIMX_DASSFEVENTINFO

#include "simx/type.h"
#include "simx/DassfEvent.h"
#include "simx/EventInfo.h"

#ifdef SIMX_USE_PRIME


namespace simx {

class EventInfo;

/// Wrap around EventInfo to hide DaSSF internals
class DassfEventInfo : public DassfEvent
{
    SSF_DECLARE_EVENT(DassfEventInfo);

    public:
	/// Construct an event from a data stream message
	explicit DassfEventInfo(prime::ssf::ssf_compact* dp);
	/// constructor from EventInfo
	explicit DassfEventInfo(const EventInfo&);

        virtual ~DassfEventInfo();

        /// Makes a copy
        virtual DassfEventInfo* clone() const;
  
	/// pack EventInfo
        virtual prime::ssf::ssf_compact* pack(prime::ssf::ssf_compact*);
	/// unpack EventInfo
        static prime::ssf::Event* unpack(prime::ssf::ssf_compact*);

        /// Execute operation
        virtual void execute(LP& lp);

    protected:
    private:

	/// the actual EventInfo
	EventInfo 	fEventInfo;
 
};

//==============================================================

inline DassfEventInfo::DassfEventInfo(prime::ssf::ssf_compact* dp)
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

inline prime::ssf::ssf_compact* DassfEventInfo::pack(prime::ssf::ssf_compact* dp)
{
    PackedData pd(dp);
    fEventInfo.pack( pd );
    return dp;
}

inline prime::ssf::Event* DassfEventInfo::unpack(prime::ssf::ssf_compact* dp)
{
    return new DassfEventInfo(dp);
}

inline void DassfEventInfo::execute(LP& lp)
{
    fEventInfo.execute();
}


} // namespace

#endif // SIMX_USE_PRIME

#endif 


