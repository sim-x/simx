//--------------------------------------------------------------------------
// $Id: backtrace.C,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    backtrace.C
// Module:  Common
// Author:  Keith Bisset
// Created: July 23 2002
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/Common/backtrace.h"
#include "config.h"
#include <iostream>


#ifndef __APPLE__
#include <execinfo.h>
#endif

#ifdef HAVE_CXA_DEMANGLE
#include <cxxabi.h>
#endif

using namespace std;

//--------------------------------------------------------------------------

namespace Common {

  std::string Backtrace()
  {

#ifndef __APPLE__

    void* symbols[128];
    char** syms;

    int size =  backtrace (symbols, 128);
    syms = backtrace_symbols (symbols, size);

    string bt;
    // Start at 1, since 0 is this function
    for (int i=1; i < size; i++)
    {
      // symbol is of the form
      // file(function+offset) [addr]
    
      string sym = syms[i];
    
      long pos_op = sym.find('('); // open paren
      long pos_plus = sym.find('+'); // plus
      long pos_cp = sym.find(')'); // close paren
      long pos_ob = sym.find('['); // open bracket
      long pos_cb = sym.find(']'); // close bracket

      string file = sym.substr(0, pos_op);
      string name = sym.substr(pos_op+1, pos_plus-pos_op-1);
      string offset = sym.substr(pos_plus+1, pos_cp-pos_plus-1);
      string addr = sym.substr(pos_ob+1, pos_cb-pos_ob-1);

      string dname;
      if (name[0] == '_' && name[1] == 'Z')
      {
        dname = demangle(name.c_str());
      }
      else
        dname = name;
      bt += '\t';
      bt += file + "\t" + dname + "\n";
    }
    return bt;

#endif //__APPLE__

  }

  //--------------------------------------------------------------------------

#ifdef HAVE_CXA_DEMANGLE

  const char* demangle(const char* name)
  {
    char buf[1024];
    unsigned int size=1024;
    int status;
    char* res = abi::__cxa_demangle 
		(name,
		 buf,
		 &size,
		 &status);
    return res;
  }

  //--------------------------------------------------------------------------

  string demangle(const string& name)
  {
    char buf[1024];
    unsigned int size=1024;
    int status;
    string res = abi::__cxa_demangle 
		 (name.c_str(),
		  buf,
		  &size,
		  &status);
    return res;
  }

  //--------------------------------------------------------------------------

#else

  const char* demangle(const char* name)
  {
    return name;
  }

//--------------------------------------------------------------------------

  string demangle(const string& name)
  {
    return name;
  }

  //--------------------------------------------------------------------------

#endif  

} // namespace

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
