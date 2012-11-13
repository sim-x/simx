// fwd/smart_assert_h_win32.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_HANDLERS_WIN32_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_HANDLERS_WIN32_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

//
// classes for handling ASSERTs for Win32 - UI
//
// note: #include <simx/smart_assert/smart_assert_ext.hpp> instead !!!


#ifdef BOOST_SMART_ASSERT_WIN32

namespace boost { 

namespace smart_assert {

namespace Private {

    typedef enum show_dlg_result { 
        // show_assert_dialog() failed
        show_failed = -1,
        
        do_nothing = 0,
        
        ignore_now,
        ignore_forever,
        ignore_all,
        do_debug,
        do_abort,
        do_show_more,

        // ...dialog is already shown
        already_shown,
    } show_dlg_result;


    show_dlg_result show_assert_dialog(
        int nLevel, const char_type * title, const char_type* text, const char_type * text_detailed);

} // namespace Private

} // namespace smart_assert

} // namespace boost


#endif // BOOST_SMART_ASSERT_WIN32



#endif
