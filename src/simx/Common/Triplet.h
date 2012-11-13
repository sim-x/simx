//--------------------------------------------------------------------------
// $Id: Triplet.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    triplet.h
// Module:  Common
// Author:  Keith Bisset
// Created: October 30 2003
//
// Description: triplet class based on std::pair
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef INCLUDE_COMMON_TRIPLET
#define INCLUDE_COMMON_TRIPLET

//--------------------------------------------------------------------------

namespace Common
{

  //--------------------------------------------------------------------------

  /// \class triplet triplet.h "Common/triplet.h"
  ///
  /// \brief extension of std::pair to three members
  ///
  template <class T1, class T2, class T3>
  struct triplet {

    typedef T1 first_type;
    typedef T2 second_type;
    typedef T3 third_type;

    /// First data member
    T1 first;
    /// Second data member
    T2 second;
    /// Third data member
    T3 third;
    triplet() : first(), second(), third() {}

    triplet(const T1& a, const T2& b, const T3& c)
      : first(a), second(b), third(c)
    {}

    template <class U1, class U2, class U3>
    triplet(const triplet<U1, U2, U3>& p)
      : first(p.first), second(p.second), third(p.third)
    {}
  };

  //--------------------------------------------------------------------------

  template <class T1, class T2, class T3>
  inline bool operator==
  (const triplet<T1, T2, T3>& x, const triplet<T1, T2, T3>& y)
  {
    return x.first == y.first &&
	  x.second == y.second &&
	   x.third == y.third;
  }

  //--------------------------------------------------------------------------

  template <class T1, class T2, class T3>
  inline bool operator<
    (const triplet<T1, T2, T3>& x, const triplet<T1, T2, T3>& y)
  {
    if (x.first == y.first)
      if (x.second == y.second)
	return x.third < y.third;
      else
	return x.second < y.second;
    else
      return x.first < y.first;
  }

  //--------------------------------------------------------------------------

  template <class T1, class T2, class T3>
  inline bool operator!=
  (const triplet<T1, T2, T3>& x, const triplet<T1, T2, T3>& y)
  {
    return !(x == y);
  }

  //--------------------------------------------------------------------------

  template <class T1, class T2, class T3>
  inline bool operator>
  (const triplet<T1, T2, T3>& x, const triplet<T1, T2, T3>& y)
  {
    return y < x;
  }

  //--------------------------------------------------------------------------

  template <class T1, class T2, class T3>
  inline bool operator<=
  (const triplet<T1, T2, T3>& x, const triplet<T1, T2, T3>& y)
  {
    return !(y < x);
  }

  //--------------------------------------------------------------------------

  template <class T1, class T2, class T3>
  inline bool operator>=
  (const triplet<T1, T2, T3>& x, const triplet<T1, T2, T3>& y)
  {
    return !(x < y);
  }

  //--------------------------------------------------------------------------

  template <class T1, class T2, class T3>
  inline triplet<T1, T2, T3> maketriplet(T1 x, T2 y, T3 z)
  {
    return triplet<T1, T2, T3>(x, y, z);
  }

} // namespace Common

//--------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------
