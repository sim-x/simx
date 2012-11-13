// impl/smart_assert_default.hpp

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


/*
    defaults for initializing the SMART_ASSERT

    Define BOOST_SMART_ASSERT_OVERRIDE_DEFAULTS if you don't 
    want them.
*/

#ifndef BOOST_INNER_SMART_ASSERT_DEFAULT_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_DEFAULT_IMPL_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

namespace boost {

namespace smart_assert {
    // initialize the smart_assert library


    // FIXME - for all such things, make sure
    // I make a macro for simpler usage !!!
    smart_assert_initializer &initializer() {
        // FIXME - make safe_log_name(), safe_persist_name(),
        // checking if we can write to the file;
        // if not possible, write to temp. directory or so!
        //
        // also, in each failed assertion, we should include the executable name;
        // should be the Executable name!
        // (win32 - GetModuleFileName)
        static smart_assert_initializer inst( "./assert.txt", "./persist.txt");
        return inst;
    }
    smart_assert_initializer::before_main smart_assert_initializer::s_before_main;


} // namespace smart_assert

} // namespace boost

#endif

