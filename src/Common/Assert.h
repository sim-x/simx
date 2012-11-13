//--------------------------------------------------------------------------
// $Id: Assert.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    assert.h
// Author:  K. Bisset
// Created: October 30 2000
// Description:
//
// Special definition of 'assert' macro (uses UIS abort);
// always-active NISAC_ASSERT; and always-active boost SMART_ASSERTs.
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_COMMON_ASSERT_H 
#define NISAC_COMMON_ASSERT_H

//--------------------------------------------------------------------------

// Disable the horrible "v_" macro, which conflicts with other things in boost.
#define BOOST_SMART_ASSERT_DISABLE_V_
#include "simx/smart_assert/smart_assert.hpp"

//--------------------------------------------------------------------------
// assert: just like normal assert() from assert.h, except use our
// abort if the assertion fails.
//--------------------------------------------------------------------------

#undef assert

#if defined(NDEBUG)
#define	assert(EX) ((void)0)
#else

namespace Log {
  void cppAssert(const char* ex, const char* file, int line);
}

#define	assert(EX) (void)((EX) || \
(Log::cppAssert(#EX, __FILE__, __LINE__), 0))

#endif // NDEBUG

//--------------------------------------------------------------------------
// NISAC_ASSERT: active unless DISABLE_NISAC_ASSERTS is defined
//--------------------------------------------------------------------------

namespace Log
{
  void Log_Assert
  (const char* expr, const char* func,
   const char* file,long line,const char* msg);

  void Log_Checkpoint
  (const char *func,const char *file,long line, const char *msg);
}

#undef NISAC_ASSERT
#undef NISAC_ASSERT_MSG
#undef NISAC_ASSERT_EQUAL
#undef NISAC_ASSERT_CLOSE
#undef NISAC_ASSERT_EQUAL_MSG
#undef NISAC_ASSERT_CLOSE_MSG
#undef NISAC_ASSERT_CHECKPOINT


#if defined(DISABLE_NISAC_ASSERTS)

#define NISAC_ASSERT(expr) ((void)0)
#define NISAC_ASSERT_EQUAL(l,r) ((void)0)
#define NISAC_ASSERT_CLOSE(l,r,t) ((void)0)
#define NISAC_ASSERT_EQUAL_MSG(l,r,m) ((void)0)
#define NISAC_ASSERT_CLOSE_MSG(l,r,t,m) ((void)0)
#define NISAC_ASSERT_CHECKPOINT(m) ((void)0)

#else

#include <cmath>
#include <boost/current_function.hpp>

#if defined(__PRETTY_FUNCTION__)
#define NISAC_ASSERT_MSG(expr,msg) ((expr) ? ((void)0) : \
::Log::Log_Assert(#expr, __PRETTY_FUNCTION__, __FILE__, __LINE__,msg))
#else
#define NISAC_ASSERT_MSG(expr,msg) ((expr) ? ((void)0) : \
::Log::Log_Assert(#expr,__func__, __FILE__, __LINE__,msg))
#endif // __PRETTY_FUNCTION__

#define NISAC_ASSERT(expr) NISAC_ASSERT_MSG(expr,"") 
#define NISAC_ASSERT_EQUAL(l,r) NISAC_ASSERT(l == r)
#define NISAC_ASSERT_CLOSE(l,r,tol) NISAC_ASSERT(abs(l-r) < tol)
#define NISAC_ASSERT_EQUAL_MSG(l,r,msg) NISAC_ASSERT_MSG(l == r,msg)

#define NISAC_ASSERT_CLOSE_MSG(l,r,tol,msg) \
NISAC_ASSERT_MSG(abs(l-r) < tol,msg)

#define NISAC_ASSERT_CHECKPOINT(msg) \
::Log::Log_Checkpoint(__PRETTY_FUNCTION__,__FILE__,__LINE__,msg)

#endif // DISABLE_NISAC_ASSERTS

//--------------------------------------------------------------------------
// SMART_ASSERT:  always active. Uses boost::smart_assert
//--------------------------------------------------------------------------

#undef SMART_CHECKPOINT

#if defined(BOOST_SMART_ASSERT_DEBUG)

#define SMART_CHECKPOINT(expr) \
if ( !(expr)) ; else \
boost::smart_assert::make_assert().test_expr\
( (expr), #expr).SMART_ASSERT_CONTEXT.context\
("checkpoint", 1).level(250).SMART_ASSERT_A \
    /**/

#else

#define SMART_CHECKPOINT(expr) \
if ( !(expr) ) ; else \
boost::smart_assert::make_assert().SMART_ASSERT_A \
    /**/

#endif // BOOST_SMART_ASSERT_DEBUG

//--------------------------------------------------------------------------
#endif  // NISAC_COMMON_ASSERT_H
//--------------------------------------------------------------------------
