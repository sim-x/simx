//--------------------------------------------------------------------------
// $Id: OptionManager.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    OptionManager.C
// Module:  Global
// Author:  Randall Bosetti
// Created: June 07 2004
// Description:
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Global/OptionManager.h"
#include "simx/Global/util.h"
#include "simx/Common/Assert.h"
#include "simx/Log/Logger.h"

#include <stdio.h>
#include <ostream>
#include <iostream>



//--------------------------------------------------------------------------


namespace Global {

  //--------------------------------------------------------------------------
  // Option
  //--------------------------------------------------------------------------


  const std::string Option::empty = std::string("");

  Option::Option(std::vector<std::string> av): argc(av.size()),argv(av)
  {}

  Option::~Option()
  {}

  //--------------------------------------------------------------------------

  const std::string Option::GetName() const
  {
    std::string temp;
    temp = argv[0];
    std::string::size_type pos = temp.find_first_of("-");

    while (pos != temp.npos)
    {
      temp.erase(pos,1);
      pos = temp.find_first_of("-");
    }

    return(temp);
  }

  //--------------------------------------------------------------------------

  const std::string& Option::operator [](unsigned int anum) const
  {
    if (anum >= static_cast< unsigned int>( argc ))
    { 
      return(empty); 
    }
    else
    {
      return(argv[anum]); 
    }
  }

  //--------------------------------------------------------------------------

  void Option::Print( std::ostream& os ) const
  {
    for (int ii = 0 ; ii < argc ; ++ii)
    {
      os << "[" <<  argv[ii] << "]" << ' ';
    }
  }

  //--------------------------------------------------------------------------

  bool Option::operator == ( const Option& lhs ) const
  {
    return argc == lhs.argc && argv == lhs.argv; 
  }

  //--------------------------------------------------------------------------
  // OptionManager
  //--------------------------------------------------------------------------

  OptionManager::OptionManager()
  {}

  OptionManager::~OptionManager()
  {}

  //--------------------------------------------------------------------------

  const OptionManager::opvec_t& OptionManager::GetOptionsBeforeDDash() const
  { 
    return( beforeDDash );
  }

  //--------------------------------------------------------------------------

  const OptionManager::opvec_t& OptionManager::GetOptionsAfterDDash() const
  {
    return( afterDDash ); 
  }

  //--------------------------------------------------------------------------

  int OptionManager::GetNumOptionsBeforeDDash() const
  { return( beforeDDash.size() ); }

  //--------------------------------------------------------------------------

  int OptionManager::GetNumOptionsAfterDDash() const
  { 
    return( afterDDash.size() );
  }

  //--------------------------------------------------------------------------

  const Option& OptionManager::operator[]( unsigned int idx ) const
  {
      SMART_ASSERT(beforeDDash.empty() == false).msg
					( "beforeDDash empty" );

    if (idx >= beforeDDash.size())
    {
      return( beforeDDash[beforeDDash.size() - 1 ]);
    }
    else
    {
      return( beforeDDash[idx] );
    }
  }

  //--------------------------------------------------------------------------

  const Option& OptionManager::FreeOpt( unsigned int idx ) const
  {
     SMART_ASSERT(afterDDash.empty() == false).msg
				      ( "afterDDash empty" );

    if (idx >= afterDDash.size())
    {
      return( afterDDash[afterDDash.size()-1] );
    }
    else
    {
      return( afterDDash[idx] );
    }
  }

  //--------------------------------------------------------------------------

  void OptionManager::Print( std::ostream& os, const opvec_t& opvec ) const
  {
    for (unsigned int ii = 0 ; ii < opvec.size() ; ++ii)
    {
      os << "*" << opvec[ii] << "*" << ' ';
    }
  }

  //--------------------------------------------------------------------------

  void OptionManager::Print( std::ostream& os ) const
  {
    os << "before ddash: ";
    Print( os, beforeDDash );
    os << "\n";
    os << "after ddash: ";
    Print( os, afterDDash );
  }

  //--------------------------------------------------------------------------

  bool OptionManager::operator == ( const OptionManager& lhs ) const
  {
    return beforeDDash == lhs.beforeDDash && afterDDash == lhs.afterDDash;
  }

  //--------------------------------------------------------------------------
  //Separate args into the arrays "beforeDDash" and "afterDDash". If
  //PackOptions is called on args from a command line, the routine
  //assumes that argv[0] from the command line (the command name) was
  //removed by the caller and the original argc was reduced by one.

  bool OptionManager::PackOptions(int argc, char **argv) 
  {
    beforeDDash.clear();
    afterDDash.clear();

    if (argc < 1)
    {
      return(false);
    }

    // Put arguments that preceed the double dash into leftArgs and
    // args that follow it into afterDDash.  If there is no double dash,
    // all args go into leftArgs.

    bool saw_doubledash = false;
    svec_t leftArgs;

    for (int ii = 0 ; ii < argc ; ++ii)
    {
      svec_t tmp;
      const std::string argii( argv[ii] );
      tmp.push_back( std::string( argii ) );

      if (argii.size() > 1 && argii[0] == '-' && argii[1] == '-')
      { 
	saw_doubledash = true;
      }
      else if (saw_doubledash)
      {
	afterDDash.push_back( Option( tmp ) );
      }
      else
      {
	leftArgs.push_back( argii );
      }
    }

    // afterDDash is complete.

    // To construct beforeDDash, we have to process the contents of
    // leftArgs as follows.  We first put initial args with no leading
    // dash (if any) as separate options into beforeDDash. After that,
    // each arg with a leading dash is grouped with the following
    // no-dash args (up to the next dashed arg) into one Option and
    // put into beforeDDash.

    if (leftArgs.empty())
    {
      return true;
    }

    const std::string s0( *(leftArgs.begin()) );

    if (s0[0] == '-')
    {
      svec_t opts;
      MakeDashOptions( leftArgs, opts );
    }
    else
    {
      MakeNodashOptions( leftArgs );
    }

    return true;
  }

  //--------------------------------------------------------------------------
  //Build up groups of options of the form "-xx yy zz" and add them to
  //beforeDDash.  A group consists of an option with a leading dash
  //and the immediately following no-dash options; it runs up to (not
  //including) the next dashed option or the end of the args list.
 
  void OptionManager::MakeDashOptions( svec_t& args, svec_t& opts )
  {
    svec_iterator_t xx = args.begin();
    opts.push_back( *xx );

    if (xx == args.end() || xx + 1 == args.end())
    { 
      // Done with all args.
      beforeDDash.push_back( Option( opts ) );
      return; 
    }
    else
    {
      svec_iterator_t yy = xx + 1;
      svec_t rest( yy, args.end() );
      const std::string ss( *yy );

      if (ss[0] == '-')
      {
	// Current group is complete.  Put it into beforeDDash and
	// begin the next group.
	beforeDDash.push_back( Option( opts ) );
	opts.clear();
	MakeDashOptions( rest, opts );
      }
      else
      {
	// Continue to construct current group.
	MakeDashOptions( rest, opts );
      }
    }
  }

  //--------------------------------------------------------------------------
  // Add no-dash options one by one to beforeDDash.

  void OptionManager::MakeNodashOptions( svec_t& args )
  {
    svec_iterator_t xx = args.begin();
    svec_t tmp;

    // Put this option into beforeDDash.
    tmp.push_back( *xx );
    beforeDDash.push_back( Option( tmp ) );
      
    if (xx == args.end() || xx + 1 == args.end())
    { 
      // No more args.
      return; 
    }
    else
    {
      svec_iterator_t yy = xx + 1;
      svec_t rest( yy, args.end() );
      const std::string ss( *yy );

      if (ss[0] == '-')
      {
	// End of initial no-dash args; begin dashed options.
	svec_t opts;
	MakeDashOptions( rest, opts );
      }
      else
      {
	// Continue with no-dash args.
	MakeNodashOptions( rest );
      }
    }
  }

  //--------------------------------------------------------------------------

  // Global OptionManager
  OptionManager gOptMan;



} // end namespace Global

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
