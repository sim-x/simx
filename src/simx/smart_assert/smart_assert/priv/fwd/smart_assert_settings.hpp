// fwd/smart_assert_settings.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_SETTINGS_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_SETTINGS_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

#include <map>
#include <iosfwd>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_ctxfunc.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_ts.hpp>


namespace boost {

namespace smart_assert {

// forward declaration(s)
class smart_assert_settings;
smart_assert_settings & assert_settings();
class assert_context;


    // FIXME - when smart_assert_settings is destroyed,
    // EVERY assertion is dumped to some std::cerr or so (everything we would access,
// would be invalid data TOTHINK);
// eventually, try to recreate TOTHINK

/*
    allows manipulating the assert settings
*/
class smart_assert_settings {

    friend smart_assert_settings & assert_settings();

    // level handler function
    typedef assert_context_func level_handler_func;

    // logger function - for logging a failed assertion
    typedef assert_context_func logger_func;


    // use boost::smart_assert::assert_settings() to manipulate it
    smart_assert_settings();
public:

    //////////////////////////////////////////////////////
    // levels

    // sets the handler for a given ASSERTion level
    void set_level_handler( int nLevel, level_handler_func f);

    // returns true if we have a handler for a given ASSERTion level
    bool has_level_handler( int nLevel) const;


    // handles a failed ASSERTion
    void handle( const assert_context & context);

    // gets the handler for a given ASSERTion level
    //
    // this is useful when you want to "surround" an existing handler
    // (that is, do some custom behavior besides the default)
    level_handler_func get_level_handler( int nLevel);

private:
    // gets the handler for a given ASSERTion level
    //
    // IMPORTANT: we should return a const reference, instead of a value;
    // this is because internally, we might have a functor (an object);
    // if we were to return it by value, we would return a copy,
    // instead of the object itself - not what we would want!!!
    const level_handler_func & internal_get_level_handler( int nLevel);

    //////////////////////////////////////////////////////
    // logger
public:
    void set_logger( logger_func l);
    void log( const assert_context & context);
    logger_func get_logger() const;

    //////////////////////////////////////////////////////
    // dumper

    // in case we need to dump logical errors related 
    // to misuse of SMART_ASSERTs somewhere...
    //
    // note that throwing a std::logic_error would be pretty drastic,
    // for misuse of a SMART_ASSERT...
    void set_dumper( client_ostream_type & out);
    client_ostream_type & get_dumper() const;


private:

    // the handlers for different levels of ASSERTions
    typedef std::map< int, level_handler_func, std::less<int>, Private::smart_assert_alloc< level_handler_func> > level_handlers_collection;
    level_handlers_collection m_level_handlers;

    // where to dump logical errors related to using SMART_ASSERT
    // 
    // we can't assert, can we? ;-)
    //
    // note that throwing a std::logic_error would be pretty drastic,
    // for misuse of a SMART_ASSERT...
    client_ostream_type * m_dumper;

    // does the logging of failed ASSERTions
    logger_func m_logger;

    // mutex for thread-safe access to levels
    mutable Private::mutex_type m_cs_levels;

    // mutex for thread-safe access to logger
    mutable Private::mutex_type m_cs_logger;
};



// allows manipulating the assert settings
smart_assert_settings &assert_settings();


} // namespace smart_assert

} // namespace boost



#endif

