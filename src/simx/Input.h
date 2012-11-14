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
// File:    Input.h
// Module:  simx
// Authors:  Lukas Kroc, Sunil Thulasidasan
// Created: Feb 6 2005
//
// Description:
//	Base class for inputs  
//	Inputs provide creation-time info for Entities, Services and Infos
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_INPUT
#define NISAC_SIMX_INPUT

#include <boost/python.hpp>

#include "simx/type.h"
#include "simx/ProfileHolder.h"

#include <ostream>
#include <map>

namespace simx {

/// Inputs provide creation-time info for Entities, Services and Infos
/// Data input is one that is instance specific (possibly different values for each object created)
/// Profile input is one which values are shared among a group of instances (each instance has a ProfileID to use)
/// Its a struct because it's just a data collection holder
///
/// - each of descendants MUST have a copy-constructor (default is OK)
/// - profile data is read in only once for each ProfileID, and then copied using the copy-constructor
/// - it's probably a good idea to clear all values to something in default constructor
struct Input
{
    public:
	typedef std::istream	DataSource;	///< which object is to be used to get Data info
	typedef ProfileHolder	ProfileSource;	///< which object is to be used to get Profile info
    
	Input();
	virtual ~Input();

	/// reads in CustomData input
	virtual void readData(DataSource&);
  
  /// sets data input from Python
  virtual void readData( const boost::python::object& );
  
  /// sets profile input from Python
  virtual void readProfile( boost::shared_ptr<PyProfile>& );

	/// reads in Profile input
	virtual void readProfile(ProfileSource&);
	
	/// Debug Printing functions, has operators<< to go with it
	virtual void print(std::ostream&) const;

};

//=======================================================================

//<<<<<<< Input.h
inline Input::Input(void)
{
}

inline Input::~Input(void)
{
}

inline void Input::readData(Input::DataSource&)
{
    /// empty, because it may be that something doesn't need any Data info
}

  inline void Input::readData( const boost::python::object&  data)
  {
    /// empty
  }
  
  inline void Input::readProfile( boost::shared_ptr<PyProfile>&  profile_ptr)
  {
    /// empty
  }


inline void Input::readProfile(Input::ProfileSource&)
{
    /// empty, because it may be that something doesn't need any Profile info
}

inline void Input::print(std::ostream& os) const
{
    os << "Empty Input or Info";
}//                                       Raghupathy Sivakumar

//=======================================================================


//=======
//>>>>>>> 1.5
inline std::ostream& operator<<(std::ostream& os, const Input& p)
{
    p.print(os);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Input* const p)
{
    if(p) { p->print(os); }
    else  { os << "NULL"; }
    return os;
}

} // namespace
#endif 

