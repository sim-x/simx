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
// $Id: basic_type_wrapper.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    basic_type_wrapper.h
// Module:  Common
// Author:  K. Bisset
// Created: November  3 2004
//
// Description: A class that acts like a fundamental type.  This is
// useful in debugging.  If you have a set of typedefs to fundamental
// types, and you want to ensure that they are used consistantly,
// define them as classes derived from basic_type_wrapper.  They will
// be implicitly converted to/from the fundamental type, but can't be
// assigned to each other.
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_COMMON_BASIC_TYPE_WRAPPER
#define NISAC_COMMON_BASIC_TYPE_WRAPPER

#include <iostream>

//--------------------------------------------------------------------------

namespace Common {

  template<class T>
  class basic_type_wrapper
  {
  public:
    basic_type_wrapper() : value(0)
    {}

    basic_type_wrapper(T v) : value(v)
    {}

    basic_type_wrapper(const basic_type_wrapper<T>& t) : value(t())
    {}

    T operator()()
    {return value;}

    const T operator()() const 
    {return value;}

    T* operator&()
    {return &value;}

    // Conversion operators

    operator const T() const
    {return value;}

    T operator ++()
    {return ++value;}

    T operator ++(int)
    {return value++;}

    T operator =(const basic_type_wrapper<T>& lhs)
    {value = lhs(); return *this;}

  protected:
    T value;
  };

  //--------------------------------------------------------------------------

  template<class T>
  std::ostream& operator<< (std::ostream& s, const basic_type_wrapper<T>& t)
  {
    return s << t();
  }

  //--------------------------------------------------------------------------

  template<class T>
  std::istream& operator>> (std::istream& s, basic_type_wrapper<T>& t)
  {
    T v;
    s >> v;
    t = v;
    return s;
  }

} // namespace

//--------------------------------------------------------------------------
#endif // NISAC_COMMON_BASIC_TYPE_WRAPPER
//--------------------------------------------------------------------------
