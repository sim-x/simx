// fwd/macros.hpp

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
    Note: we need this, since we need to undefine these macros,
    when on impl/smart_assert.hpp, since we have something similar to:


    Assert::Assert() 
        : m_bIgnoreThisAssert( false),
          m_nLevel( lvl_debug),
          SMART_ASSERT_A( *this),
          SMART_ASSERT_B( *this) {
        ...
    }

    If macros were in effect, there would be a compile-time error
*/


// FIXME (docs) - explain the internals of this mechanism!
// same for SMART_VERIFY, SMART_ENFORCE!
#define SMART_ASSERT_A(x) SMART_ASSERT_OP(x, B)
#define SMART_ASSERT_B(x) SMART_ASSERT_OP(x, A)

#define SMART_ASSERT_OP(x, next) \
    SMART_ASSERT_A.print_current_val((x), #x).SMART_ASSERT_ ## next \
    /**/


