// impl/smart_assert_util.hpp

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


#ifndef BOOST_INNER_SMART_ASSERT_UTIL_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_UTIL_IMPL_HPP_INCLUDED

// Utility functions

#if _MSC_VER >= 1020
#pragma once
#endif

#include <string>
#include <algorithm>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_context.hpp>

namespace boost { 

namespace smart_assert {

namespace Private {

    // returns a message corresponding to the type of level
    // takes into considerance only default existing levels 
    // (does not take custom levels into account, in case the user
    //  has defined any)
    string_type get_typeof_level( int nLevel) {
        switch ( nLevel) {
        case lvl_warn:
            return "Warning";

        case lvl_debug:
            return "Assertion failed";

        case lvl_error:
            return "Assertion failed (Error)";

        case lvl_enforce:
            return "ENFORCE failed";

        case lvl_fatal:
            return "Assertion failed (FATAL)";

        default: {
            ostringstream_type out;
            out << "Assertion failed (level=" << nLevel << ")";
            return out.str();
                 }
        };
    }



} // namespace Private


    // dumps all ASSERT context
    void dump_assert_context_detail( const assert_context & context, client_ostream_type & out) {

        // log context
        out << "\n" << Private::get_typeof_level( context.get_level() ) << " in ["
            << context.get_context_file()
            << ':' << context.get_context_line() << "]:\n";
        if ( context.get_context_vals_collection().size() > 2) {
            // extra information
            std::for_each( 
                context.get_context_vals_collection().begin(), 
                context.get_context_vals_collection().end(),
                Private::log_context_val( out) );
        }

        // log expression
        if ( !context.get_level_msg().empty())
            out << "\nUser-friendly msg: '" << context.get_level_msg() << "'";
        out << "\n    Expression = '" << context.get_expr() << "'\n";

        // log values
        out << "\n    Values:\n";
        std::for_each( 
            context.get_vals_array().begin(), context.get_vals_array().end(),
            Private::log_val( out) );
    }


    // dumps the summary of the ASSERT context
    void dump_assert_context_summary( const assert_context & context, client_ostream_type & out) {
        out << "\n" << Private::get_typeof_level( context.get_level() ) << " in ["
            << context.get_context_file()
            << ":" << context.get_context_line() << "]:\n";

        if ( !context.get_level_msg().empty())
            // we have a user-friendly message
            out << context.get_level_msg() << std::endl;
        else
            out << "Expression: " << context.get_expr() << std::endl;
    }



} // namespace smart_assert

} // namespace boost


#endif
