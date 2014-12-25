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
// $Id$
//--------------------------------------------------------------------------
//
// File:        pycontrol.C
// Module:      Python
// Author:      Sunil Thulasidasan
// Created:     July 9 2012 
// Description: control functions for python. 
//
// @@
//
//--------------------------------------------------------------------------

#include <boost/python.hpp>
#include "simx/ServiceManager.h"
#include "simx/EntityManager.h"
#include "simx/Python/PyEntityData.h"
#include "simx/control.h"
#include "simx/LP.h"
#include "simx/type.h"
#include "simx/constants.h"

using namespace std;
using namespace boost;

namespace simx {
  
  namespace Python {
    
    void preparePyService( const PyServiceData& serv_data ) {
#ifdef DEBUG
      Logger::debug3() << "PyControl: preparing python service" << endl;
#endif
      theServiceManager().preparePyService( serv_data );
    }
    
    
    void preparePyServices( const python::list& s_list ) {
      for (int i = 0; i < python::len(s_list); ++i )
	preparePyService( python::extract<PyServiceData&>(s_list[i]) );

    }
    
    void createController() {
      theEntityManager().createController();
    }

    simx::Time getMinDelay() {
      return LP::MINDELAY;
    }

    simx::Time getLocalMinDelay() {
      return LOCAL_MINDELAY;
    }


    simx::Time getNow() {
      static const Control::LpPtrMap& lpMap = Control::getLpPtrMap();
      static const LP& lp = *(lpMap.begin()->second); // get address of ANY lp on this computer node
      return lp.getNow();
    }

  }
  
}
using namespace boost::python;
using namespace simx::Control;

void export_pycontrol() {
  
  python::def("prepare_services",&simx::Python::preparePyServices,"Pre-initialization of all services in the given list of services");
  python::def("prepare_service",&simx::Python::preparePyService,"Pre-initialization of service");
  //python::def("create_controller",&simx::Python::createController);

  //export simx::Control enums
  enum_<eSimPhase>("SimPhase")
    .value("PHASE_INIT", kPhaseInit)
    .value("PHASE_RUN", kPhaseRun)
    .value("PHASE_WRAPUP", kPhaseWrapUp)
    ;  
  
  enum_<simx::ServiceAddress>("ServiceAddress")
    ;  

  // export time functions
  def("get_min_delay",&simx::Python::getMinDelay, "Returns the value of look-ahead in a parallel simulatoin");
  def("get_local_min_delay",&simx::Python::getLocalMinDelay,"Returns the minimum delay with which events can be schedules on the local processor");
  def("get_now",&simx::Python::getNow,"Returns the value of the simulation clock on this logical process");
  // export simx::Control functions
  def("get_num_machines",&getNumMachines,"Returns the number of MPI processes in a simulation" );
  def("get_rank",&getRank,"Returns the MPI rank of this process" );
  def("get_num_lps",&getNumLPs,"Returns the number of logical processes. This is usually the same as the number of MPI processes." );
  def("get_sim_phase",&getSimPhase,"Returns the current simulation phase. Will be one of PHASE_INIT, PHASE_RUN or PHASE_WRAPUP" );
  def("get_processor_name",&getProcessorName,"Returns the processor name on which the calling process is running",
      return_value_policy<copy_const_reference>());
}
