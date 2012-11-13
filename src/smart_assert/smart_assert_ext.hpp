// smart_assert_ext.h

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

#ifndef BOOST_SMART_ASSERT_EXT_HPP_INCLUDED
#define BOOST_SMART_ASSERT_EXT_HPP_INCLUDED

#if _MSC_VER >= 1020
#pragma once
#endif



#include <simx/smart_assert/smart_assert.hpp>

#include <simx/smart_assert/smart_assert/priv/fwd/before_includes.hpp>


#ifdef BOOST_SMART_ASSERT_WIN32
// Win32 specific
#define WIN32_LEAN_AND_MEAN
#define BOOST_WIN32_LEAN_AND_MEAN_DEFINED
#endif // BOOST_SMART_ASSERT_WIN32


// persistence
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_persist.hpp>

// extra (cool) handlers !!!
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_h_ext.hpp>

// extra (cool) loggers !!!
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_l_ext.hpp>


// Configurations
#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_cfgarray.hpp>

#include <simx/smart_assert/smart_assert/priv/fwd/smart_assert_init.hpp>


// should we include implementation?
#ifndef BOOST_SMART_ASSERT_DONOT_INCLUDE_IMPL

#include <simx/smart_assert/smart_assert/priv/impl/smart_assert.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_cfgarray.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_context.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_ctxfunc.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_h_basic.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_h_ext.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_h_win32.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_init.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_l_basic.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_l_ext.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_persist.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_settings.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_util.hpp>
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_keeper.hpp>


#ifndef BOOST_SMART_ASSERT_OVERRIDE_DEFAULTS
#include <simx/smart_assert/smart_assert/priv/impl/smart_assert_default.hpp>
#endif

#endif


#ifdef BOOST_WIN32_LEAN_AND_MEAN_DEFINED
    #undef BOOST_WIN32_LEAN_AND_MEAN_DEFINED
    #undef WIN32_LEAN_AND_MEAN
#endif

#include <simx/smart_assert/smart_assert/priv/fwd/after_includes.hpp>

#endif
