// impl/smart_assert_context.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_CONTEXT_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_CONTEXT_IMPL_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif


namespace boost { 

namespace smart_assert {


assert_context::assert_context() 
    : m_strExpr( 0),
      // 2004-07-15 10:23:42 kbisset
      m_nLevel( lvl_error),
      m_nModule(0) {
    // reserve a few vals in advance - thanks Pavel Vozenilek!
    m_aVals.reserve( 4);
}



// FIXME
/*
care about BOOST_NO_EXCEPTIONS
#ifndef BOOST_NO_EXCEPTIONS
        try {
#endif

*/


string_type assert_context::get_context_val( const char_type * str) const {
    try {
        context_vals_collection::const_iterator
            found = m_collContextVals.find( str);
        if ( found != m_collContextVals.end())
            return found->second;
        else
            return string_type();
    }
    catch (...) {
        static string_type s;
        return s;
    }
}


string_type assert_context::get_context_file() const {
    try {
        return get_context_val( "file");
    }
    catch (...) {
        static string_type s;
        return s;
    }
}


int assert_context::get_context_line() const {
    try {
        istringstream_type line( get_context_val( "line"));
        int nLine = 0;
        line >> nLine;
        return nLine;
    }
    catch (...) {
        return 0;
    }
}


void assert_context::internal_add_context_val( const char_type * str, const string_type & val) {
    try {
        m_collContextVals[ str] = val;
    }
    catch (...) {}
}


void assert_context::set_expr( const char_type * str) {
    m_strExpr = str;
}




// adds a value, and returns the size of existing values array
int assert_context::add_val( const string_type & val, const string_type & str) {
    try {
        m_aVals.push_back( val_and_str( val, str));
    }
    catch (...) {}

    return m_aVals.size();
}


////////////////////////////////////////////
// level + extra param (level_msg)

void assert_context::set_level( int nLevel) {
    m_nLevel = nLevel;
}

int assert_context::get_level() const {
    return m_nLevel;
}

// 2004-07-15 10:26:12 kbisset
void assert_context::set_module( int nModule) {
    m_nModule = nModule;
}

int assert_context::get_module() const {
    return m_nModule;
}

void assert_context::set_level_msg( const char_type * strMsg) {
    try {
        if ( strMsg != 0)
            m_strLevelMsg = strMsg;
        else
            m_strLevelMsg.erase();
    }
    catch (...) {}
}


} // namespace smart_assert

} // namespace boost

#endif
