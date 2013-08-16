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
// File:    DassfEvent.h
// Module:  simx
// Author:  K. Bisset
// Created: June 25 2004
//
// Description:
//	Base for all DassfEvents
//
// @@
//
//--------------------------------------------------------------------------

#ifdef SIMX_USE_PRIME

#ifndef NISAC_SIMX_EVENT
#define NISAC_SIMX_EVENT

#include "simx/type.h"
#include "simx/PackedData.h"



#include "ssf.h"

#include <iosfwd>

namespace simx {
class LP;

/// \class DassfEvent DassfEvent.h "simx/DassfEvent.h"
///
/// \brief Base for all DassfEvents
///
/// 
class DassfEvent : public minissf::Event
{
public:
  /// Default constructor
  DassfEvent();
  /// Copy constructor (required)
  explicit DassfEvent(const DassfEvent&);
  /// Construct from PackedData
  explicit DassfEvent(minissf::CompactDataType*);
  
  /// Destroy a DassfEvent
  virtual ~DassfEvent();
  
  /// Copy a DassfEvent (required)
  virtual minissf::Event* clone() const = 0;
  
  /// Pack a DassfEvent (required)
  virtual minissf::CompactDataType* pack();
  virtual minissf::CompactDataType* pack(minissf::CompactDataType*);
  virtual int pack(char* buf, int bufsiz);
  
  /// Unpack a DassfEvent (required) - MUST be overriden
  //static minissf::Event* unpack(minissf::CompactDataType*);
  static minissf::Event* unpack(char* buf, int bufsize);

  /// Print an event (empty in DassfEvent base)
  virtual void print(std::ostream& os) const;

  /// called when event is executed at destination at the proper time
  virtual void execute(LP& lp) = 0;

protected:
private:
};

  //===================================================================================

} // namespace



#endif // NISAC_SIMX_EVENT

#endif // SIMX_USE_PRIME
