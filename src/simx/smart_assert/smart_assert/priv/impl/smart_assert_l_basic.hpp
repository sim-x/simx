// impl/smart_assert_l_basic.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_LOGGERS_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_LOGGERS_IMPL_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

#include <sstream>
#include <fstream>
#include <algorithm>
#include <utility>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_context.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_defs.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_util.hpp>

// Default classes that do the logging,
// in case an ASSERT fails

namespace boost { 

namespace smart_assert {

void default_logger( const assert_context & context) {
    ostringstream_type out;
    dump_assert_context_detail( context, out);

    std::cerr << out.str() << std::endl;
}


} // namespace smart_assert

} // namespace boost

#endif
