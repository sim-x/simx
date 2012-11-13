
// fwd/named_locker.hpp

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


#ifndef BOOST_INNER_SMART_ASSERT_NAMED_LOCKER_INCLUDED
#define BOOST_INNER_SMART_ASSERT_NAMED_LOCKER_INCLUDED

#include <boost/utility.hpp>

// FIXME
namespace boost {
namespace smart_assert { 

/*
    represents a locker, uniquely identified by a name.
    It's sole purpose is to be used to know if a handler functor
    is alredy called from another thread, or from the same thread,
    recursively

 Usage:

    void my_functor( const assert_context & ctx) {
        // note: it does not need to be static;
        // as a matter of fact, it should NOT be static!
        named_locker lock( "smart_assert debug");
        if ( lock.already_called() )
            // already called from another thread, or recursively
            return;

	    // ... here we know we're the not already called
    }

 Note:
    in the future, this will work across dynamic libraries as well
*/
struct named_locker : private ::boost::noncopyable{

    named_locker( const char * name);
    ~named_locker();
    bool already_called() const { return m_condition != 0; }

private:
    // the condition we're locking;
    // if 0, we're already called from another thread or recursively
    named_locker_condition *m_condition;
};


}}

#endif
