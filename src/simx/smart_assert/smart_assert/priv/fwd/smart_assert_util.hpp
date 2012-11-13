// fwd/smart_assert_util.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_UTIL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_UTIL_HPP_INCLUDED

// Utility functions

#if _MSC_VER >= 1020
#pragma once
#endif


namespace boost { 

namespace smart_assert {

    // forward declaration
    class assert_context;


    namespace Private {
        string_type get_typeof_level( int nLevel);

        // ... used for dumping part of the assert_context
        struct log_context_val {
            log_context_val( client_ostream_type & out) : m_out( out) {}

            typedef std::pair< const string_type, string_type> value_type;
            void operator()( const value_type & val) {
                if ( ( val.first != "file") && 
                     ( val.first != "line") ) {
                    m_out << val.first << " = '" << val.second << "';\n";
                }
            }
        private:
            client_ostream_type & m_out;
        };

        // ... used for dumping part of the assert_context
        struct log_val {
            log_val( client_ostream_type & out) : m_out( out) {}

            typedef assert_context::val_and_str value_type;
            void operator()( const value_type & val) {
                m_out << "    " << val.m_str << " = '" << val.m_val << "';\n";
            }
        private:
            client_ostream_type & m_out;
        };


    } // namespace Private

    
    // dumps all ASSERT context
    void dump_assert_context_detail( const assert_context & context, client_ostream_type & out);

    // dumps the summary of the ASSERT context
    void dump_assert_context_summary( const assert_context & context, client_ostream_type & out);

} // namespace smart_assert

} // namespace boost


#endif
