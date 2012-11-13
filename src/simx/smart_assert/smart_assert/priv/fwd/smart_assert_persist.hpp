// fwd/smart_assert_persist.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_PERSIST_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_PERSIST_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_ts.hpp>
#include <iostream>
#include <fstream>
#include <set>

namespace boost { 

namespace smart_assert {


// forward declaration(s)
class assert_persistence;
inline assert_persistence & persistence();


namespace Private {

    // the context that is persisted
    struct persistence_context {
        // thanks Pavel Vozenilek!
        persistence_context( const char_type * strFile, int nLine) 
            : m_strFile( strFile), m_nLine( nLine) {
        }
        persistence_context() 
            : m_nLine( 0) {
        }
        persistence_context( const string_type & strFile, int nLine) 
            : m_strFile( strFile), m_nLine( nLine) {
        }

        // mark this context to mean: IGNORE ALL FOREVER
        void set_all_forever() {
            m_strFile = "***all_forever***";
        }

        bool is_all_forever() const {
            return m_strFile == "***all_forever***";
        }

        string_type m_strFile;
        int m_nLine;
    };

    inline bool operator< ( const persistence_context& first, const persistence_context & second) {
        int n = first.m_strFile.compare( second.m_strFile);
        if ( n != 0)
            return n < 0 ? true : false;
        else
            // same file
            return first.m_nLine < second.m_nLine;
    }

    inline client_ostream_type& operator<< ( client_ostream_type & out, const persistence_context & val) {
        out << '\"' << val.m_strFile << "\" " << val.m_nLine;
        return out;
    }

    inline client_istream_type & operator>>( client_istream_type & in, persistence_context & val) {
        char_type ch = in.get();
        if ( ch != '\"') {
            in.setstate( std::ios::failbit);
            return in;
        }

        val.m_strFile.erase();
        // will be set to true in case we have successfully read
        // the whole file (the file name is surrounded in quotes)
        bool bOk = false;
        while ( in.good() ) {
            char_type ch = in.get();
            if ( ch != '\"')
                val.m_strFile += ch;
            else {
                bOk = true;
                break;
            }
        }

        if ( !bOk) {
            in.setstate( std::ios::failbit);
            return in;
        }

        in >> val.m_nLine;
        return in;
    }

} // namespace Private

/*
    allows (ignored) ASSERTs to be persistent throughout
    successive runs of the same program
*/
class assert_persistence {

    friend inline assert_persistence & persistence();

    assert_persistence() : m_in_out( 0), m_bIgnoreAll( false) {
    }
public:

    // this must be valid until clear_persistence_stream is called!
    void set_persistence_stream( client_iostream_type & in_out) {
        Private::lock_type locker( m_cs);

        m_in_out = &in_out;
        load_persistent_contexts();
    }

    void clear_persistence_stream() {
        Private::lock_type locker( m_cs);

        m_in_out = 0;
        load_persistent_contexts();
    }

    // returns true if the assert can be ignored by default
    bool can_assert_be_ignored_by_default( const assert_context & context) {
        Private::lock_type locker( m_cs);
        if ( m_bIgnoreAll)
            // all assertions are ignored
            return true;
        
        Private::persistence_context ctx = from_assert_context( context);
        return is_ignored( ctx); 
    }

    // ignores an ASSERTion
    void ignore_assert( const assert_context & context, ignore_type eIgnore) {
        Private::lock_type locker( m_cs);

        Private::persistence_context ctx = from_assert_context( context);
        switch ( eIgnore) {
        case ignore_all_related:
        case ignore_all_related_persistent:
            // note: not implemented yet
            break;

        case ignore_now:
            // ignore it only now
            break;

        case ignore_forever: 
            ignore( ctx);
            break;

        case ignore_forever_persistent:
            persist( ctx);
            break;

        case ignore_all:
            m_bIgnoreAll = true;
            break;


        case ignore_all_persistent:
            ctx.set_all_forever();
            persist( ctx);
            m_bIgnoreAll = true;
            break;
        };
    }

private:
    static Private::persistence_context from_assert_context( const assert_context & context) {
        string_type strFile = context.get_context_file();
        int nLine = context.get_context_line();
        return Private::persistence_context( strFile, nLine);
    }

    bool is_ignored( const Private::persistence_context & ctx) const {
        bool bIs = m_ignored.find( ctx) != m_ignored.end();
        return bIs;
    }

    bool ignore( const Private::persistence_context & ctx) {
        bool bInserted = m_ignored.insert( ctx).second;
        return bInserted;
    }

    void persist( const Private::persistence_context & ctx) {

        bool bInserted = ignore( ctx);
        if ( bInserted) {
            if ( m_in_out)
                *m_in_out << ctx << std::endl;
        }
        else
            // the context already exists
            ;
    }

private:
    // loads the persisted contexts from the stream
    void load_persistent_contexts() {
        m_bIgnoreAll = false;
        m_ignored.clear();
        if ( !m_in_out)
            return;
        client_iostream_type & in_out = *m_in_out;
        in_out.seekg( 0, std::ios::beg);
        string_type strLine;
        while ( std::getline( in_out, strLine) ) {
            istringstream_type in( strLine);
            Private::persistence_context ctx;
            in >> ctx;
            if ( in.fail() )
                // invalid context
                ;
            else {
                m_ignored.insert( ctx);
                if ( ctx.is_all_forever() )
                    // we encountered IGNORE ALL
                    m_bIgnoreAll = true;
            }
        }
        // go to the end of the stream; we'll be appending 
        // new contexts to be persisted...
        in_out.seekp( 0, std::ios::end);
        in_out.clear();
    }


private:
    // the stream for doing persistence
    client_iostream_type * m_in_out;

    // the contexts that have already been ignored...
    typedef Private::persistence_context persistence_context;
    std::set< persistence_context, std::less< persistence_context>, Private::smart_assert_alloc< persistence_context> > m_ignored;

    // thread-safe access
    mutable Private::mutex_type m_cs;

    // true if everything should be ignored
    bool m_bIgnoreAll;
};


// the one-and-only instance
inline assert_persistence & persistence() {
    static assert_persistence inst;
    return inst;
}


} // namespace smart_assert

} // namespace boost



#endif
