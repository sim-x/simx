// fwd/smart_assert_context.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_CONTEXT_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_CONTEXT_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

#include <string>
#include <vector>
#include <map>


namespace boost { 

namespace smart_assert {

// forward declaration
class assert_context;



/*
    contains the context in which an ASSERT appeared

 Imporant note:
    The code here NEVER throws.
    It's made like this, so that in Assert<> class, we won't
    have any try/catch blocks when calling them.
*/
class assert_context {
public:

    assert_context();
    ////////////////////////////////////////////
    // context


    typedef std::map< string_type, string_type, std::less<string_type>, Private::smart_assert_alloc< string_type> > context_vals_collection;
    const context_vals_collection & get_context_vals_collection() const {
        return m_collContextVals;
    }

    // helper
    string_type get_context_val( const char_type * str) const;

    // helpers - simpler access to FILE and LINE
    // 
    // improve access to them - thanks Pavel Vozenilek!
    string_type get_context_file() const;
    int get_context_line() const;

    // FIXME (possible improvement)
    //
    // the 'str' (the string part) should be always constant;
    // therefore I could keep it as simply 'const char_type*'.
    // This would mean that I would have to provide a predicate
    // to compare 'const char_type*', for the context_vals_collection map
    template< class type>
    void add_context_val( const char_type * str, const type & val) {
        ostringstream_type out;

        out << val;
        internal_add_context_val( str, out.str() );
    }
    
    ////////////////////////////////////////////
    // expression
    void set_expr( const char_type * str);
    const char_type* get_expr() const {
        return m_strExpr;
    }


    ////////////////////////////////////////////
    // vals

    struct val_and_str {
        val_and_str( const string_type & val, const string_type & str)
            : m_val( val), m_str( str) {}

        // ... needed for allocator_array 
        val_and_str() {
        }

        // the value, converted to string
        //
        // Example:
        // int *p = 0; SMART_ASSERT(p);
        // m_val = '00000000'
        string_type m_val;
        // the string corresponding to the value
        //
        // Example:
        // int *p = 0; SMART_ASSERT(p);
        // m_str = 'p'
        string_type m_str;
    };
    typedef std::vector< val_and_str, Private::smart_assert_alloc< val_and_str> > vals_array;

    // adds a value, and returns the size of existing values array
    int add_val( const string_type & val, const string_type & str);
    const vals_array & get_vals_array() const {
        return m_aVals;
    }

    
    ////////////////////////////////////////////
    // level + extra param (level_msg)

    void set_level( int nLevel);
    int get_level() const;


    // 2004-07-15 10:20:57 kbisset
    void set_module( int nModule);
    int get_module() const;

    void set_level_msg( const char_type * strMsg);

    const string_type & get_level_msg() const {
        return m_strLevelMsg;
    }

private:
    void internal_add_context_val( const char_type * str, const string_type & val);

private:
    // the context surrounding the SMART_ASSERT
    // (the file & line it occured on, etc.;
    //  see the SMART_ASSERT_CONTEXT macro)
    context_vals_collection m_collContextVals;

    // the expression that failed (the failed assertion)
    const char_type * m_strExpr;

    // the values from the expression
    // (pairs: m_str (the name of the variable)
    //         m_val (the value of the variable)
    vals_array m_aVals;

    // the level of the assertion (warn, debug,.etc)
    int m_nLevel;

    // 2004-07-15 10:19:44 kbisset
    // the module of the assertion
    int m_nModule;

    // the message corresponding to this level (optional);
    // this can be a user-friendly message
    string_type m_strLevelMsg;
};






} // namespace smart_assert

} // namespace boost



#endif
