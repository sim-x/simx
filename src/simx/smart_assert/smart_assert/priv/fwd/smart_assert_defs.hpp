// fwd/smart_assert_defs.hpp

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

#ifndef BOOST_INNER_SMART_ASSERT_DEFS_HPP_INCLUDED
#define BOOST_INNER_SMART_ASSERT_DEFS_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif

#include <string>
#include <sstream>
#include <boost/current_function.hpp>
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_alloc.hpp>

namespace {
    // optimize storage (otherwise, using this multiple times
    // could make the compiler insert the same string multiple times
    // into the executable, making it much larger)

    // note: using 'const char BOOST_SMART_ASSERT_FILE[] = __FILE__'
    // does not work, since __FILE__ = ".../smart_assert_defs.hpp"
    #define BOOST_SMART_ASSERT_FILE __FILE__
    const char BOOST_SMART_ASSERT_FILE_KEY[] = "file";
    const char BOOST_SMART_ASSERT_LINE_KEY[] = "line";
    const char BOOST_SMART_ASSERT_FUNCTION_KEY[] = "function";
} // anonymous namespace

namespace boost { 

namespace smart_assert {


// ... internals
typedef char char_type;
typedef std::basic_string< char_type, std::char_traits< char_type>, Private::smart_assert_alloc< char_type> > string_type;
typedef std::basic_ostringstream< char_type, std::char_traits< char_type>, Private::smart_assert_alloc< char_type> > ostringstream_type;
typedef std::basic_istringstream< char_type, std::char_traits< char_type>, Private::smart_assert_alloc< char_type> > istringstream_type;

// ... what the clients of the library might use
typedef std::basic_string< char_type> client_string_type;
// note: no need for client_*stringsteam_type, since
// stringstreams are used ONLY internally
typedef std::ostream client_ostream_type;
typedef std::istream client_istream_type;
typedef std::iostream client_iostream_type;






/* 
    default ASSERT levels
    (you can add your own, just make sure it does not
    mix with this)

    IMPORTANT
    Convention:

    The bigger the level is, the more critical is the error.
    For instance, a warning has a very low level number.
    The debug level is more critical than a warning, etc.

    As I said, you can add your own custom levels, just make sure
    you stick to this convension.

    Do it because the current code (and future code, for this matter) 
    assumes this of you.
*/
enum {

    // default behavior - just loggs this assert
    // (a message is shown to the user to the console)
    lvl_warn = 100,

    // default behavior - asks the user what to do:
    // Ignore/ Retry/ etc.
    lvl_debug = 200,

    // 2004-07-15 10:40:20 kbisset
    lvl_info = 250,

    // default behavior - throws a smart_assert_error
    lvl_error = 300,

    // used with SMART_ENFORCE
    // default behavior - throws a smart_assert_error
    lvl_enforce = 301,

    // default behavior - dumps all assert context to console, 
    // and aborts
    lvl_fatal = 1000
};


/*
    when ignoring an ASSERTion, how are we ignoring it?
*/
typedef enum ignore_type {
    
    // ignores this ASSERTion now
    // (if it triggers again, it won't be ignored by default)
    ignore_now,

    // ignores this ASSERTion forever 
    // (throughout the lifetime of the program)
    ignore_forever,

    // ignores all ASSERTions from now on
    // (note: they are still logged...)
    ignore_all,

    // ignores all ASSERTions related to this assertion
    // throughout the lifetime of the program
    //
    // NOTE - not implemented yet
    ignore_all_related,


    // ignores this ASSERTion forever
    //
    // (if ASSERTions are persisted, this will be ignored throughout
    //  next runs of the same program;
    //
    //  otherwise, behaves like ignore_forever)
    ignore_forever_persistent,


    // ignores all ASSERTions related to this assertion
    // throughout the lifetime of the program
    //
    // (if ASSERTions are persisted, they will be ignored throughout
    //  next runs of the same program;
    //
    //  otherwise, behaves like ignore_all_related)
    //
    // NOTE - not implemented yet
    ignore_all_related_persistent,


    // ignores all ASSERTions from now on
    // (note: they are still logged...)
    //
    // (if ASSERTions are persisted, they will be ignored throughout
    //  next runs of the same program;
    //
    //  otherwise, behaves like ignore_all)
    ignore_all_persistent

    /*
        possible future improvement

        ignore_this_function:
        - ignores all assertions from this function, for the lifetime
          of the program, when this is possible
          (the BOOST_CURRENT_FUNCTION macro)

      */

} 
    //  BC++ needs this
    //thanks Pavel Vozenilek!
    ignore_type; // typedef enum ignore_type 




// note: we're not throwing a std::runtime_exception, since
// you construct a runtime_exception by passing a std::string;
//
// creating a temporary std::string to pass to a runtime_exception,
// in case the system has run out of memory could fail!!!
struct smart_assert_error : public std::exception {
    smart_assert_error( const string_type & val) : m_val( val) {
    };
    smart_assert_error( const char * val) : m_val( val) {
    };

    BOOST_SMART_ASSERT_CDECL ~smart_assert_error() throw() {
    }

    const char * BOOST_SMART_ASSERT_CDECL what() const throw() {
        return m_val.c_str();
    }
private:
    string_type m_val;
};



} // namespace smart_assert

} // namespace boost



// thanks Pavel Vozenilek!
// based on boost/current_function.hpp
namespace boost { namespace smart_assert { namespace Private {

// note: I need the BOOST_SMART_ASSERT_FUNCTION_EXISTS macro, in order
// to know if I add the BOOST_SMART_ASSERT_FUNCTION to the context or not
inline void current_function_helper() // SGI's __FUNCSIG__ requires this
{

#define BOOST_SMART_ASSERT_FUNCTION_EXISTS

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000))
# define BOOST_SMART_ASSERT_FUNCTION __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
# define BOOST_SMART_ASSERT_FUNCTION __FUNCSIG__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
# define BOOST_SMART_ASSERT_FUNCTION __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
# define BOOST_SMART_ASSERT_FUNCTION __func__
#elif (defined BOOST_MSVC) && (__MSC_VER >= 1300)
# define BOOST_SMART_ASSERT_FUNCTION __FUNCDNAME__
#elif  defined(__IBMCPP__) && (__IBMCPP__ <= 500)
# define BOOST_SMART_ASSERT_FUNCTION __FUNCTION__
#elif (defined __HP_aCC) && (__HP_aCC <= 33300)
# define BOOST_SMART_ASSERT_FUNCTION __FUNCTION__
#else
# undef BOOST_SMART_ASSERT_FUNCTION_EXISTS

#endif

} } } }  // namespaces


// note: the user can define SMART_ASSERT_CONTEXT prior to 
// #including any of smart_assert*.hpp files, to use its own!
#ifndef SMART_ASSERT_CONTEXT
    

    // FIXME (nice to have)
    // include the thread-id of the current thread, when the assertion fails
    // TOTHINK how will find get it? (in a portable way?)

    #ifdef BOOST_SMART_ASSERT_FUNCTION_EXISTS
        #define SMART_ASSERT_CONTEXT context(BOOST_SMART_ASSERT_FILE_KEY,BOOST_SMART_ASSERT_FILE).context(BOOST_SMART_ASSERT_LINE_KEY,__LINE__).context(BOOST_SMART_ASSERT_FUNCTION_KEY,BOOST_SMART_ASSERT_FUNCTION)
    #else
        #define SMART_ASSERT_CONTEXT context(BOOST_SMART_ASSERT_FILE_KEY,BOOST_SMART_ASSERT_FILE).context(BOOST_SMART_ASSERT_LINE_KEY,__LINE__)
    #endif

#endif // SMART_ASSERT_CONTEXT


#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

    #ifndef BOOST_DISABLE_WIN32
        #define BOOST_SMART_ASSERT_WIN32
    #endif // ndef BOOST_DISABLE_WIN32
#endif


/*
// FIXME - mail from Pavel



> > > 2. If assert is invoked from thread in A.exe and some other thread in
> > B.dll asserts you get two message boxes or two console messages. This
> scenario
> > is definitely possible in real apps.

For the two concurrent message boxes, we're in Win32 so we can do it somehow.
I think named events should be the easiest way.

(and yet I just thought a little more about it, and named events are not so good, after all - named events would be global for ALL running applications)

For the two concurrent console messages (I assume you mean when the user is asked to Ignore/debug/... from two different threads), I don't know. 

Frankly, this is quite twisted. I wanna see what happens on Win32 (two threads waiting for input), and have no idea what happens on unixes.

I guess I'll post a question on comp.lang.c++.moderated.


*/


#endif // BOOST_INNER_SMART_ASSERT_DEFS_HPP_INCLUDED
