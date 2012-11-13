// fwd/smart_assert_init.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_INIT_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_INIT_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_defs.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_alloc.hpp>

namespace boost { 

namespace smart_assert {

// forward decalaration(s)
struct smart_assert_initializer;
smart_assert_initializer & initializer();


/*
    Allows initialzing the SMART_ASSERT library.
    This allows setting the default configurations,
    and selecting one of them.

  FIXME
    explain why we use a function instead of a variable.
    (basically, because an ASSERT could happen before main)

    TOTHINK should we use an initializer variable also,
    which would call initializer().reinitialize() before main()?

    should we make it thread-safe ??? YES WE SHOULD
*/
struct smart_assert_initializer {
    
    typedef void (*initialize_func)( 
        const char_type * strLogFile, 
        const char_type * strPersistFile, 
        const char_type * strDefaultConfig);

public:
    // constructor
    inline smart_assert_initializer( 
        const char_type * strLogFile, 
        const char_type * strPersistFile, 
        const char_type * strDefaultConfig = "dbg");

    void initialize() {
        if ( m_bInitialized)
            return;

        // force the user to define s_before_main
        before_main * p = &s_before_main;
        if ( p == 0)
            ++p;

        m_bInitialized = true;
        m_initializer( 
            m_strLogFile.c_str(), m_strPersistFile.c_str(), m_strDefaultConfig.c_str());
    }

    void reinitialize( 
            const char_type * strLogFile, 
            const char_type * strPersistFile, 
            const char_type * strDefaultConfig = "dbg") {
        m_bInitialized = false;
        m_strLogFile = strLogFile;
        m_strPersistFile = strPersistFile;
        m_strDefaultConfig = strDefaultConfig;
        initialize();
    }

private:
    // where to log assertions, persistence file, and default config.
    string_type m_strLogFile;
    string_type m_strPersistFile;
    string_type m_strDefaultConfig;
    // have we been initialized?
    bool m_bInitialized;
    // what function to call, in order to initialize?
    initialize_func m_initializer;

    /*
        make sure the initializer is called before main!
        (even if no assertion fails ;-))
    */
    struct before_main {
        before_main() {
            initializer().initialize();
        }
    };
    static before_main s_before_main;
};


// the function used to initialize the SMART_ASSERT library
//
// note: when you define this function, MAKE SURE to 
// #include <simx/smart_assert/smart_assert_ext.hpp>
smart_assert_initializer & initializer();


} // namespace smart_assert

} // namespace boost




#endif
