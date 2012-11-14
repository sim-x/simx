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

//--------------------------------------------------------------------------
// $Id: util.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    util.C
// Module:  Global
// Author:  Randall Bosetti
// Created: June 07 2004
// Description:
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/Global/util.h"
#include "simx/Global/OptionManager.h"
#include "simx/Global/OptionMap.h"
#include "simx/Common/Values.h"
#include "simx/Common/Versions.h"
#include "simx/Config/Configuration.h"
#include "simx/Log/Logger.h"
//LK why? #include "config.h"

#include <signal.h>
#include <iostream>

//--------------------------------------------------------------------------

namespace {

  using namespace Global;
  using namespace Common;

  bool Version_Opt_f(const Option& opt)
  {
    write_versions();
    return(false);
  }

  //--------------------------------------------------------------------------

  bool Logfile_Opt_f(const Option &opt)
  {
    if (opt.GetArgc() < 2) {
      return(true);
    }

    //  Config::gConfig.SetConfigurationValue("LOG_FILE",opt[1]);
    return(false);
  }

  //--------------------------------------------------------------------------

  bool Rank_Opt_f(const Option &opt)
  {
    if (opt.GetArgc() < 2) {
      return(true);
    }

    Values::SetRank( atoi(opt[1].c_str()) );
    return(false);
  }

  //--------------------------------------------------------------------------

  bool DebugLevel_Opt_f(const Option &opt)
  {
    if (opt.GetArgc() < 2) {
      return(true);
    }
  
    //  Config::gConfig.SetConfigurationValue("LOG_LEVEL",opt[1]);
    return(false);
  }

  //--------------------------------------------------------------------------

  bool CoutLevel_Opt_f(const Option &opt)
  {
    if (opt.GetArgc() < 2) {
      return(true);
    }

    //  Config::gConfig.SetConfigurationValue("LOG_COUT_LEVEL",opt[1]);
    return(false);
  }

  //--------------------------------------------------------------------------

  static bool versionf =
  OptionMap::Instance().Register("version", Version_Opt_f);

  static bool logfilef = 
  OptionMap::Instance().Register("logfile", Logfile_Opt_f);

  static bool debuglevelf = 
  OptionMap::Instance().Register("debuglevel", DebugLevel_Opt_f);

  static bool coutlevelf = 
  OptionMap::Instance().Register("coutlevel", CoutLevel_Opt_f);

  //--------------------------------------------------------------------------

  static void CatchSignal(int num) 
  {
    struct sig_struct { int value; char *name; };
    const struct sig_struct sig_array[] = {
      { 1, "SIGHUP" },
      { 2, "SIGINT" },
      { 3, "SIGQUIT" },
      { 4, "SIGILL" },
      { 6, "SIGABRT" },
      { 8, "SIGFPE"},
      { 9, "SIGKILL" }, // uncatchable
      { 11, "SIGSEGV" },
      { 13, "SIGPIPE" },
      { 14, "SIGALRM" },
      { 15, "SIGTERM" },
      { 10, "SIGUSR1" },
      { 12, "SIGUSR2" },
      { 17, "SIGCHLD" },
      { 18, "SIGCONT" },
      { 19, "SIGSTOP" },  // uncatchable
      { 20, "SIGSTP" },
      { 21, "SIGTTIN" },
      { 22, "SIGTTOU" },
      { 7, "SIGBUS" },
      { 5, "SIGTRAP" },
      { 23, "SIGURG" },
      { 26, "SIGVTALRM" },
      { 24, "SIGXCPU" },
      { 25, "SIGXFSZ" },
    };

    const int arr_size = sizeof(sig_array)/sizeof(sig_array[0]);
    static volatile int caught_sig = 0;

    if (caught_sig) {
      exit(num);
    }

    caught_sig = 1;
    char msg[250];
    int ii;

    for (ii = 0; ii < arr_size; ++ii) {
      if (sig_array[ii].value == num) {
	sprintf(msg,"Caught signal %d: %s; Aborting.",
		num,sig_array[ii].name);
	break;
      }
    }

    if (ii == arr_size) {
      sprintf(msg,"Caught signal %d; Aborting.",num);
    }

    Log::log().info(0) << msg << std::endl;
    Log::log().Failure(0,msg);
  }


  //--------------------------------------------------------------------------

  int util_afterdashes() 
  {

    int ac = gOptMan.GetNumOptionsAfterDDash();
    std::ios_base::sync_with_stdio(false);

    // Backwards compatible behaviour
    if (ac == 0)
    {
      int bc = gOptMan.GetNumOptionsBeforeDDash();
      if (bc > 1) 
        Values::SetRank
	  ( atoi(gOptMan[1].GetName().c_str()) );

      if (bc > 2) {
        Values::SetRankExtLength
	  ( atoi(gOptMan[2].GetName().c_str()) );
        if (Values::gRankExtLength() < 1) 
          Values::SetRankExtLength( 2 );
      }
    }

    // Process and store rank number, if supplied.
    if (ac > 1) {
      //std::cout << "Trying to store rank number" << std::endl;
      Values::SetRank
	( atoi(gOptMan.FreeOpt(2).GetName().c_str()) );
    }

    if (ac > 2) {
      //std::cout << "Trying to store rank extension length" << std::endl;
      Values::SetRankExtLength
	( atoi(gOptMan.FreeOpt(3).GetName().c_str()) );
      if (Values::gRankExtLength() < 1) {
	Values::SetRankExtLength( 2 );
      }
    }
    return(0);
  }

} // anonymous namespace

//--------------------------------------------------------------------------

namespace Global
{
  std::string DecToAbc(int num) {

    assert( num > -1 );
     
    int mod = 1;
    int j;
    std::string str = "";
    int length = Values::gRankExtLength();
    //    length = 2;
       
    // We've overflowed length
    if (num/26 > length)
    {
      int v=num;
      length=0;
      while (v > 0)
      {
	v /= 26;
	length++;
      }
    }
 
    for (int i = length-1; i >= 0; i--) {
      for (j = i; j > 0; j--) {
	mod *= 26;
      }
      str += ('A'+num/mod);
      num %= mod;
      mod = 1;
    }
    return(str);
  }


  //--------------------------------------------------------------------------

  std::string& GetBaseName( std::string& name ) 
  {
    if (name == "")
    { return name; }

    // remove terminating '/'s
    while (name[name.size() - 1] == '/')
    {
      name = name.substr( 0, name.size() - 1 );
    }
    if (name.size() == 0)
    {
      name = "/";
      return name;
    }

    // remove everything before last '/'
    std::string::size_type ii = name.find_last_of("/");
    if (ii != std::string::npos)
    {
      name.erase(0,ii);
    }

    // remove remaining '/' if present
    if (name[0] == '/' && name.size() > 1)
    {
      name.erase( 0, 1 );
    }

    return name;
  }

  //--------------------------------------------------------------------------

  void InstallSignalHandler(const int which[15])
  {
    for (int i = 1; i < 15; i++) {
      if (which[i] != 0 && which[i] != SIGABRT) {
	signal(which[i],CatchSignal);
      }
    }
  }

  //--------------------------------------------------------------------------

  /// util_main() handles all global options and some of the init
  /// stuff that used to be in main().  In order to handle a new
  /// global option, you must add an entry to the options[] array,
  /// _before_ the { NULL,NULL } entry.  You must also provide a
  /// function with signature bool func(Option &opt) which returns
  /// false on success and true otherwise.q

  int util_main(int argc, char **argv)
  {
    std::string name( argv[0] );
    Values::SetProgName( GetBaseName( name ) );

    gOptMan.PackOptions( argc-1, argv+1 );

    int rac = gOptMan.GetNumOptionsBeforeDDash();
    OptionHandler oh;

    for (int i = 0, j = 0; i < rac; i++, j = 0) {
      oh = OptionMap::Instance().GetHandler(gOptMan[i].GetName());
      if (oh) {
	oh(gOptMan[i]);
      }
    }

    // add the post '--' functions here
    if (util_afterdashes()) {
      exit(EXIT_FAILURE);
    }

    // Set the suffix based on the rank, if supplied.
    if (Values::gRank() != -1) {
      std::string sufx( Values::gRankSuffix() );
      sufx += "."  + DecToAbc( Values::gRank() );
      Values::SetRankSuffix( sufx ); 
    }
    else {
      Values::SetRankSuffix( "" );
    }

    return(0);
  }
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
