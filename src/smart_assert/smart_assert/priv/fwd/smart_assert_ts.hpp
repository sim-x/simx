// fwd/smart_assert_ts.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_THREAD_SAFE_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_THREAD_SAFE_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif


// thread-safe issues

// thanks Pavel Vozenilek!
#include <boost/config.hpp>
#ifndef BOOST_HAS_THREADS
#undef BOOST_SMART_ASSERT_THREAD_SAFE
#endif

#if defined( BOOST_MSVC)
    // VC has a flag, when threads are enabled
    // (for VC6 - Project/Settings/"C/C++" tab/
    //            Category "Code generation"/ use run-time library)
    #ifdef BOOST_HAS_THREADS
    #undef BOOST_SMART_ASSERT_THREAD_SAFE
    #define BOOST_SMART_ASSERT_THREAD_SAFE
    #endif
#endif

#if defined( BOOST_DISABLE_THREADS)
// threads disabled explicitly
#undef BOOST_SMART_ASSERT_THREAD_SAFE
#endif

#ifdef BOOST_SMART_ASSERT_THREAD_SAFE
#include <boost/thread/recursive_mutex.hpp>
#endif

namespace boost {

namespace smart_assert {

namespace Private {

#ifdef BOOST_SMART_ASSERT_THREAD_SAFE
// thread-safe

// we might enter ASSERT recursively (twice in the same thread);
// therefore, we should use recursive_mutex !!!!! 
typedef boost::recursive_mutex mutex_type;
typedef boost::recursive_mutex::scoped_lock lock_type;


#else
// not thread-safe
struct mutex_type {};
struct lock_type { 
    lock_type( mutex_type &) {}
    ~lock_type() {}
};


#endif

    
} // namespace Private

} // namespace smart_assert

} // namespace boost

#endif 
