// fwd/smart_assert_l_ext.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_LOGGERS_EXT_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_LOGGERS_EXT_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

// COOL classes that do the logging,
// in case an ASSERT fails

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_l_basic.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_context.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_ctxfunc.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_alloc.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_util.hpp>
#include <vector>
#include <fstream>

#if defined( BOOST_SMART_ASSERT_WIN32)
#include <windows.h>
#endif


namespace boost { 

namespace smart_assert {

    /*
        allows chaining multiple loggers.

        For instance, you might want to log failed assertions
        to both a file & to the console.
        
        Create a logger_array, and do something similar to the following:

        logger_array a;
        a.add_logger( logger_to_file);
        a.add_logger( logger_to_console);
        assert_settings().set_logger( a);

        In this case, when a failed assertion happens, this
        logger gets called, which forwards to the inner loggers.
    */
    class logger_array {
        typedef std::vector< assert_context_func, Private::smart_assert_alloc< assert_context_func> > array;

    public:
        void operator()( const assert_context & context) const {
            array::const_iterator first = m_loggers.begin(), last = m_loggers.end();
            while ( first != last) {
                (*first)( context);
                ++first;
            }
        }

        // adds a logger, and returns its index
        // (the index can be used for removal)
        int add_logger( assert_context_func f) {
            m_loggers.push_back( f);
            return m_loggers.size();
        }

        // erases a logger from the array
        void erase_logger( std::size_t idx) {
            if ( idx < m_loggers.size() ) 
                m_loggers.erase( m_loggers.begin() + idx);
            else 
                assert_settings().get_dumper() << "[SMART_ASSERT logical error] "
                    "erase_logger; index too big: " << idx << std::endl ;
        }
        
    private:
        array m_loggers;
    };


    /*
        logs failed assertions to a file
    */
    struct logger_to_file {
        logger_to_file( const char * strFileName)
            : m_strFileName( strFileName),
              m_pOut( 0)
        {}

        void operator()( const assert_context & context) {
            if ( !m_pOut ) {
                // open only when first used!
                m_pOut = new std::ofstream( m_strFileName.c_str() );
            }

            ostringstream_type out;
            dump_assert_context_detail( context, *m_pOut);
            *m_pOut << out.str() << std::endl;
        }

    private:
        // where are we logging?
        string_type m_strFileName;
        std::ofstream * m_pOut;
    };


#if defined( BOOST_SMART_ASSERT_WIN32)
    //
    // outputs using OutputDebugString
    //
    void output_to_debug_wnd_win32( const assert_context & context);
#endif
} // namespace smart_assert

} // namespace boost



#endif
