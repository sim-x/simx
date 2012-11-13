// smart_assert_fwd.hpp

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

#ifndef BOOST_SMART_ASSERT_FWD_HPP_INCLUDED
#define BOOST_SMART_ASSERT_FWD_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

/*
    forward declarations/definitions (that can be overridden)
*/

namespace boost { namespace smart_assert {

    // forward declarations
    class default_printer;
    class no_custom_assert_class;

    /*
        IMPORTANT:

        in case you want to provide your own class,
        just specialize printer_class/ assert_base for n = 0.
    */
    template< int n = 0>
    struct printer_class {
        typedef default_printer type;
    };

    template< int n = 0>
    struct assert_base_class {
        typedef no_custom_assert_class type;
    };
}}


#endif
