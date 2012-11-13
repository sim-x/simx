// fwd/before_includes.hpp

// Boost.SmartAssert library
//
// Copyright (C) 2003 John Torjo (john@torjo.com)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies.
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty,
// and with no claim as to its suitability for any purpose.
 
// See http://www.boost.org for updates, documentation, and revision history.

// to know the version of the SMART_ASSERT you're using right now, 
// check out <smart_assert/smart_assert/version.txt>

#include <boost/config.hpp>

// thanks Pavel Vozenilek!
#ifdef BOOST_MSVC
#pragma warning (push)

// large names for STL names
#pragma warning (disable : 4786)
// *this used in initializer list - this is ok ;-)
#pragma warning( disable: 4355)
// exception handler used but unwind semantics are not supported (Dinkumware iostreams)
#pragma warning (disable : 4530) 

// '<Unknown>' has C-linkage specified, but returns UDT 'Assert' which is incompatible with C
#pragma warning (disable : 4190) 
#endif


// thanks Pavel Vozenilek!
#ifdef __BORLANDC__
#pragma warn -8027  // functions not expanded inline
#pragma warn -8026  // functions with exception spec not expanded inline
#endif


// thanks Pavel Vozenilek!
#ifdef __BORLANDC__
// if we use different default calling convention
#define BOOST_SMART_ASSERT_CDECL __cdecl
#else
#define BOOST_SMART_ASSERT_CDECL
#endif

