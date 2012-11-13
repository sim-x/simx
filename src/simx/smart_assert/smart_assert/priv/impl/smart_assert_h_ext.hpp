// impl/smart_assert_h_ext.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_HANDLERS_EXT_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_HANDLERS_EXT_IMPL_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_h_basic.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_h_win32.hpp>
#include <fstream>
#include <iostream>


//
// cool classes for handling ASSERTs
//
// note: #include <simx/smart_assert/smart_assert_ext.hpp> instead !!!

namespace boost { 

namespace smart_assert {

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
    void ask_user_on_assert_internal_testing( const assert_context & context) {

        static std::fstream persist_inout( "./persist.txt");
        static bool bFirstTime = true;
        if ( bFirstTime) {
            bFirstTime = false;
            if ( persist_inout.fail()) {
                persist_inout.close();
                persist_inout.clear();
                persist_inout.open( "./persist.txt", std::ios::in | std::ios::out | std::ios::trunc );
            }
            persistence().set_persistence_stream( persist_inout);
        }

        if ( persistence().can_assert_be_ignored_by_default( context))
            // ignore assertion
            return;

        // show info to the user
        std::cout << "\n" << Private::get_typeof_level( context.get_level() ) << " in ["
            << context.get_context_file()
            << ":" << context.get_context_line() << "]:\n";

        if ( !context.get_level_msg().empty())
            // we have a user-friendly message
            std::cout << context.get_level_msg() << std::endl;
        else
            std::cout << "Expression: " << context.get_expr() << std::endl;

        // read
        string_type strChooseMsg = 
            "\nChoose option: \n"
            "D - Details\n"
            "1 - Ignore now\n"
            "2 - Ignore forever\n"
            "3 - Ignore all\n"
            "4 - Ignore all related (not impl. yet)\n"
            "5 - Ignore forever PERSISTENT\n"
            "6 - Ignore all PERSISTENT\n"
            "7 - Ignore all related PERSISTENT (not impl. yet)\n"
            "E - Erase PERSISTENT stream\n"
            "R - Retry\n"
            "A - Abort\n";

        std::cout << strChooseMsg << std::endl;
        bool bContinue = true;
        while ( bContinue) { 
            bContinue = false;
            char_type ch = std::cin.get();
            switch ( ch) {
            case 'd': case 'D':
                // details
                std::cout << std::endl;
                dump_assert_context_detail( context, std::cout);
                bContinue = true;
                break;

            case '1':
                persistence().ignore_assert( context, ignore_now);
                break;
            case '2':
                persistence().ignore_assert( context, ignore_forever);
                break;
            case '3':
                persistence().ignore_assert( context, ignore_all);
                break;
            case '4':
                persistence().ignore_assert( context, ignore_all_related);
                break;
            case '5':
                persistence().ignore_assert( context, ignore_forever_persistent);
                break;
            case '6':
                persistence().ignore_assert( context, ignore_all_persistent);
                break;
            case '7':
                persistence().ignore_assert( context, ignore_all_related_persistent);
                break;

            case 'e': case 'E':
                // erase persistence file
                bContinue = true;
                persistence().clear_persistence_stream();
                persist_inout.close();
                persist_inout.clear();
                persist_inout.open( "./persist.txt", std::ios::in | std::ios::out | std::ios::trunc );
                persistence().set_persistence_stream( persist_inout);
                break;

            case 'r': case 'R':
                // retry
#ifdef BOOST_BREAK_INTO_DEBUGGER_DEFINED
                break_into_debugger();
#else
                // don't know how to break...
                std::cout << "Don't know how to break into debugger..." << std::endl;
                abort();
#endif
                break;

            case 'a': case 'A':
                // abort
                abort();
                break;

            default:
                bContinue = true;
                break;
            };

            if ( !isspace( ch) && bContinue)
                // ignore spaces
                std::cout << strChooseMsg << std::endl;
        };
    }



    ////////////////////////////////////////////////////////////
    // ask_user_on_assert_ext 

    /* 
        Asks the user, when an assertion fails. 
        (EXTended version)

      Remarks

        Ignore Forever/Ignore All are persistent.
        If you don't want persistence, make sure you call
        persistence().clear_persistence_stream();
    */
    void ask_user_on_assert_ext::operator()( const assert_context & context) {

        if ( persistence().can_assert_be_ignored_by_default( context))
            // ignore assertion
            return;

        // FIXME protect against being called at the same time recursively
        // or from two threads - TOTHINK
        //note: the logger can be entered recursively as well.

        // show info to the user
        if ( m_bShowDetailsByDefault)
            dump_assert_context_detail( context, m_out);
        else
            dump_assert_context_summary( context, m_out);

        // read
        string_type strChooseMsg = "\nChoose option: ";
        if ( !m_bShowDetailsByDefault)
            strChooseMsg += "(M)ore details, ";
        strChooseMsg += "(I)gnore, Ignore (F)orever, Ignore A(L)l, (D)ebug, (A)bort\n";

        m_out << strChooseMsg << std::endl;
        bool bContinue = true;
        while ( bContinue) { 
            bContinue = false;
            char_type ch = m_in.get();
            switch ( ch) {
            case 'm': case 'M':
                // details
                if ( !m_bShowDetailsByDefault) {
                    m_out << std::endl;
                    dump_assert_context_detail( context, m_out);
                }
                bContinue = true;
                break;

            case 'i': case 'I':
                // ignore
                persistence().ignore_assert( context, ignore_now);
                break;

            case 'f': case 'F':
                // ignore forever
                persistence().ignore_assert( context, ignore_forever_persistent);
                break;

            case 'l': case 'L':
                // ignore all
                persistence().ignore_assert( context, ignore_all_persistent);
                break;

            case 'd': case 'D':
                // Debug
#ifdef BOOST_BREAK_INTO_DEBUGGER_DEFINED
                break_into_debugger();
#else
                // don't know how to break...
                m_out << "Don't know how to break into debugger..." << std::endl;
                bContinue = true;
#endif
                break;

            case 'a': case 'A':
                // abort
                abort();
                break;

            default:
                bContinue = true;
                break;
            };

            if ( !isspace( ch) && bContinue)
                // ignore spaces
                m_out << strChooseMsg << std::endl;
        };
    }



#ifdef BOOST_SMART_ASSERT_WIN32
    ////////////////////////////////////////////////////////////
    // ask_user_on_assert_ext_win32

    /* 
        Asks the user, when an assertion fails. 

        Win32 specific (shows a dialog to the user)
    */
    void ask_user_on_assert_ext_win32( const assert_context & context) {

        if ( persistence().can_assert_be_ignored_by_default( context))
            // ignore assertion
            return;

        ostringstream_type out;
        dump_assert_context_summary( context, out);
        ostringstream_type out_detail;
        dump_assert_context_detail( context, out_detail);

        string_type title = Private::get_typeof_level( context.get_level() );
        Private::show_dlg_result nRet =
            Private::show_assert_dialog( context.get_level(),
                title.c_str(), out.str().c_str(), out_detail.str().c_str() );
        switch ( nRet) {
        default:
        case Private::show_failed:
            // FIXME (nice to have) - if cannot create dialog, show a msg box.
            break;

        case Private::ignore_now:
            persistence().ignore_assert( context, ignore_now);
            break;

        case Private::ignore_forever:
            persistence().ignore_assert( context, ignore_forever_persistent);
            break;

        case Private::ignore_all:
            persistence().ignore_assert( context, ignore_all_persistent);
            break;

        case Private::do_debug:
    #ifdef BOOST_BREAK_INTO_DEBUGGER_DEFINED
            // FIXME - what if there is not BOOST_BREAK_INTO_DEBUGGER_DEFINED?
            break_into_debugger();
    #endif
            break;

        case Private::do_abort:
            abort();
            break;

        case Private::already_shown:
            // another assertion dialog is shown at this time;
            // we're not showing two dialogs at the same time.
            // ...ignore.
            break;
        };
    }


#endif




} // namespace smart_assert

} // namespace boost


#endif
