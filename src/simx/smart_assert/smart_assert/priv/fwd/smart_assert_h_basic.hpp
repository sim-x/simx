// fwd/smart_assert_h_basic.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_HANDLERS_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_HANDLERS_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

//
// default (BASIC) handlers
//
// note: these are basic handlers;
//
// for cool features,
// #include <simx/smart_assert/smart_assert_ext.hpp>
//
// this should be #included only once, and then
// set the handler (somewhere at the beginning of main() )

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_util.hpp>


namespace boost { 

namespace smart_assert {

    // forward declaration
    class assert_context;


    // ignores the assert
    void ignore_assert( const assert_context & context);

    // aborts on assert
    void abort_on_assert( const assert_context & context);

    // prints ALL DETAILS of the assertion to console
    inline void print_details_to_console( const assert_context & context);

    // prints a summary of the assertion to console
    void print_summary_to_console( const assert_context & context);

    void throw_exc_on_assert( const assert_context & context);

    void throw_exc_on_assert_detailed( const assert_context & context);


    // basic - doesn't do very much;
    //
    // for cool features, check out smart_assert_h_ext.h !!!
    void basic_ask_user_on_assert( const assert_context & context);

    // Thanks Darren Cook!
    void ask_user_on_assert_console_detailed( const assert_context & context);


    inline void default_handle_warn( const assert_context & context) {
        print_summary_to_console( context);
    }

    inline void default_handle_debug( const assert_context & context) {
        basic_ask_user_on_assert( context);
    }

    inline void default_handle_error( const assert_context & context) {
        throw_exc_on_assert( context);
    }

    inline void default_handle_enforce( const assert_context & context) {
        throw_exc_on_assert_detailed( context);
    }

    inline void default_handle_fatal( const assert_context & context) {
        dump_assert_context_detail( context, std::cout );
        abort_on_assert( context);
    }


} // namespace smart_assert

} // namespace boost





#endif
