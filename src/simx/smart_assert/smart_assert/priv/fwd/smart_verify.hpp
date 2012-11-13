// fwd/smart_verify.hpp

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

#ifndef BOOST_INNER_SMART_VERIFY_HPP_INCLUDED
#define BOOST_INNER_SMART_VERIFY_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert.hpp>
#include <boost/config.hpp>

namespace boost {

namespace smart_assert {

/*
    Usage: 
        just like SMART_ASSERT, but
        works in Release mode as well !!!

    The only difference:
    if the ASSERTion fails, it's treated as an ERROR
    (which means that by default, a smart_assert_error will be triggered)
*/
#define SMART_VERIFY(expr) \
    if ( (expr)) ; else \
        boost::smart_assert::make_assert().test_expr( (expr), #expr).SMART_ASSERT_CONTEXT.SMART_ASSERT_A \
    /**/

// ... for use with 'v_' macro
#define SMART_VERIFY_V_(expr) \
    if ( boost::smart_assert::Private::keeper boost_private_assert_keeper = 0) ; else \
    if ( boost_private_assert_keeper.test_expr((expr),#expr) ) ; else \
        boost::smart_assert::make_assert().error().SMART_ASSERT_CONTEXT.print_prewrittern_values( boost_private_assert_keeper).SMART_ASSERT_A \
    /**/

/*
    Usage:
        just like SMART_ASSERT, but
        works in Release mode as well !!!

    In case the assertion fails, it returns a value 
    (which should signal something invalid)

    You could use this to validate your function's input.
    For instance, in case your input data is invalid, 
    return an error code.
    Otherwise, process the data.

      Example:
        // calculate n!
        int factorial( int n) {
            SMART_VERIFY_RET( n >= 0)(n) .ret_on_fail(-1);
        }

    IMPORTANT
        always use .ret_on_fail() as the last function called on a SMART_VERIFY_RET.
        Example:

        // ok
        SMART_VERIFY_RET( n >= 0)(n) .msg("invalid input").ret_on_fail(-1);

        // error
        SMART_VERIFY_RET( n >= 0)(n) .ret_on_fail(-1).msg("invalid input");

*/
#define SMART_VERIFY_RET(expr) \
    if ( boost::smart_assert::Private::keeper boost_private_assert_keeper = 0) ; else \
    if ( boost_private_assert_keeper.test_expr((expr),#expr) ) ; else \
        return boost::smart_assert::make_assert().SMART_ASSERT_CONTEXT.print_prewrittern_values( boost_private_assert_keeper).SMART_ASSERT_A \
    /**/


#ifndef BOOST_MSVC
// FIXME (nice to have) - find out which version 
// of MSVC allows
// void f() { return (void)0; }

/*
    just like SMART_VERIFY_RET, in case your function returns void.

    (Of course, in this case, it's useless to call .ret_on_fail()) 
*/
#define SMART_VERIFY_RET_VOID(expr) \
    if ( boost::smart_assert::Private::keeper boost_private_assert_keeper = 0) ; else \
    if ( boost_private_assert_keeper.test_expr((expr),#expr) ) ; else \
        return (void)boost::smart_assert::make_assert().SMART_ASSERT_CONTEXT.print_prewrittern_values( boost_private_assert_keeper).SMART_ASSERT_A \
    /**/

#endif


} // namespace smart_assert

} // namespace boost

#endif
