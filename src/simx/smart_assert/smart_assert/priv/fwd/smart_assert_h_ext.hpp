// fwd/smart_assert_h_ext.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_HANDLERS_EXT_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_HANDLERS_EXT_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

//
// cool classes for handling ASSERTs
//
// note: #include <simx/smart_assert/smart_assert_ext.hpp> instead !!!


#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_defs.hpp>


#ifndef BOOST_SMART_ASSERT_EXT_HPP_INCLUDED
#error "Don't include directly; #include smart_assert_ext.h instead!"
#endif


namespace boost { 

namespace smart_assert {
    
    // forward declaration
    class assert_context;


    ////////////////////////////////////////////////////////////
    // ask_user_on_assert_internal_testing


    // this is just to see how the IGNORE mechanism works
    //
    // it shows to the user all possibilities for ignoring an ASSERT;
    // 
    // this is most likely too complicated for a user (even a programmer!),
    // since there are too many IGNORE options
    //
    // as persistence stream,it uses the "./persist.txt" file.
    //
    // IMPORTANT: Never use this in real-life code; as said,
    // it's just for you to get a feel of how persistence/ Ignoring really works!
    void ask_user_on_assert_internal_testing( const assert_context & context);


    ////////////////////////////////////////////////////////////
    // ask_user_on_assert_ext 

    // should we show details?
    typedef enum show_details_type {
        show_details_by_default,
        donot_show_details_by_default
    //  BC++ needs this
    //thanks Pavel Vozenilek!
    } show_details_type;

    /* 
        Asks the user, when an assertion fails. 
        (EXTended version)

      Remarks

        Ignore Forever/Ignore All are persistent.
        If you don't want persistence, make sure you call
        persistence().clear_persistence_stream();
    */
    struct ask_user_on_assert_ext {

        ask_user_on_assert_ext( 
            show_details_type eType = show_details_by_default, 
            client_ostream_type & out = std::cout ,
            client_istream_type & in = std::cin
            ) 
            : m_bShowDetailsByDefault( eType == show_details_by_default),
              m_out( out),
              m_in( in) {
        }

        void operator()( const assert_context & context);
    private:        
        // are we showing the whole ASSERT context details,
        // or just a summary?
        bool m_bShowDetailsByDefault;

        // for asking the user - where should we write to,
        // where should we read from
        client_ostream_type & m_out;
        client_istream_type & m_in;
    }; // ask_user_on_assert_console_ext


#ifdef BOOST_SMART_ASSERT_WIN32
    ////////////////////////////////////////////////////////////
    // ask_user_on_assert_ext_win32

    /* 
        Asks the user, when an assertion fails. 

        Win32 specific (shows a dialog to the user)
    */
    void ask_user_on_assert_ext_win32( const assert_context & context);

#endif




} // namespace smart_assert

} // namespace boost




#endif
