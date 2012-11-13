// fwd/smart_assert_print.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_PRINT_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_PRINT_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

// classes for printing the variables that
// are involved in an ASSERT, in case the ASSERT fails

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_context.hpp>
#include <iostream>

namespace boost { 

namespace smart_assert {

namespace Private {

/* 
    Allows printing a range

  Example:

  ASSERT( *first != *(last-1) )(range_(first,last));

  Will print out the values of first, last, and the range
  [first, last)
*/
template< class iterator_type>
struct printer_range_t {
    printer_range_t( iterator_type first, iterator_type last)
        : m_first( first), m_last( last) {
    }

    // note: we treat the iterator_type as dereferenceable!!!
    iterator_type m_first, m_last;
};


template< class iterator_type>
inline client_ostream_type & operator<<( client_ostream_type & out, const printer_range_t< iterator_type> &val) {
    /* 
        STLPort - cannot dereference val.m_last! 
    */
    int count = 0;
    if ( val.m_first != val.m_last) {
        out << "Range starting at [" << &*(val.m_first) << "]; "
            << "Values= [";
    }
    else {
        out << "Range= [";
    }

    iterator_type first = val.m_first;
    while ( first != val.m_last) {
        if ( first != val.m_first)
            out << ", ";
        out << '\'' << *first << '\'';
        ++count;
        ++first;
    }
    out << ']';
    if ( count > 0)
        out << "( " << count << " values)";
    return out;
}

} // namespace Private

template< class iterator_type>
inline Private::printer_range_t< iterator_type> range_( iterator_type first, iterator_type last) {
    return Private::printer_range_t< iterator_type>( first, last);
}




namespace Private {

    /*
        finds a NULL 'char*' pointer.
        This is because writing a null 'char*' might crash your program 
        (this happens for several platforms).

        Example:
        char* p = 0;
        // this can crush the program!!!
        // because internally it could call strlen( p), 
        // and if p is null, this could crash the program
        std::cout << p;
    */
    template< class T>
    struct is_null_finder {
        bool is( const T &) const {
            return false;
        }
    };

    template<>
    struct is_null_finder< char*> {
        bool is( char * const & val) {
            return val == 0;
        }
    };

    template<>
    struct is_null_finder< const char*> {
        bool is( const char * const & val) {
            return val == 0;
        }
    };


} // namespace Private

class default_printer {

public:
    template< class T>
    int print_val( assert_context & context, T const & val, const char_type * str) {
        ostringstream_type out;
        
        Private::is_null_finder< T> f;
        bool bIsNull = f.is( val);
        if ( !bIsNull)
            out << val;
        else
            // null string
            out << "null";
        
        return context.add_val( out.str(), str);
    }

};

} // namespace smart_assert

} // namespace boost


#endif
