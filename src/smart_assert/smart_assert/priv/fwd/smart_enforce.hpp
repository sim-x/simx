// fwd/smart_enforce.hpp

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

#ifndef BOOST_INNER_SMART_ENFORCE_HPP_INCLUDED
#define BOOST_INNER_SMART_ENFORCE_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

/*
    Many thanks to Andrei Alexandrescu, 
    for letting me borrow his code.
*/

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert.hpp>

#include <string>
#include <sstream>
#include <stdexcept>

namespace boost {

namespace smart_assert {

struct default_predicate
{
  template <class T>
  static bool is_wrong(const T& obj)
  {
    return !obj;
  }
};


/*

 Note

    Enforce must be as fast as possible (unlike SMART_ASSERT).
    This is why we don't derive from Assert (it would then
    be too slow).

    This has a drawback: you won't be able to call ALL of the 
    methods you're used to when using SMART_ASSERT.
    You'll have to do with these methods:
    - .context()
    - .msg()
    
    The level is a "policy", defaulting to 'lvl_enforce'. 

    Also, unfortunately, you won't be able to use the v_ macro.

 IMPORTANT:
    We need for ENFORCE to be as fast as possible,
    therefore we're making everything inline!
*/
template<typename Ref, int nLevel>
class Enforcer 
{
    typedef Enforcer< Ref, nLevel> this_class;
    
    this_class& operator=( this_class & not_implemented);

public:

    template< class predicate>
    Enforcer(Ref ref, const char * strExpr, predicate)
        : SMART_ENFORCE_A( *this),
          SMART_ENFORCE_B( *this),
          m_ref( ref),
          m_pAssert( 0) {

        if ( predicate::is_wrong( ref)) {

            // FIXME (nice to have) - use our allocator, make sure we have
            // the memory for creating it!
            m_pAssert = new Assert;
            m_pAssert->level( nLevel);
            m_pAssert->set_expr( strExpr);

            // we're printing the expression itself, considering it
            // like a variable
            //
            // this makes perfect sense, since if we're here, the enforcement
            // failed; but with what value, we might ask ourselves
            //
            // the following expression finds that out.
            m_pAssert->print_current_val( ref, strExpr);
        }
    }

    Enforcer( const Enforcer & other) 
        : SMART_ENFORCE_A( *this),
          SMART_ENFORCE_B( *this),
          m_ref( other.m_ref),
          m_pAssert( other.m_pAssert) {
    }

    // FIXME (MUST) - I should probably allow enforcements not to be
    // logged, if this is what the user wants.
    //
    // this could be made more general, meaning that some
    // ASSERT levels would not be logged.
    //
    // this makes sense, for instance, even for SMART_VERIFY,
    // which sometimes you don't want logging for.
    //
    // IMPORTANT: maybe this thing should be included in the 
    // configuration as well !- TOTHINK



    template< class type>
    Enforcer & print_current_val( const type & val, const char_type * str) {
        if ( m_pAssert)
            m_pAssert->print_current_val( val, str);
        return *this;
    }

    template< class type>
    Enforcer & context( const char_type * str, const type & val) {
        if ( m_pAssert)
            m_pAssert->context( str, val);
        return *this;
    }

    Enforcer & msg( const char_type * strMsg) {
        if ( m_pAssert)
            m_pAssert->msg( strMsg);
        return *this;
    }

    Ref operator*() const {
        // this might throw!
        delete m_pAssert;
        return m_ref;
    }

public:
    Enforcer& SMART_ENFORCE_A;
    Enforcer& SMART_ENFORCE_B;

private:
    Ref m_ref;

    // note: it is destroyed on operator* !!!
    // (no need to destroy it in the destructor, since we always
    //  know operator* will be called!)
    Assert * m_pAssert;
};



template< 
    int nLevel,
    class T, class predicate>
inline Enforcer<const T&, nLevel>
    make_enforcer(
        const T& t, const char * strExpr, predicate p) {

    return Enforcer< const T&, nLevel>( t, strExpr, p);
}

template< 
    int nLevel,
    class T, class predicate>
inline Enforcer< T&, nLevel>
    make_enforcer(
        T& t, const char * strExpr, predicate p) {

    return Enforcer< T&, nLevel>( t, strExpr, p);
}


#define BOOST_CREATE_SMART_ENFORCE( expr, level, predicate) \
    *::boost::smart_assert::make_enforcer< level>( \
        (expr), #expr, predicate).SMART_ASSERT_CONTEXT.SMART_ENFORCE_A

#define SMART_ENFORCE(expr) BOOST_CREATE_SMART_ENFORCE(expr, ::boost::smart_assert::lvl_enforce, ::boost::smart_assert::default_predicate())
#define SMART_ENFORCE_EXT(expr, predicate) BOOST_CREATE_SMART_ENFORCE(expr, ::boost::smart_assert::lvl_enforce, predicate)


#define SMART_ENFORCE_A(x) SMART_ENFORCE_OP(x, B)
#define SMART_ENFORCE_B(x) SMART_ENFORCE_OP(x, A)

#define SMART_ENFORCE_OP(x, next) \
    SMART_ENFORCE_A.print_current_val((x), #x).SMART_ENFORCE_ ## next \
    /**/




} // namespace smart_assert

} // namespace boost


#endif
