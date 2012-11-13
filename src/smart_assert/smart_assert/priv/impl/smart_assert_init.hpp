// impl/smart_assert_init.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_INIT_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_INIT_IMPL_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_init.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_cfgarray.hpp>

namespace boost { 

namespace smart_assert {

// placed in a different header;
//
// if it were on the original "smart_assert_init.hpp" header,
// that would have to #include "smart_assert_cfgarray.hpp"
//
// this would have been very costly, knowing that "smart_assert_init.hpp"
// would be #included each time you use ASSERTs
inline smart_assert_initializer::smart_assert_initializer( 
        const char_type * strLogFile, 
        const char_type * strPersistFile, 
        const char_type * strDefaultConfig)
        : m_strLogFile( strLogFile),
          m_strPersistFile( strPersistFile),
          m_strDefaultConfig( strDefaultConfig) {
    // at this time, we know where that is ;-)
    m_initializer = &default_initialize;
}


} // namespace smart_assert

} // namespace boost


#endif
