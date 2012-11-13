// fwd/smart_assert_ctxfunc.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_CONTEXT_FUNC_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_CONTEXT_FUNC_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif


#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_l_basic.hpp>


namespace boost { 

namespace smart_assert {

#include <boost/config.hpp>

// taken from boost/function/function_base.hpp
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300 || defined(__ICL) && __ICL <= 600 || defined(__MWERKS__) && __MWERKS__ < 0x2406
#  define BOOST_FUNCTION_SMART_ASSERT_TARGET_FIX(x) x
#else
#  define BOOST_FUNCTION_SMART_ASSERT_TARGET_FIX(x)
#endif // not MSVC



// forward declaration(s)
class assert_context;

namespace Private {


    // note: we created our own assert_context wrapper,
    // because #including <boost/function.hpp> would take too long!!!
    //
    // besides, VC6 gives an INTERNAL COMPILER ERROR

    struct assert_context_func_impl_base {

        virtual ~assert_context_func_impl_base() {}
        virtual void call_func( const assert_context &context) = 0;
        virtual assert_context_func_impl_base* clone() const = 0;
    };

    template< class type>
    struct assert_context_func_impl : public assert_context_func_impl_base {

        assert_context_func_impl() : m_f() {
        }
        assert_context_func_impl( type f) : m_f( f) {
        }
        void call_func( const assert_context &context) {
            (m_f)( context);
        }
        assert_context_func_impl_base* clone() const {
            // FIXME (nice to have) - I could use smart_assert_alloc:
            // here and below
            //
            // thanks Pavel Vozenilek!
            return new assert_context_func_impl< type>( *this);
        }
    private:
        type m_f;
    };

} // namespace Private 


// functor taking an assert_context argument
//
struct assert_context_func_base {

    assert_context_func_base() : m_pimpl( 0) {
    }

    assert_context_func_base( const assert_context_func_base & val) : m_pimpl( 0) {
        m_pimpl = get_clone( val.m_pimpl);
    }

protected:
    static Private::assert_context_func_impl_base * get_clone( Private::assert_context_func_impl_base * pimpl) {
        return pimpl ? pimpl->clone() : 0;
    }


protected:
    mutable Private::assert_context_func_impl_base * m_pimpl;

};

class assert_context_func : private assert_context_func_base {

    typedef void (*func)( const assert_context&);

public:
    assert_context_func() {
    }

#ifndef BOOST_MSVC
    assert_context_func( assert_context_func const & val) 
        : assert_context_func_base( val) {
    }
#endif

    template< typename Functor>
    assert_context_func( Functor BOOST_FUNCTION_SMART_ASSERT_TARGET_FIX(const &) val) {
        assign( val);
    }


    ~assert_context_func() {
        delete m_pimpl;
    }

    void operator() ( const assert_context &context) const {
        if ( m_pimpl)
            m_pimpl->call_func( context);
    }

    void operator=( const assert_context_func & val) {
        copy( val);
    }

private:
    void copy( const assert_context_func & val) {
        delete m_pimpl;
        // ... in case creating the new object throws
        m_pimpl = 0;
        m_pimpl = get_clone( val.m_pimpl);
    }

    template< class T> 
        void assign( const T & val) {
    
        delete m_pimpl;
        // ... in case creating the new object throws
        m_pimpl = 0;
        m_pimpl = new Private::assert_context_func_impl< T>(val);
    }

    void assign( func f) {

        delete m_pimpl;
        // ... in case creating the new object throws
        m_pimpl = 0;
        m_pimpl = new Private::assert_context_func_impl< func>(f);
    }

};

#undef BOOST_FUNCTION_SMART_ASSERT_TARGET_FIX

} // namespace smart_assert

} // namespace boost



#endif
