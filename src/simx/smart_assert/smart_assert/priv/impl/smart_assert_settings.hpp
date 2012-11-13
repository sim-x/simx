// impl/smart_assert_settings.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_SETTINGS_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_SETTINGS_IMPL_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

#include <iostream>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_context.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_h_basic.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_l_basic.hpp>


namespace boost {

namespace smart_assert {

// forward declaration(s)
class smart_assert_settings;
smart_assert_settings & assert_settings();



smart_assert_settings::smart_assert_settings() {
    set_level_handler( lvl_debug, &default_handle_debug);
    set_level_handler( lvl_warn, &default_handle_warn);
    set_level_handler( lvl_error, &default_handle_error);
    set_level_handler( lvl_enforce, &default_handle_enforce);
    set_level_handler( lvl_fatal, &default_handle_fatal);

    m_dumper = &std::cout;

    m_logger = &default_logger;
}


void smart_assert_settings::set_level_handler( int nLevel, level_handler_func f) {
    Private::lock_type locker( m_cs_levels);
    m_level_handlers[ nLevel] = f;
}


bool smart_assert_settings::has_level_handler( int nLevel) const {
    Private::lock_type locker( m_cs_levels);
    level_handlers_collection::const_iterator found = m_level_handlers.find( nLevel);
    return found != m_level_handlers.end();
}


void smart_assert_settings::handle( const assert_context & context) {
    int nLevel = context.get_level();
    Private::lock_type locker( m_cs_levels);

    const level_handler_func & handler = internal_get_level_handler( nLevel);
    // FIXME MUST - this should not be thread-safe, see docs!!!!

    handler( context);
}


smart_assert_settings::level_handler_func smart_assert_settings::get_level_handler( int nLevel) {

    level_handlers_collection::const_iterator found = m_level_handlers.find( nLevel);
    if ( found != m_level_handlers.end() )
        return found->second;
    else {
        get_dumper() << "[SMART_ASSERT logical error] "
            "handler not set for level " << nLevel 
            << ", setting it to same as debug's" << std::endl;
        found = m_level_handlers.find( lvl_debug);
        m_level_handlers[ nLevel] = found->second;
        return found->second;
    }
}


const smart_assert_settings::level_handler_func & smart_assert_settings::internal_get_level_handler( int nLevel) {

    level_handlers_collection::const_iterator found = m_level_handlers.find( nLevel);
    if ( found != m_level_handlers.end() )
        return found->second;
    else {
        get_dumper() << "[SMART_ASSERT logical error] "
            "handler not set for level " << nLevel 
            << ", setting it to same as debug's" << std::endl;
        found = m_level_handlers.find( lvl_debug);
        m_level_handlers[ nLevel] = found->second;
        return found->second;
    }
}


void smart_assert_settings::set_logger( logger_func l) {
    Private::lock_type locker( m_cs_logger);
    m_logger = l;
}


void smart_assert_settings::log( const assert_context & context) {
    Private::lock_type locker( m_cs_logger);
    // FIXME (MUST) - if logging fails, make sure to output
    // to cout or something 
    //
    // this is very important!!!
    m_logger( context);
}


smart_assert_settings::logger_func smart_assert_settings::get_logger() const {
    Private::lock_type locker( m_cs_logger);
    return m_logger;
}


void smart_assert_settings::set_dumper( client_ostream_type & out) {
    m_dumper = &out;
}


client_ostream_type & smart_assert_settings::get_dumper() const {
    return *m_dumper;
}








// allows manipulating the assert settings
smart_assert_settings &assert_settings() { 
    static smart_assert_settings inst;
    return inst;
}


} // namespace smart_assert

} // namespace boost


#endif

