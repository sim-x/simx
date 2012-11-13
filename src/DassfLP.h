//--------------------------------------------------------------------------
// File:    DassfLP.h
// Module:  simx
// Author:  K. Bisset
// Created: June 25 2004
//
// Description:
//	wrapper for LP to hide all DaSSF internals
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_DASSFLP
#define NISAC_SIMX_DASSFLP

#include "simx/type.h"

#ifdef SIMX_USE_PRIME


#include "ssf.h"

#include <vector>

namespace simx {
class LP;

/// wrapper for LP to hide all DaSSF internals
class DassfLP : public prime::ssf::Entity
{
public:
  /// Construct an DassfLP.
  /// \param lp is the host LP, must be globally unique
  DassfLP(const LPID id, LP& lp);

  virtual ~DassfLP();

  /// Run at before begining of simulation.
  virtual void init();

  /// Run at after end of simulation.
  /// TODO: not yet implemented
  virtual void wrapup();

  /// Called by DaSSF to process events.  
  void process(prime::ssf::Process*); //! SSF PROCEDURE SIMPLE

  /// send events
  void sendDassfEvent(const LPID destLP, prime::ssf::Event* e, const Time delay);

protected:
private:
  std::vector<prime::ssf::inChannel*> fIn;   ///< Incomming connections
  std::vector<prime::ssf::outChannel*> fOut; ///< Outgoing connections
  
  LP& fLP;	///< the host LP

  /// these shouldn't be needed
  DassfLP(const DassfLP& rhs);
  DassfLP& operator=(const DassfLP& rhs);
};

} // namespace

#endif // SIMX_USE_PRIME

#endif // NISAC_SIMX_DASSFLP


