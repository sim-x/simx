// fwd/smart_assert_keeper.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_KEEPER_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_KEEPER_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

// Private details !!!

#ifndef BOOST_INNER_SMART_ASSERT_HPP_INCLUDED
#error "Never include this directly!"
#endif

#include <vector>
#include <utility>
#include <string>
#include <boost/config.hpp>

namespace boost { 

namespace smart_assert {

namespace Private {

// forward declaration
class keeper;


// in case the tested value is non-bool, it's very likely
// that the expression is a simple-assert, like:
// SMART_ASSERT(p); // for asserting a pointer
//
// it's simpler and more straightforward to say: 
// 'SMART_ASSERT(p)' than
// 'SMART_ASSERT(p)(p)'
//
// note: moved outside Keeper class, due to VC6 bug
// (complains about ambigous call to overloaded function)
template< class type>
inline void log_val_if_nonbool( keeper & k, const type & val, const char * strExpr);

// ...specialize for bool
#ifndef BOOST_SMART_ASSERT_VC6_WORKAROUND
inline void log_val_if_nonbool( keeper &, const bool &, const char * ) {
}
#else
// for VC6
inline void log_val_if_nonbool< bool>( keeper &, const bool &, const char * ) {
}
#endif


// workaround for GCC 3.2 - does not see the 'value_type' typename defined 
// in keeper, if used here
typedef std::pair< string_type, string_type> keeper_value_type;


// pre-logs a given variable, written using the v_ macro;
//
// note: this should be called very rarely; the v_ macro should
// be used only when you have lengthy functions.
template< class T>
void BOOST_SMART_ASSERT_keeper_print_current_val_impl( keeper & k, const T & val, const char * msg);
template< class T>
void BOOST_SMART_ASSERT_keeper_print_current_val( keeper & k, const T & val, const char * msg);

BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const bool               , const char*, bool              )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const char               , const char*, char              )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const signed char        , const char*, signed_char       )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const unsigned char      , const char*, unsigned_char     )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const signed short int   , const char*, signed_short_int  )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const unsigned short int , const char*, unsigned_short_int)
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const int                , const char*, int               )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const unsigned int       , const char*, unsigned_int      )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const long               , const char*, long              )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const unsigned long      , const char*, unsigned_long     )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const char*              , const char*, char_ptr          )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const signed char*       , const char*, signed_char_ptr   )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const unsigned char*     , const char*, unsigned_char_ptr )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const float              , const char*, float             )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const double             , const char*, double            )
BOOST_SMART_ASSERT_DECL_OUTOFLINE3( BOOST_SMART_ASSERT_keeper_print_current_val, keeper &, const long double        , const char*, long_double       )




/*
    helper class, to keep:
    - the expression string
    - the lengthy function's name & value, in case such a 
      function is used within an ASSERT
*/
class keeper {

public:
    typedef keeper_value_type value_type;
    typedef std::vector< value_type, Private::smart_assert_alloc< value_type> > values_array;

    /*
        July 19, 2003
        Note for VC6: keeper's constructor seems to 
        generate a LOT of code (it's not inlined)

        Therefore, the keeper is not included by default
        in SMART_ASSERT.
        
        Since the situations in which it's required are VERY FEW,
        two new macros have been created:
        SMART_ASSERT_V and
        SMART_VERIFY_V
        
    */
    keeper( int);
    operator bool() { return false; }

    template< class type>
    type test_expr( type val, const char * strExpr) {
        m_strExpr = strExpr;
        ::boost::smart_assert::Private::log_val_if_nonbool( *this, val, strExpr);
        return val;
    }

    // pre-logs a given variable, written using the v_ macro;
    //
    // note: this should be called very rarely; the v_ macro should
    // be used only when you have lengthy functions.
    template< class T>
    void print_current_val( const T & val, const char * msg) {
        BOOST_SMART_ASSERT_keeper_print_current_val( *this, val, msg);
    }

    const values_array & get_values_array() const {
        return m_values;
    }

    const char_type* get_expr() const {
        return m_strExpr;
    }

    values_array & get_values_array() {
        return m_values;
    }

private:

    // the values that were printed with the v_ macro
    values_array m_values;

    // the expression string - note that is always something like '#expr',
    // therefore embodied within the executable; no need to keep it as a string
    const char_type * m_strExpr;

};


// helper - allows logging a value using the v_ macro
template< class type>
struct log_me_t {
    log_me_t( const type & val, const char * str, keeper & k)
            : m_val( val) { 
        k.print_current_val( val, str); }

    const type & m_val;
};

template< class type> 
inline log_me_t< type> log_me( const type & val, const char * str, keeper & k) { 
    return log_me_t< type>( val, str, k); }


// the user can disable the v_ macro, in case 
// it might conflict with its own code
//
// thanks Pavel Vozenilek!
#ifndef BOOST_SMART_ASSERT_DISABLE_V_
    #define v_(x) boost::smart_assert::Private::log_me((x), #x, boost_private_assert_keeper).m_val
#endif

template< class type>
inline void log_val_if_nonbool( keeper & k, const type & val, const char * strExpr) {
    k.print_current_val( val, strExpr);
}

template< class T>
void BOOST_SMART_ASSERT_keeper_print_current_val_impl( keeper & k, const T & val, const char * msg) {

    typedef printer_class<0>::type printer_type;
    assert_context context;
    printer_type printer;
    printer.print_val( context, val, msg);

    k.get_values_array().push_back( keeper_value_type( 
        context.get_vals_array()[0].m_val, context.get_vals_array()[0].m_str));
}

template< class T>
void BOOST_SMART_ASSERT_keeper_print_current_val( keeper & k, const T & val, const char * msg) {
    BOOST_SMART_ASSERT_keeper_print_current_val_impl( k, val, msg);
}

} // namespace Private

} // namespace smart_assert

} // namespace boost



#endif
