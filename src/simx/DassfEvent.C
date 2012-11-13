//--------------------------------------------------------------------------
// $Id: DassfEvent.C,v 1.6 2010/02/28 05:01:39 kroc Exp $
//--------------------------------------------------------------------------
// File:    DassfEvent.C
// Module:  simx
// Author:  K. Bisset
// Created: June 25 2004
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/DassfEvent.h"
#include "simx/DassfEventInfo.h"
#include "simx/DassfEventInfoManager.h"
#include "simx/logger.h"

#ifdef SIMX_USE_PRIME

#include "Common/Assert.h"
#include "Common/backtrace.h"

using namespace std;

namespace simx {

  /// register all derived DassfEvent classes:
  SSF_REGISTER_EVENT(DassfEventInfo, DassfEventInfo::unpack);
  SSF_REGISTER_EVENT(DassfEventInfoManager, DassfEventInfoManager::unpack);

  DassfEvent::DassfEvent()
    : prime::ssf::Event()
  {
  }

  DassfEvent::DassfEvent(const DassfEvent& e)
    : prime::ssf::Event(e)
  {
  }

  DassfEvent::DassfEvent(prime::ssf::ssf_compact* dp)
  {
  }

  DassfEvent::~DassfEvent()
  {
  }

  prime::ssf::ssf_compact* DassfEvent::pack()
  {
    prime::ssf::ssf_compact* dp = new prime::ssf::ssf_compact;
    return pack(dp);
  }

  prime::ssf::ssf_compact* DassfEvent::pack(prime::ssf::ssf_compact* dp)
  {
    SMART_ASSERT( dp );
    return dp;
  }

  prime::ssf::Event* DassfEvent::unpack(prime::ssf::ssf_compact*)
  {
    SMART_VERIFY(0)("DassfEvent can only be used as a base class, therefore unpack should never get called");
    return NULL;
  }

  void DassfEvent::print(ostream& os) const
  {
    os << "DassfEvent";
  }

} // namespace

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

#endif // SIMX_USE_PRIME
