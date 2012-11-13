// fwd/outofline.hpp

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
    We do our best to turn-off inlining
*/

#ifndef BOOST_INNER_SMART_ASSERT_OUTOFLINE_INCLUDED
#define BOOST_INNER_SMART_ASSERT_OUTOFLINE_INCLUDED


/*
    note: most templated functions, in Release mode generate
    inline code, making the footprint of SMART_ASSERT somewhat big.

    Therefore, we need to specialize for fundamental types
    (in order to get out-of-line calls).

    We specialize for:

    bool               -
    char               -
    signed char        -
    unsigned char      -
    signed short int   -
    unsigned short int -
    int                -
    unsigned int       -
    long               -
    unsigned long      -
    char*              -
    signed char*       -
    unsigned char*     -
    float              -
    double             -
    long double        -

*/


#define BOOST_SMART_ASSERT_DECL_OUTOFLINE3( function_name, type1, type2, type3, suffix) \
    extern "C" { \
        void function_name##_##suffix( type1, type2, type3); \
    } \
 \
    inline void function_name( type1 val1, type2 val2, type3 val3) { \
        function_name##_##suffix( val1, val2, val3); \
    }

#define BOOST_SMART_ASSERT_DEF_OUTOFLINE3( function_name, type1, type2, type3, suffix) \
    extern "C" { \
        void function_name##_##suffix( type1 val1, type2 val2, type3 val3) { \
            function_name##_impl( val1, val2, val3); \
        } \
    }



#endif
