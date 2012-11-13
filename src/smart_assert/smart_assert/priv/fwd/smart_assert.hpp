// fwd/smart_assert.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_HPP_INCLUDED


#if _MSC_VER >= 1020
#pragma once
#endif



#include <stdlib.h>
#include <iostream>
#include <exception>

#include <simx/smart_assert/smart_assert/priv/fwd/workaround_defs.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_defs.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_print.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_context.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_keeper.hpp>

#include <boost/config.hpp>


#ifdef BOOST_SMART_ASSERT_MODE
// the user has defined the mode for SMART_ASSERTs:
//
// 1 - use SMART_ASSERTs
//
// 0 - ignore SMART_ASSERTs 
// (like regular 'assert's, in release mode)
    #if BOOST_SMART_ASSERT_MODE != 0
        // (custom) debug
        #undef BOOST_SMART_ASSERT_DEBUG
        #define BOOST_SMART_ASSERT_DEBUG
    #else
        // (custom) release
        #undef BOOST_SMART_ASSERT_DEBUG
    #endif

#else

// default
    #ifndef NDEBUG
        // debug
        #undef BOOST_SMART_ASSERT_DEBUG
        #define BOOST_SMART_ASSERT_DEBUG
    #else
        // release
        #undef BOOST_SMART_ASSERT_DEBUG
    #endif
#endif





namespace boost {

namespace smart_assert {


// in case the user does not need to customize the Assert...
class no_custom_assert_class {};

// forward declaration(s)
namespace Private {
    class keeper;

} // namespace Private







struct Assert;



///////////////////////////////////////////////////////
// optimize for size!!!
// (so that the footprint of SMART_ASSERT is as small as possible!)

template< class type>
void BOOST_SMART_ASSERT_context_impl( assert_context & context, const char_type * msg, const type & val) {
    context.add_context_val( msg, val);
}

template< class type>
void BOOST_SMART_ASSERT_context( assert_context & context, const char_type * msg, const type & val) {
    BOOST_SMART_ASSERT_context_impl( context, msg, val);
}

BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const bool               , bool              )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const char               , char              )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const signed char        , signed_char       )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const unsigned char      , unsigned_char     )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const signed short int   , signed_short_int  )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const unsigned short int , unsigned_short_int)
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const int                , int               )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const unsigned int       , unsigned_int      )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const long               , long              )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const unsigned long      , unsigned_long     )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const char*              , char_ptr          )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const signed char*       , signed_char_ptr   )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const unsigned char*     , unsigned_char_ptr )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const float              , float             )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const double             , double            )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const long double        , long_double       )



// prints the current value
template< class T>
void BOOST_SMART_ASSERT_print_current_val_impl( Assert & assert, const T & val, const char_type * str);

template< class T>
void BOOST_SMART_ASSERT_print_current_val( Assert & assert, const T & val, const char_type * str);

BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const bool               , const char_type*, bool              )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const char               , const char_type*, char              )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const signed char        , const char_type*, signed_char       )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const unsigned char      , const char_type*, unsigned_char     )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const signed short int   , const char_type*, signed_short_int  )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const unsigned short int , const char_type*, unsigned_short_int)
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const int                , const char_type*, int               )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const unsigned int       , const char_type*, unsigned_int      )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const long               , const char_type*, long              )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const unsigned long      , const char_type*, unsigned_long     )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const char*              , const char_type*, char_ptr          )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const signed char*       , const char_type*, signed_char_ptr   )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const unsigned char*     , const char_type*, unsigned_char_ptr )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const float              , const char_type*, float             )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const double             , const char_type*, double            )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const long double        , const char_type*, long_double       )


// ENDOF optimize for size!!!
///////////////////////////////////////////////////////



// assert base class
typedef assert_base_class<0>::type assert_base_type;
// printer class
typedef printer_class<0>::type printer_type;


/*
    The Assert class.
    You can customize its base class/ its printer class.
*/
struct Assert : private assert_base_type {

    Assert();
    Assert( const Assert & other);

    ~Assert();


    // adds something to the current context
    template< class type>
    Assert & context( const char_type * msg, const type & val) {
        BOOST_SMART_ASSERT_context( m_context, msg, val);
        return *this;
    }

    // sets the level message
    Assert & msg( const char_type * strMsg) {
        m_context.set_level_msg( strMsg);
        return *this;
    }


    // sets the level of the assertion
    //
    // strMsg - extra information (optional)
    //
    // note: replaced all "" with 0 - thanks Pavel!
    Assert & level( int nLevel, const char_type * strMsg = 0) {
        m_context.set_level( nLevel);
        m_context.set_level_msg( strMsg);
        return *this;
    }

    // 2004-07-15 10:23:05 kbisset
    Assert & module( int nModule, const char_type * strMsg = 0) {
        m_context.set_module( nModule);
        m_context.set_level_msg( strMsg);
        return *this;
    }

    // helper, similar to 'level( lvl_debug, msg)'
    Assert & debug( const char_type * strMsg = 0) {
        return level( lvl_debug, strMsg);
    }

    // helper, similar to 'level( lvl_warn, strMsg)'
    Assert & warn( const char_type * strMsg = 0) {
        return level( lvl_warn, strMsg);
    }

    // helper, similar to 'level( lvl_err, strMsg)'
    Assert & error( const char_type * strMsg = 0) {
        return level( lvl_error, strMsg);
    }

    // helper, similar to 'level( lvl_fatal, strMsg)'
    Assert & fatal( const char_type * strMsg = 0) {
        return level( lvl_fatal, strMsg);
    }


    // sets the expression
    Assert & set_expr( const char_type * strExpr) {
        m_context.set_expr( strExpr);
        return *this;
    }

    // prints the current value
    template< class T>
    Assert & print_current_val( const T & val, const char_type * str) {
        BOOST_SMART_ASSERT_print_current_val( *this, val, str);
        return *this;
    }

    
    // tests the expression
    //
    // in case the tested value is non-bool, it's very likely
    // that the expression is a simple-assert, like:
    // SMART_ASSERT(p); // for asserting a pointer
    //
    // it's simpler and more straightforward to say: 
    // 'SMART_ASSERT(p)' than
    // 'SMART_ASSERT(p)(p)'

    // FIXME - could this be improved (compile-time speed)?
    template< class type>
    Assert & test_expr( type val, const char * expr) {
        BOOST_SMART_ASSERT_print_current_val( *this, val, expr);
        return set_expr( expr);
    }

    Assert & test_expr( bool, const char * expr) {
        return set_expr( expr);
    }

    // logs the expression and 
    // the values that were pre-written using the v_ macro
    Assert & print_prewrittern_values( const Private::keeper & val);

    // use this to communicate whether this assertion
    // should be TOTALLY IGNORED
    // (therefore, we ignore everything in the destructor)
    //
    // this should be called, in case an exception is thrown, for instance
    Assert & set_ignore_this_assert( bool bIgnore = true) { 
        m_bIgnoreThisAssert = bIgnore; 
        return *this;
    }

    // what to return on failure
    //
    // IMPORTANT - this should be called ONLY for SMART_VERIFY_RET
    template< class type>
    type ret_on_fail( const type & val) { 
        return val; 
    }

    /*
        NEVER use them directly !!!
        Used in 
        BOOST_SMART_ASSERT_print_current_val and
        BOOST_SMART_ASSERT_context
    */
    printer_type & get_printer() { return m_printer; }
    assert_context & get_context() { return m_context; }

private:
    void handle_assert( const assert_context & context);

public:
    // helpers, in order to be able to compile the code
    Assert & SMART_ASSERT_A;
    Assert & SMART_ASSERT_B;

private:
    // The level of the assertion (warn, debug, etc.)
    int m_nLevel;

    // used to customize how each variable involved in the assert 
    // is printed
    printer_type m_printer;

    // the context of this assertion
    assert_context m_context;

    // if set to true, we ignore this assert 
    // (therefore, we ignore everything in the destructor)
    //
    // this should be set to true, in case an exception is thrown, for instance
    mutable bool m_bIgnoreThisAssert;

}; // class Assert

template< class T>
void BOOST_SMART_ASSERT_print_current_val_impl( Assert & assert, const T & val, const char_type * str)
{
#ifndef BOOST_NO_EXCEPTIONS
    try {
#endif
        assert.get_printer().print_val( 
            assert.get_context(), val, str);
#ifndef BOOST_NO_EXCEPTIONS
    }
    catch (...) {
    }
#endif
}

template< class T>
void BOOST_SMART_ASSERT_print_current_val( Assert & assert, const T & val, const char_type * str) {
    BOOST_SMART_ASSERT_print_current_val_impl( assert, val, str);
}


// creates an assertion
extern "C" {
Assert make_assert();
}


#ifdef BOOST_SMART_ASSERT_DEBUG
// behave like debug
#define SMART_ASSERT(expr) \
    if ( (expr)) ; else \
        boost::smart_assert::make_assert().test_expr( (expr), #expr).SMART_ASSERT_CONTEXT.SMART_ASSERT_A \
    /**/

// FIXME (docs) - explain about Keeper k= 0;
// ... for use with 'v_' macro
#define SMART_ASSERT_V_(expr) \
    if ( boost::smart_assert::Private::keeper boost_private_assert_keeper = 0) ; else \
    if ( boost_private_assert_keeper.test_expr((expr),#expr) ) ; else \
        boost::smart_assert::make_assert().SMART_ASSERT_CONTEXT.print_prewrittern_values( boost_private_assert_keeper).SMART_ASSERT_A \
    /**/

#else
// behave like release (ignore all SMART_ASSERTs)
//
#define SMART_ASSERT(expr) \
    if ( true ) ; else \
        boost::smart_assert::make_assert().SMART_ASSERT_A \
    /**/

#define SMART_ASSERT_V_(expr) \
    if ( true ) ; else \
        boost::smart_assert::make_assert().SMART_ASSERT_A \
    /**/

#endif

#include <simx/smart_assert/smart_assert/priv/fwd/macros.hpp>

} // namespace smart_assert

} // namespace boost


#endif


