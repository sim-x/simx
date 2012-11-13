// impl/smart_assert.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_IMPL_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_IMPL_HPP_INCLUDED


#if _MSC_VER >= 1020
#pragma once
#endif


#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_settings.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_l_basic.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_util.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_init.hpp>
#include <boost/config.hpp>

namespace boost {

namespace smart_assert {

// see 'smart_assert/smart_assert/priv/fwd/macros.hpp' for more details
#undef SMART_ASSERT_A
#undef SMART_ASSERT_B
#undef SMART_ASSERT_OP


#ifdef BOOST_MSVC
#pragma warning (push)
// *this used in initializer list - this is ok ;-)
#pragma warning( disable: 4355)
#endif

extern "C" {
Assert make_assert() {
    return Assert();
}
}


Assert::Assert() 
    : SMART_ASSERT_A( *this),
      SMART_ASSERT_B( *this),
      m_nLevel( lvl_debug),
      m_bIgnoreThisAssert( false)
{
    // make sure we're initialized!
    initializer().initialize();

    // FIXME - if we call initialize(), if
    // initialize calls an assert, we should guard against
    // that!!!!

    // FIXME - find out if we can use assert_settings(),
    // (or if within initialize()) and in this case,
    // we'll output to std::cerr !!! (also, no handling whatsoever!)
    //

    // FIXME - after smart_assert_settings is destructed
    // we can't allow throwing exceptions. Therefore, an
    // exceptional handler should be used - TOTHINK - enforcements (maybe this is not such a good idea)?
}


// copy-construction
//
// SMART_ASSERT_A and SMART_ASSERT_B should point to *this
// 2004-07-15 10:36:03 kbisset
Assert::Assert( const Assert & other) 
    : assert_base_type( other),
      SMART_ASSERT_A( *this),
      SMART_ASSERT_B( *this),
      m_nLevel( other.m_nLevel),
      m_printer( other.m_printer),
      m_context( other.m_context),
      m_bIgnoreThisAssert( other.m_bIgnoreThisAssert)
{
    // if copying an ASSERTion, this means we don't need
    // the old one
    other.m_bIgnoreThisAssert = true;
}

#if (defined BOOST_MSVC)
#  pragma warning (pop)
#endif


#include <simx/smart_assert/smart_assert/priv/fwd/macros.hpp>


Assert::~Assert()  {
    if ( m_bIgnoreThisAssert)
        return;

    // note:
    // we do the real hanlding of the assertion right here;
    // this is because only here we have ALL of the context
    // information (for instance, the user can, at any time,
    // add a context, set the debug level, or whatever else)

    // note2:
    // you might consider it very dangerous to handle the assert 
    // so late - in a destructor, since something might throw;
    // this is true, but I cannot handle it any sooner, 
    // since I don't know when the user has set all the information
    // 
    // For instance, we might have:
    // SMART_ASSERT(i != j)(i)(j)
    //      .context("func", "get_app()").debug( "i different from j");
    //
    // or simply:
    // SMART_ASSERT(i != j)(i)(j)
    //      .context("func", "get_app()");
    // 
    // or
    // SMART_ASSERT(i != j)(i)(j)
    //      .debug( "i different from j");
    //
    // or
    // SMART_ASSERT(i != j)(i)(j);
    //
    //
    // throwing an exception from Assert's destructor
    // is not as problematic as you might think.
    // In our case, we cannot be within the context of unwinding
    // as exception, since we guard against every possible exception
    handle_assert( m_context);
}







// handles this failed assertion
void Assert::handle_assert( const assert_context & context) {
#ifndef BOOST_NO_EXCEPTIONS
    try {
#endif
        assert_settings().log( context);
#ifndef BOOST_NO_EXCEPTIONS
    }
    catch(const std::exception & exc) {
        assert_settings().get_dumper() << "[SMART_ASSERT logical error] "
            "your logger class is implemented wrong.\n"
            "it should NEVER throw an exception: '" 
            << exc.what() << '\'' << std::endl;
    }
    catch(...) {
        assert_settings().get_dumper() << "[SMART_ASSERT logical error] "
            "your logger class is implemented wrong.\n"
            "it should NEVER throw an exception" << std::endl;
    }
#endif
    // handle the failed ASSERTion
    //
    // might throw!
    assert_settings().handle( context);
}


Assert& Assert::print_prewrittern_values( const Private::keeper & val) {

#ifndef BOOST_NO_EXCEPTIONS
    try {
#endif
        // first, log the expression
        m_context.set_expr( val.get_expr());

        // now, log the values written using the v_ macro
        typedef Private::keeper::values_array values_array;
        values_array::const_iterator
            first = val.get_values_array().begin(), last = val.get_values_array().end();
        while ( first != last) {
            m_printer.print_val( m_context, 
                // the value, then its corresponding string
                //
                // note: the value's type has been lost,
                // on Private::keeper::log_current_var
                first->first, first->second.c_str());
            ++first;
        }
#ifndef BOOST_NO_EXCEPTIONS
    }
    catch (...) {
    }
#endif
    return *this;
}



BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const bool               , bool              )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const char               , char              )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const signed char        , signed_char       )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const unsigned char      , unsigned_char     )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const signed short int   , signed_short_int  )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const unsigned short int , unsigned_short_int)
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const int                , int               )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const unsigned int       , unsigned_int      )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const long               , long              )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const unsigned long      , unsigned_long     )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const char*              , char_ptr          )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const signed char*       , signed_char_ptr   )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const unsigned char*     , unsigned_char_ptr )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const float              , float             )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const double             , double            )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3( BOOST_SMART_ASSERT_context, assert_context&,  const char_type*, const long double        , long_double       )


BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const bool               , const char_type*, bool              )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const char               , const char_type*, char              )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const signed char        , const char_type*, signed_char       )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const unsigned char      , const char_type*, unsigned_char     )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const signed short int   , const char_type*, signed_short_int  )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const unsigned short int , const char_type*, unsigned_short_int)
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const int                , const char_type*, int               )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const unsigned int       , const char_type*, unsigned_int      )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const long               , const char_type*, long              )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const unsigned long      , const char_type*, unsigned_long     )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const char*              , const char_type*, char_ptr          )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const signed char*       , const char_type*, signed_char_ptr   )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const unsigned char*     , const char_type*, unsigned_char_ptr )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const float              , const char_type*, float             )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const double             , const char_type*, double            )
BOOST_SMART_ASSERT_DEF_OUTOFLINE3(BOOST_SMART_ASSERT_print_current_val, Assert&, const long double        , const char_type*, long_double       )



} // namespace smart_assert

} // namespace boost


#endif


