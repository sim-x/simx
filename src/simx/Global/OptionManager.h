// Copyright (c) 2012. Los Alamos National Security, LLC. 

// This material was produced under U.S. Government contract DE-AC52-06NA25396
// for Los Alamos National Laboratory (LANL), which is operated by Los Alamos 
// National Security, LLC for the U.S. Department of Energy. The U.S. Government 
// has rights to use, reproduce, and distribute this software.  

// NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, 
// EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  
// If software is modified to produce derivative works, such modified software should
// be clearly marked, so as not to confuse it with the version available from LANL.

// Additionally, this library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License v 2.1 as published by the 
// Free Software Foundation. Accordingly, this library is distributed in the hope that 
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See LICENSE.txt for more details.

//-*-C++-*------------------------------------------------------------------
// $Id: OptionManager.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    OptionManager.h
// Module:  Global
// Author:  Randall Bosetti
// Created: June 07 2004
// Description:
//
// @@
//
//--------------------------------------------------------------------------

#ifndef _GLOBAL_OPTION_MANAGER_H
#define _GLOBAL_OPTION_MANAGER_H

#include <vector>
#include <iosfwd>

//--------------------------------------------------------------------------

namespace Global {
  
  class Option
  {
  public:

    Option(std::vector<std::string> argv);
    ~Option();

    /// Returns argv[0] without leading dashes.
    const std::string GetName() const;

    // If anum < argc, returns argv[anum]; otherwise returns empty.
    const std::string& operator [](unsigned int anum) const;
    
    int GetArgc() const { return(argc); }
    void Print( std::ostream& os ) const;

    // for testing
    bool operator == ( const Option& lhs ) const;

  private:

    int                      argc;
    std::vector<std::string> argv;
    static const std::string empty;
  };

  typedef bool (*OptionHandler)(const Option& opt);

  //--------------------------------------------------------------------------
  // OptionManager
  //--------------------------------------------------------------------------

  // OptionManager arranges the string of arguments that is supplied
  // to PackOptions.  This string is assumed to have the form "[A] [B]
  // -- [C]".  None of these elements is required.  [A] is a list of
  // arguments, none of which begins with a dash.  [B] is a list of
  // arguments whose first element begins with a dash; subsequent
  // elements can begin with or without a dash.  The double dash "--"
  // is a marker that is not itself an argument; it signals that all
  // following arguments (in [C]) are placed without further
  // processing into the container 'afterDDash'.

  // OptionManager stores each element of [A] as a single stand-alone
  // option in beforeDDash.  OptionManager treats elements of [B] in
  // groups.  A group consists of an argument that begins with a dash
  // (or a double dash) and all following arguments up to the next
  // argument that begins with a dash (or the end of [B]).  Each group
  // is put into beforeDDash as a single option.

  // OptionManager's arrangement of options is modeled on Perl's
  // option processing.

  class OptionManager
  {
  public:

    typedef std::vector< std::string> svec_t;
    typedef svec_t::iterator          svec_iterator_t;
    typedef std::vector< Option >     opvec_t;
    typedef opvec_t::iterator         opvec_iterator_t;

    OptionManager();
    ~OptionManager();

    const opvec_t& GetOptionsBeforeDDash() const;
    const opvec_t& GetOptionsAfterDDash() const;

    int GetNumOptionsBeforeDDash() const;
    int GetNumOptionsAfterDDash() const;

    // If idx < beforeDDash.size(), returns beforeDDash[idx];
    // otherwise returns last element of beforeDDash.
    const Option& operator [](unsigned int idx) const;

    // If idx < afterDDash.size(), returns afterDDash[idx]; otherwise
    // returns last element of afterDDash.
    const Option& FreeOpt(unsigned int idx) const;

    void Print( std::ostream& ) const;

    // for testing
    bool operator == ( const OptionManager& lhs ) const;

    // PackOptions builds up members beforeDDash and afterDDash as
    // follows. All args that follow the double dash in the argument
    // list go as separate options into afterDDash.  Args that preceed
    // the double dash are processed in two steps. If the argument
    // string begins with arguments that lack leading dashes, those
    // arguments are put as separate options into beforeDDash. Upon
    // encountering an arg that begins with a dash, PackOptions groups
    // that arg with all immediately following no-dash args and adds
    // the group as a single option to beforeDDash.  PackOptions
    // terminates a group when it finds the next dashed arg or runs
    // out of before-double-dash arguments.  If no double dash is
    // present, all args, processed as described above, go into
    // beforeDDash.

    // PackOptions processes its arguments as passed in; it assumes
    // that the command-line argv[0] (the command name) has been
    // removed and the command-line argc reduced by one.  PackOptions
    // returns true if argc >= 1, false otherwise.

    bool PackOptions(int argc, char **argv);

  private:
    
    void MakeDashOptions( svec_t& args, svec_t& opts );
    void MakeNodashOptions( svec_t& args );
    void Print( std::ostream& os, const opvec_t& opvec ) const;
    
    opvec_t beforeDDash;
    opvec_t afterDDash;
  };
   
  //--------------------------------------------------------------------------

  /// the global option manager
  extern OptionManager gOptMan;

} // namespace Global

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

inline std::ostream& operator << 
  ( std::ostream& os, const Global::Option& op )
{
  op.Print( os );
  return os;
}

//--------------------------------------------------------------------------

inline std::ostream& operator << 
  ( std::ostream& os, const Global::OptionManager& opman )
{
  opman.Print( os );
  return os;
}

//--------------------------------------------------------------------------
#endif // _GLOBAL_OPTION_MANAGER_H
//--------------------------------------------------------------------------
