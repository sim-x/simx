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
      Logger::info() << "PyControl: preparing python service" << endl;
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
  
  python::def("prepare_services",&simx::Python::preparePyServices);
  python::def("prepare_service",&simx::Python::preparePyService);
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
  def("get_min_delay",&simx::Python::getMinDelay);
  def("get_local_min_delay",&simx::Python::getLocalMinDelay);
  def("get_now",&simx::Python::getNow);
  // export simx::Control functions
  def("get_num_machines",&getNumMachines );
  def("get_rank",&getRank );
  def("get_num_lps",&getNumLPs );
  def("get_sim_phase",&getSimPhase );
  def("get_processor_name",&getProcessorName,
      return_value_policy<copy_const_reference>());
}
