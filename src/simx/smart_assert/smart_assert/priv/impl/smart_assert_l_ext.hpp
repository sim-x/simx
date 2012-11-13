// impl/smart_assert_l_ext.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_LOGGERS_EXT_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_LOGGERS_EXT_IMPL_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

// COOL classes that do the logging,
// in case an ASSERT fails


namespace boost { 

namespace smart_assert {

#if defined( BOOST_SMART_ASSERT_WIN32)
    //
    // outputs using OutputDebugString
    //
    void output_to_debug_wnd_win32( const assert_context & context) {

        // follow VC convension:
        // first, write "File(Line) : ...", then on the next lines
        // write the rest of the message;
        //
        // this way, someone can actually double click on the first line,
        // and see where the assertion occured (in debug mode)

        // log context
        ostringstream_type out;
        out << '\n' 
            << context.get_context_file()
            << '(' << context.get_context_line() << ") : [SMART_ASSERT]\n"
            << Private::get_typeof_level( context.get_level() ) << " (details below)\n";

        if ( context.get_context_vals_collection().size() > 2) {
            // extra information
            std::for_each( 
                context.get_context_vals_collection().begin(), 
                context.get_context_vals_collection().end(),
                Private::log_context_val( out) );
        }

        // log expression
        if ( !context.get_level_msg().empty())
            out << "\n    User-friendly msg: '" << context.get_level_msg() << "'";
        out << "\nExpression = '" << context.get_expr() << "'\n";

        // log values
        out << "\nValues:\n";
        std::for_each( 
            context.get_vals_array().begin(), context.get_vals_array().end(),
            Private::log_val( out) );

        out << "---------------------------\n";
        ::OutputDebugStringA( out.str().c_str());
    };
#endif


} // namespace smart_assert

} // namespace boost

#endif
