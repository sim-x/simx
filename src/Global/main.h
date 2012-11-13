//--------------------------------------------------------------------------
// $Id: main.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    main.h
// Module:  Global
// Author:  Keith Bisset
// Created: July 10 2002
// Description: 
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef MODULE_GLOBAL_MAIN
#define MODULE_GLOBAL_MAIN

//--------------------------------------------------------------------------

/// \file main.h ModuleMain functions that must be available to all
/// main routines.

//--------------------------------------------------------------------------

namespace Global {

  /// Normal modules should use ModuleMain(). main() should call this
  /// function inside a try-catch block, after processing the
  /// configuration file and other setup functions.
  void ModuleMain();


  /// ModuleMain( int, char** ) should be used only by utility progrms
  /// that do not take a config file as an argument.
  void ModuleMain(int argc, char** argv);

  
  void initEnv();
  void startSimulation();

}

//--------------------------------------------------------------------------
#endif // MODULE_GLOBAL_MAIN
//--------------------------------------------------------------------------
