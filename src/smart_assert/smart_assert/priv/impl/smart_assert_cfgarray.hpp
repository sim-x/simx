// impl/smart_assert_cfgarray.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_CONFIGS_ARRAY_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_CONFIGS_ARRAY_IMPL_HPP_INCLUDED

// FIXME
/*
    Make it so that if the user includes <smart_assert_ext.hpp>
    from two source files, to issue a very user-friendly error!
*/


#if _MSC_VER >= 1020
#pragma once
#endif


// Configurations - allows easier dealing with settings
// related to running the SMART_ASSERT library

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_l_ext.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_h_ext.hpp>

namespace boost {

namespace smart_assert {

smart_assert_configs_array & configs_array() {
    static smart_assert_configs_array inst;
    return inst;
}


void default_initialize( 
        const char_type * strLogFile, 
        const char_type * strPersistFile, 
        const char_type * strDefaultConfig) {

    // ... logger to file
    logger_to_file log( strLogFile);
    // ... EXTended handler
    ask_user_on_assert_ext dbg_handler;

    /*
        "ignore"
        
        Logger:   logs all failed assertions to strLogFile.
        Handlers: warn  : ignores this type of assertion (does nothing)
                  debug : ignores this type of assertion (does nothing)
                  error : default
                  fatal : default
    */
    config ignore;
    ignore.logger() = log;
    ignore.handlers().set_handler( lvl_warn, &ignore_assert);
    ignore.handlers().set_handler( lvl_debug, &ignore_assert);
    configs_array().set_config( "ignore", ignore);


    /*
        "ignore_ext"
        
        Logger:   logs all failed assertions to strLogFile.
        Handlers: warn  : ignores this type of assertion (does nothing)
                  debug : ignores this type of assertion (does nothing)
                  error : throws a smart_assert_error (default)
                  fatal : throws a smart_assert_error

                  This way, no assertion will call abort
    */
    config ignore_ext;
    ignore_ext.logger() = log;
    ignore_ext.handlers().set_handler( lvl_warn, &ignore_assert);
    ignore_ext.handlers().set_handler( lvl_debug, &ignore_assert);
    ignore_ext.handlers().set_handler( lvl_fatal, &default_handle_error);
    configs_array().set_config( "ignore_ext", ignore_ext);


    /*
        "dbg"

        Logger:   logs all failed assertions to strLogFile.
        Handlers: warn  : default
                  debug : EXTENDED handler
                          (asks user, and user has multiple Ignore options:
                           Ignore/Ignore Forever/Ignore All)
                  error : default
                  fatal : default
    */
    config dbg;
    dbg.logger() = log;
    dbg.handlers().set_handler( lvl_debug, dbg_handler );
    configs_array().set_config( "dbg", dbg);


    /*
        "dbg_win"

        Logger:   logs all failed assertions to strLogFile.
        Handlers: warn  : default
                  debug : EXTENDED handler
                          (asks user, and user has multiple Ignore options:
                           Ignore/Ignore Forever/Ignore All)
                          Unlike the "dbg" setting, this shows 
                          a graphical (user-friendly) dialog.
                  error : default
                  fatal : default    
    */
    // only for Win32
#ifdef BOOST_SMART_ASSERT_WIN32
    config dbg_win;
    dbg_win.logger() = log;
    dbg_win.handlers().set_handler( lvl_debug, &ask_user_on_assert_ext_win32 );
    configs_array().set_config( "dbg_win", dbg_win);
#endif

    /*
        "dbg_persist"
        (just like "dbg" + ignored assertions are persisted)
        
        Logger:   logs all failed assertions to strLogFile.
        Handlers: warn  : default
                  debug : EXTENDED handler
                          (asks user, and user has multiple Ignore options:
                           Ignore/Ignore Forever/Ignore All)
                          Ignore Forever/Ignore All are persisted
                  error : default
                  fatal : default
    */
    config dbg_persist( dbg);
    dbg_persist.persister().set_file_name( strPersistFile);
    configs_array().set_config( "dbg_persist", dbg_persist);


    /*
        "dbg_win_persist"
        (just like "dbg_win" + ignored assertions are persisted)
        
        Logger:   logs all failed assertions to strLogFile.
        Handlers: warn  : default
                  debug : EXTENDED handler
                          Asks user showing a user-friendly dialog.
                          Ignore Forever/Ignore All are persisted
                  error : default
                  fatal : default
    */
    // only for Win32
#ifdef BOOST_SMART_ASSERT_WIN32
    config dbg_win_persist( dbg_win);
    dbg_win_persist.persister().set_file_name( strPersistFile);
    configs_array().set_config( "dbg_win_persist", dbg_win_persist);
#endif

    if ( configs_array().has_config( strDefaultConfig))
        configs_array().run_config( strDefaultConfig);
    else
        // the config we should run does not exist
        configs_array().run_config( "dbg");
}




} // namespace smart_assert

} // namespace boost


#endif
