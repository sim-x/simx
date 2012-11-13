// impl/smart_assert_keeper.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_KEEPER_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_KEEPER_IMPL_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

// Private details !!!

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_keeper.hpp>

namespace boost { 

namespace smart_assert {

namespace Private {

keeper::keeper( int) 
    : m_strExpr( 0) {
}



BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const bool               , const char*, bool              )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const char               , const char*, char              )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const signed char        , const char*, signed_char       )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const unsigned char      , const char*, unsigned_char     )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const signed short int   , const char*, signed_short_int  )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const unsigned short int , const char*, unsigned_short_int)
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const int                , const char*, int               )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const unsigned int       , const char*, unsigned_int      )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const long               , const char*, long              )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const unsigned long      , const char*, unsigned_long     )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const char*              , const char*, char_ptr          )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const signed char*       , const char*, signed_char_ptr   )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const unsigned char*     , const char*, unsigned_char_ptr )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const float              , const char*, float             )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const double             , const char*, double            )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const long double        , const char*, long_double       )



} // namespace Private

} // namespace smart_assert

} // namespace boost



#endif
