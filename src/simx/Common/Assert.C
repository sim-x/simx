
//--------------------------------------------------------------------------
// $Id: Assert.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    assert.C
// Module:  Common
// Author:  K. Bisset
// Created: November  2 2000
// Description:
//        Function wrapper called from our assert macro (Common/assert.h) to
//        calls the Logger::Assert() method for appropriate program exit.
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Common/Assert.h"
#include "simx/Log/Logger.h"
#include "simx/smart_assert/smart_assert_ext.hpp"

//--------------------------------------------------------------------------

using namespace std;

//--------------------------------------------------------------------------

namespace Log
{
  void cppAssert(const char* ex, const char* file, int line)
  {
    Log::log().Assert(file, ex, line);
  }
}

//--------------------------------------------------------------------------

// Initialize boost smartassert system

namespace {

  int gMod = Log::log().registerModule("Assert");

  //--------------------------------------------------------------------------

  void handle_default
  ( const boost::smart_assert::assert_context & context, ostream* out)
  {
    (*out) 
      << "Assertion failed: " << context.get_level_msg()
      << "\n"
      << "File "
      << context.get_context_file()
      << ':'
      << context.get_context_line()
      << "]:\n";

    // if we have extra information about the assertion,
    // like, a __PRETTY_FUNCTION__ or something
    if ( context.get_context_vals_collection().size() > 2) {
      // extra information
      std::for_each
	(context.get_context_vals_collection().begin(), 
	 context.get_context_vals_collection().end(),
	 boost::smart_assert::Private::log_context_val( *out) );
    }

    // log expression
    if (context.get_level() != 250)
      (*out) << "Expression = '" << context.get_expr() << "'\n";

    // log values
    (*out) << "Values:\n";
    std::for_each
      (context.get_vals_array().begin(), context.get_vals_array().end(),
       boost::smart_assert::Private::log_val( *out) );

    int module = (context.get_module()==0) ? gMod : context.get_module();

    Log::log().Failure(module, "Assertion Failed");
  }

  //--------------------------------------------------------------------------

  void handle_checkpoint
  ( const boost::smart_assert::assert_context & context, ostream* out)
  {
    (*out)
      << "Checkpoint: " 
      << context.get_level_msg()
      << "\n"
      << "File "
      << context.get_context_file()
      << ':'
      << context.get_context_line()
      << "]:\n";

    // if we have extra information about the assertion,
    // like, a __PRETTY_FUNCTION__ or something
    if ( context.get_context_vals_collection().size() > 2) {
      // extra information
      std::for_each
	(context.get_context_vals_collection().begin(), 
	 context.get_context_vals_collection().end(),
	 boost::smart_assert::Private::log_context_val( *out) );
    }

    // log expression
    if (context.get_level() != 250)
      (*out) << "Expression = '" << context.get_expr() << "'\n";

    // log values
    (*out) << "Values:\n";
    std::for_each
      (context.get_vals_array().begin(), context.get_vals_array().end(),
       boost::smart_assert::Private::log_val( *out) );
  }

  //--------------------------------------------------------------------------

  void handle_assert( const boost::smart_assert::assert_context & context)
  {
    ostream* out;
    string type;
    int mod = (context.get_module()==0) ? gMod : context.get_module();
    int level = context.get_level(); 

    switch (level)
    {
    case boost::smart_assert::lvl_debug:
      out = &Log::log().debug1(mod);
      break;

    case boost::smart_assert::lvl_info:
      out = &Log::log().info(mod);
      break;

    case boost::smart_assert::lvl_warn:
      out = &Log::log().warn(mod);
      break;

    case boost::smart_assert::lvl_error:
    case boost::smart_assert::lvl_fatal:
    default:
      out = &Log::log().error(mod);
      break;
    }

    if (context.get_context_val("checkpoint") == "1")
      handle_checkpoint(context, out);
    else
      handle_default(context, out);
    (*out) << flush;
  }

  //--------------------------------------------------------------------------

  bool init()
  {

    boost::smart_assert::assert_settings().set_level_handler
      (boost::smart_assert::lvl_debug, &handle_assert);

    boost::smart_assert::assert_settings().set_level_handler
      (boost::smart_assert::lvl_info, &handle_assert);

    boost::smart_assert::assert_settings().set_level_handler
      (boost::smart_assert::lvl_warn, &handle_assert);
  
    boost::smart_assert::assert_settings().set_level_handler
      (boost::smart_assert::lvl_error, &handle_assert);

    boost::smart_assert::assert_settings().set_level_handler
      (boost::smart_assert::lvl_fatal, &handle_assert);
  
    return true;
  }

  //--------------------------------------------------------------------------

  static bool ret = init();

} // namespace

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
