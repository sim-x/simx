// fwd/smart_assert_alloc.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_ALLOC_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_ALLOC_HPP_INCLUDED

// our custom memory allocator
//
// it pre-allocates some memory, so we won't have any
// memory problems, if an assertion fails.


// note: 
// #define BOOST_SMART_ASSERT_NO_CUSTOM_ALLOC if you 
// don't want a custom allocator (therefore, default
// allocator will be used)


#if _MSC_VER >= 1020
#pragma once
#endif

#include <memory>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_ts.hpp>
#include <boost/config.hpp>

#ifndef BOOST_SMART_ASSERT_PREALLOCATE 
#define BOOST_SMART_ASSERT_PREALLOCATE 1024* 20 // 20k per type
#endif

#ifndef BOOST_SMART_ASSERT_PREALLOCATE_CHAR 
#define BOOST_SMART_ASSERT_PREALLOCATE_CHAR 1024* 100 // 100k for char, wchar_t
#endif

// FIXME - thoroughly test this

// FIXME - gcc has problems with custom allocators
//         can anybody help?
#ifdef __GNUC__
#define BOOST_SMART_ASSERT_NO_CUSTOM_ALLOC
#endif


namespace boost {

namespace smart_assert {

namespace Private {

#ifndef BOOST_SMART_ASSERT_NO_CUSTOM_ALLOC

// for a given type, how many objects can we pre-allocate?
template< class type>
struct pre_allocate_count {
    BOOST_STATIC_CONSTANT( int, value = (BOOST_SMART_ASSERT_PREALLOCATE / sizeof(type)) );

};

template<>
struct pre_allocate_count< char> {
    BOOST_STATIC_CONSTANT( int, value = BOOST_SMART_ASSERT_PREALLOCATE_CHAR);
};


template< class type>
struct allocator_array {
    
    allocator_array() {
    }

    BOOST_STATIC_CONSTANT( int, max = pre_allocate_count< type>::value);

    // try to allocate; if cannot, return 0
    type * allocate( int nChunk) {
        lock_type locker( m_cs);

        int nConsecutive = 0;
        int idxStartOfChunk = 0;
        for ( int idx = 0; idx < max; ++idx) {
            if ( is_allocated( idx)) 
                nConsecutive = 0;
            else {
                if ( nConsecutive == 0)
                    idxStartOfChunk = idx;
                ++nConsecutive;
            }

            if ( nConsecutive >= nChunk) {
                // we can allocate
                for ( int idxAllocated = 0; idxAllocated < nChunk; ++idxAllocated) 
                    set_as_allocated( idxStartOfChunk + idxAllocated);
                return m_vals + idxStartOfChunk;
            }
        }

        // we cannot allocated
        return 0;
    }

    void deallocate( type * p, int nChunk) {
        lock_type locker( m_cs);

        int idxStart = p - begin();
        if ( idxStart >= max ) 
            // should NEVER happen
            return;

        for ( int idx = 0; idx < nChunk; ++idx)
            set_as_deallocated( idxStart + idx);
    }

    const type * begin() const { return m_vals; }
    const type * end() const { return m_vals + max; }

    // returns true if the given pointer is within our
    // allocated space
    bool is_in_range( const void * p) const {

        // if not within our range, ignore
        const void * first = begin();
        const void * last = end();

        if ( ( p >= first) && ( p < last))
            return true;
        else
            return false;
    }

private:
    // returns true if allocated at index
    bool is_allocated( int idx) const {
        unsigned char ch = m_allocated_vals[ idx / 8];
        int nAt = idx % 8;

        int bIs = ( ch >> nAt) & 1;
        return bIs != false;
    }

    void set_as_allocated( int idx) {
        unsigned char & ch = m_allocated_vals[ idx / 8];
        int nAt = idx % 8;

        ch |= ( 1 << nAt);
    }

    void set_as_deallocated( int idx) {
        unsigned char & ch = m_allocated_vals[ idx / 8];
        int nAt = idx % 8;

        ch &= ~( 1 << nAt);
    }
private:
    // the data we can allocate
    type m_vals[ max];
    // which data is allocated already?
    unsigned char m_allocated_vals[ (max + 7) / 8];

    // thread-safety
    mutex_type m_cs;
};



#endif // BOOST_SMART_ASSERT_NO_CUSTOM_ALLOC


// our own allocator; 
//
// pre-allocate some memory first
template< class type>
class smart_assert_alloc : public std::allocator< type> {

public:
    smart_assert_alloc() {
    }

    template< class other_type>
        smart_assert_alloc( const smart_assert_alloc< other_type> &) {
    }

#ifndef BOOST_MSVC
    // VC6 would choke on this
    // 2004-07-15 10:34:22 kbisset
    smart_assert_alloc( const smart_assert_alloc< type> &)
    : std::allocator<type>() {
    }

    // rebinding
    template< class other_type>
        struct rebind {
        typedef smart_assert_alloc< other_type> other;
    };

#endif

#ifndef BOOST_SMART_ASSERT_NO_CUSTOM_ALLOC

    // for MSVC
    friend class smart_assert_alloc< char>;

    typedef std::allocator< type> base;
    typedef typename base::pointer pointer;
    typedef typename base::size_type size_type;

    pointer allocate(size_type n, const void *hint = 0) {

        pointer p = s_allocator().allocate( n);
        if ( p)
            return p;
        // fallback to default
        return base::allocate( n, hint);
    }

#ifndef BOOST_MSVC
    void deallocate(pointer p, size_type n) {
#else
    // VC6 bug
    void deallocate(void *pvoid, size_type n) {
        pointer p = ( pointer)pvoid;
#endif

        // if not within our range, ignore
        if ( !s_allocator().is_in_range( p) ) {

#ifdef BOOST_MSVC
            // VC has an extra function - _Charalloc
            // account for that as well
            smart_assert_alloc< char> a;
            if ( a.s_allocator().is_in_range( p) ) {
                // FIXME (question) - should this be n*sizeof(type) ??????
                a.s_allocator().deallocate( ( char *)p, n);
                return;
            }
#endif // BOOST_MSVC

            // not allocated by us...
            base::deallocate( p, n);
            return;
        }

        s_allocator().deallocate( p, n);        
    }

    void construct(pointer p, const type & v) {

        if ( s_allocator().is_in_range( p))
            // already constructed - that's how our allocator works
            *p = v;
        else
            base::construct( p, v); 
    }

    void destroy(pointer p) {

        if ( s_allocator().is_in_range( p)) 
            // no need to destroy
            ;
        else
            base::destroy( p);
    }


#ifdef BOOST_MSVC
    char *_Charalloc(size_type _N) {
        smart_assert_alloc< char> a;
        return a.allocate((difference_type)_N, 0); 
    }
#endif // BOOST_MSVC

    static allocator_array< type> & s_allocator() {
        // the object that actually knows how to allocate/deallocate
        static allocator_array< type> inst;
        return inst;
    }

#endif // BOOST_SMART_ASSERT_NO_CUSTOM_ALLOC
};


} // namespace Private

} // namespace smart_assert

} // namespace boost



#endif

