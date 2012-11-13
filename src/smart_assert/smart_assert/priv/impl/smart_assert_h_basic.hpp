// impl/smart_assert_h_basic.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_HANDLERS_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_HANDLERS_IMPL_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

#include <stdlib.h>
#include <stdexcept>

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_context.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_util.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_defs.hpp>

// for isspace()
// thanks Pavel Vozenilek!
#include <ctype.h>



//
// default (BASIC) handlers
//
// note: these are basic handlers;
//
// for cool features,
// #include <simx/smart_assert/smart_assert_ext.hpp>
//
// this should be #included only once, and then
// set the handler (somewhere at the beginning of main() )

namespace boost { 

namespace smart_assert {

// what instruction will break into debugger?
#ifndef BOOST_BREAK_INTO_DEBUGGER_INSTRUCTION
// try to find it ourselves
// thanks Pavel Vozenilek!
    // MSVC, Intel C++, BCB, Comeau (?)
    #if (defined _MSC_VER) || (defined __BORLANDC__) || (defined __MWERKS__)
        #define BOOST_BREAK_INTO_DEBUGGER_INSTRUCTION __asm { int 3 };

    #elif defined(__GNUC__)
        // GCC
        #define BOOST_BREAK_INTO_DEBUGGER_INSTRUCTION __asm ("int $0x3");

    #else
        #  error "Please supply instruction to DebugBreak (like 'int 3' on Intel processors)"
    #endif

#endif



void break_into_debugger() {
    BOOST_BREAK_INTO_DEBUGGER_INSTRUCTION
}



    // 2004-07-15 10:37:09 kbisset
    // ignores the assert
    void ignore_assert( const assert_context & /*context*/) {
    }


    // 2004-07-15 10:37:17 kbisset
    // aborts on assert
    void abort_on_assert( const assert_context & /*context*/) {
        abort();
    }


    // prints ALL DETAILS of the assertion to console
    void print_details_to_console( const assert_context & context) {
        dump_assert_context_detail( context, std::cout);
    }


    // prints a summary of the assertion to console
    void print_summary_to_console( const assert_context & context) {
        dump_assert_context_summary( context, std::cout);
    }

    void throw_exc_on_assert( const assert_context & context) {
        ostringstream_type out;
        dump_assert_context_summary( context, out);

#ifndef BOOST_NO_EXCEPTIONS
        throw smart_assert_error( out.str());
#else
        // what could we do?
        abort();
#endif
    }

    inline void throw_exc_on_assert_detailed( const assert_context & context) {
        ostringstream_type out;
        dump_assert_context_detail( context, out);

#ifndef BOOST_NO_EXCEPTIONS
        throw smart_assert_error( out.str());
#else
        // what could we do?
        abort();
#endif
    }


    // basic - doesn't do very much;
    //
    // for cool features, check out smart_assert_h_ext.h !!!
    void basic_ask_user_on_assert( const assert_context & context) {
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
        string_type strChooseMsg = "\nChoose option: (M)ore Details, (I)gnore, (D)ebug, (A)bort";
        std::cout << strChooseMsg << std::endl;
        bool bContinue = true;
        while ( bContinue) { 
            bContinue = false;
            char_type ch = std::cin.get();
            switch ( ch) {
            case 'm': case 'M':
                // details
                std::cout << std::endl;
                dump_assert_context_detail( context, std::cout);
                bContinue = true;
                break;

            case 'i': case 'I':
                // ignore
                break;

            case 'd': case 'D':
                // retry
#ifdef BOOST_BREAK_INTO_DEBUGGER_DEFINED
                break_into_debugger();
#else
                // don't know how to break...
                std::cout << "Don't know how to break into debugger..." << std::endl;
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
                std::cout << strChooseMsg << std::endl;
        };
    }



    // Thanks Darren Cook!
    void ask_user_on_assert_console_detailed( const assert_context & context) {
        // show info to the user
        std::cout << std::endl;
        dump_assert_context_detail( context, std::cout);

        // read
        std::cout << "\nChoose option: I - Ignore, D - Debug, A - Abort" << std::endl;
        bool bContinue = true;
        while ( bContinue) {
            bContinue = false;
            char ch = std::cin.get();
            switch ( ch) {
            case 'i': case 'I':
            // ignore
            break;

            case 'd': case 'D':
            // retry
    #ifdef BOOST_BREAK_INTO_DEBUGGER_DEFINED
            break_into_debugger();
    #else
            // don't know how to break...
            std::cout << "Don't know how to break into debugger..." << std::endl;
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
            }

            if ( !isspace( ch) && bContinue)
                // ignore spaces
                std::cout << "\nChoose option: I - Ignore, D - Debug, A - Abort" << std::endl;
        }
    };




} // namespace smart_assert

} // namespace boost


#endif
