//--------------------------------------------------------------------------
// $Id: AbortHandlerMPI.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    AbortHandlerMPI.h
// Module:  Global
// Author:  K. Bisset
// Created: September 14 2004
//
// Description: 
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_GLOBAL_ABORTHANDLERMPI
#define NISAC_GLOBAL_ABORTHANDLERMPI

#include "simx/Log/Logger.h"
#include "mpi.h"

//--------------------------------------------------------------------------

namespace Global {
  class AbortHandlerMPI : public Log::AbortHandler
  {
  public:
    ~AbortHandlerMPI();
    virtual void Cleanup();
  protected:
  private:
  };

  //--------------------------------------------------------------------------

  AbortHandlerMPI::~AbortHandlerMPI() {}

  //--------------------------------------------------------------------------

  void AbortHandlerMPI::Cleanup()
  {
    Log::log().error(0) << "Cleaning up MPI" << std::endl;
    MPI_Finalize();
  }

} // namespace

//--------------------------------------------------------------------------
#endif // NISAC_GLOBAL_ABORTHANDLERMPI
//--------------------------------------------------------------------------
