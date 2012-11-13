// fwd/smart_assert_cfgarray.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_CONFIGS_ARRAY_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_CONFIGS_ARRAY_HPP_INCLUDED


#if _MSC_VER >= 1020
#pragma once
#endif

#include <map>
#include <boost/function.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_alloc.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_ts.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_ctxfunc.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_persist.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_h_basic.hpp>


// Configurations - allows easier dealing with settings
// related to running the SMART_ASSERT library

namespace boost {

namespace smart_assert {

// forward declaration(s)
class smart_assert_configs_array;
inline smart_assert_configs_array & configs_array();


/*
    handlers for a given configuration

    Used internally in 'config'
*/
struct config_handlers {

    config_handlers(
            assert_context_func warn_handler  = &default_handle_warn,
            assert_context_func debug_handler = &default_handle_debug,
            assert_context_func error_handler = &default_handle_error,
            assert_context_func enforce_handler = &default_handle_enforce,
            assert_context_func fatal_handler = &default_handle_fatal) {
        set_handler( lvl_warn, warn_handler);
        set_handler( lvl_debug, debug_handler);
        set_handler( lvl_error, error_handler);
        set_handler( lvl_enforce, enforce_handler);
        set_handler( lvl_fatal, fatal_handler);
    }

    // sets the handler for a given leven
    void set_handler( int nLevel, assert_context_func handler) {
        m_handlers[ nLevel] = handler;
    }

    // sets the handlers from this configuration
    // as the current handlers
    void run_handlers() const {
        handlers_collection::const_iterator first = m_handlers.begin(), last = m_handlers.end();
        while ( first != last) {
            int nLevel = first->first;
            assert_context_func handler = first->second;

            assert_settings().set_level_handler( nLevel, handler);
            ++first;
        }
    }
    
private:
    typedef std::map< int, assert_context_func, std::less< int>, Private::smart_assert_alloc< assert_context_func> > handlers_collection;
    handlers_collection m_handlers;
};



/*
    allows managing the persistence of a configuration

    Used internally in 'config'
*/
struct config_persister {

    // no persistence
    config_persister() : m_in_out( 0) {
    }

    // persist to file
    config_persister( const char_type * strFileName)
        : m_in_out( 0),
          m_strFileName( strFileName) {
    }

    // persist to stream
    config_persister( client_iostream_type & in_out)
        : m_in_out( &in_out) {
    }

    ~config_persister() {
        reset();
    }


    // sets the file name we persist to
    void set_file_name( const char * strFileName) {
        reset();
        m_strFileName = strFileName;
        m_in_out = 0;
    }

    // sets the iostream we persist to
    void set_iostream( client_iostream_type & in_out) {
        reset();
        m_in_out = &in_out;
    }

    // sets the persistence, based on the settings we set so far
    void run_persister () const {
        bool bDoSet = false;

        if ( m_in_out)
            // persist to iostream
            bDoSet = true;
        else {
            if ( !m_strFileName.empty()) {
                // persist to file
                std::fstream * persist_file = new std::fstream( m_strFileName.c_str());
                if ( persist_file->fail()) {
                    persist_file->close();
                    persist_file->clear();
                    persist_file->open( m_strFileName.c_str(), 
                        std::ios::in | std::ios::out | std::ios::trunc );
                }
                m_in_out = persist_file;
                bDoSet = true;
            }
            else
                // we don't persist
                ;
        }

        if ( bDoSet)
            persistence().set_persistence_stream( *m_in_out);
        else
            persistence().clear_persistence_stream();
    }

private:
    void reset() {
        if ( !m_strFileName.empty())
            // it's a file, which we own
            delete m_in_out;

        m_in_out = 0;
        m_strFileName.erase();
    }
private:
    // where we're persisting...
    mutable client_iostream_type * m_in_out;

    // in case we're persisting to file.
    string_type m_strFileName;
}; // struct config_persister


/*
    Keeps details about a configuration

    Internally keeps:
    - the logger
    - the handlers for warn, debug, error, fatal
    - the persistence setter (the object that sets or
      clears the persistence)
*/
struct config {

    config( 
        assert_context_func logger = &default_logger,
        assert_context_func warn_handler  = &default_handle_warn,
        assert_context_func debug_handler = &default_handle_debug,
        assert_context_func error_handler = &default_handle_error,
        assert_context_func enforce_handler = &default_handle_enforce,
        assert_context_func fatal_handler = &default_handle_fatal,
        const config_persister & persister = config_persister() )
        : m_logger( logger),
          m_handlers( warn_handler, debug_handler, error_handler, enforce_handler, fatal_handler),
          m_persister( persister)
    {}

    // get/set the logger
    assert_context_func & logger() {
        return m_logger;
    }

    // manipulate handlers
    config_handlers & handlers() {
        return m_handlers;
    }

    // manipulate persister
    config_persister & persister() {
        return m_persister;
    }

    // runs this config
    void run_config() const {
        m_persister.run_persister();
        m_handlers.run_handlers();

        assert_settings().set_logger( m_logger);
    }
private:
    // logger
    assert_context_func m_logger;

    // handlers
    config_handlers m_handlers;

    // persister
    config_persister m_persister;
};



/*
    The Configurations array
*/
class smart_assert_configs_array {

    friend inline smart_assert_configs_array & configs_array();

    smart_assert_configs_array() {
    }
public:

    // associates a configuration with a certain string
    void set_config( const char_type * str, const config & cfg) {
        Private::lock_type locker( m_cs);

        m_configs[ str] = cfg;
    }

    // returns true if this config exists
    bool has_config( const char_type * str) const {

        configs_collection::const_iterator found = m_configs.find( str);
        bool bHas = ( found != m_configs.end() );
        return bHas;
    }

    // runs the configuration associated  with the given string
    void run_config( const char_type * str) const {
        Private::lock_type locker( m_cs);

        // by default, no persistence; however, the config
        // we're currently running, can override this.
        persistence().clear_persistence_stream();

        configs_collection::const_iterator found = m_configs.find( str);
        if ( found != m_configs.end() ) 
            // run the function
            (found->second).run_config();
        else
            assert_settings().get_dumper()
                << "[SMART_ASSERT logical error] "
                "configuration " << str << " not found!" << std::endl;
    }


private:
    // the configurations
    /*
    Note: VC6 generates bad code when encountering this.
        Anyway, in this case we don't have to use our own allocator.
        These are global settings

    typedef std::map< string_type, config, std::less< string_type>, Private::smart_assert_alloc< config> > configs_collection;
    */
    typedef std::map< string_type, config> configs_collection;
    configs_collection m_configs;


    // thread-safe access
    mutable Private::mutex_type m_cs;


};


// defined in the impl/ counterpart
smart_assert_configs_array & configs_array();
void default_initialize( 
        const char_type * strLogFile, 
        const char_type * strPersistFile, 
        const char_type * strDefaultConfig);


} // namespace smart_assert

} // namespace boost



#endif
