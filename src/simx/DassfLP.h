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
// File:    DassfLP.h
// Module:  simx
// Author:  K. Bisset, Sunil Thulasidasan
// Created: June 25 2004
//
// Description:
//	wrapper for LP to hide all DaSSF internals
//
// @@
//
//--------------------------------------------------------------------------

#ifdef SIMX_USE_PRIME

#ifndef NISAC_SIMX_DASSFLP
#define NISAC_SIMX_DASSFLP

#include "simx/type.h"




#include "ssf.h"

#include <vector>

namespace simx {
class LP;

/// wrapper for LP to hide all DaSSF internals
class DassfLP : public minissf::Entity
{
public:
  /// Construct an DassfLP.
  /// \param lp is the host LP, must be globally unique
    DassfLP(const LPID id, LP& lp);

  virtual ~DassfLP();
  
  //this will be called by LP in its constructor
  //need to call this before messages are sent on channels
  void mapChannels(); 

   /// Run  before begining of simulation.
  virtual void init();

  /// Run after end of simulation.
  /// TODO: not yet implemented
  virtual void wrapup();

  /// ssf process code
//------------------------ EMBEDDED CODE STARTS ----------------------------//
minissf::Procedure* _ssf_create_procedure_process(minissf::Process* _ssf_omit_0, void* retaddr = 0);
void process(minissf::Process*); 
//------------------------ EMBEDDED CODE ENDS ----------------------------//
  
  /// send events
  void sendDassfEvent(const LPID destLP, minissf::Event* e, const Time delay);

protected:
private:
  typedef std::set<minissf::inChannel*> InChannelSet;
  typedef std::set<minissf::outChannel*> OutChannelSet;
  InChannelSet fInSet;
  OutChannelSet fOutSet;
  std::vector<InChannelSet::iterator> fIn;  ///< Incomming connections 
  std::vector<OutChannelSet::iterator> fOut; ///< Outgoing connections
  

  LP& fLP; ///< the host LP

   /// these shouldn't be needed
  DassfLP(const DassfLP& rhs);
  DassfLP& operator=(const DassfLP& rhs);
};

  /// ssf process code
  class EventProcess : public minissf::Process {
  public:
    EventProcess(DassfLP* owner) : Process(owner) {}
//------------------------ EMBEDDED CODE STARTS ----------------------------//
virtual minissf::Procedure* _ssf_create_procedure_action(); 
virtual void action(); 
//------------------------ EMBEDDED CODE ENDS ----------------------------//
  };

} 

#endif 

#endif  //ifdef SIMX_USE_PRIME
