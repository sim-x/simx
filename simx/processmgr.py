# Copyright (c) 2012. Los Alamos National Security, LLC. 

# This material was produced under U.S. Government contract DE-AC52-06NA25396
# for Los Alamos National Laboratory (LANL), which is operated by Los Alamos 
# National Security, LLC for the U.S. Department of Energy. The U.S. Government 
# has rights to use, reproduce, and distribute this software.  

# NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, 
# EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  
# If software is modified to produce derivative works, such modified software should
# be clearly marked, so as not to confuse it with the version available from LANL.

# Additionally, this library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License v 2.1 as published by the 
# Free Software Foundation. Accordingly, this library is distributed in the hope that 
# it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See LICENSE.txt for more details.

from simx import core
from DebugStream import *
import controller


eAddr_ProcessManager = 1000

class ProcessManager(core.PyService):

    """
    The process manager is the work-horse for proces oriented simulation
    in SimX. All process switching, scheduling and management goes via
    the process manager. The process manager is a service that lives on the
    controller entity
    """

    def __init__(self, name, entity, service_input ):
        super(ProcessManager,self).__init__( name,  
                                             entity, service_input, self )
        debug1.write("Process Manager ", name, "being created on entity",
                          entity)
        


    # def recv(self, msg):
    #     self.recv_function[msg.__class__.__name__](msg)

    
    




core.register_service(ProcessManager)
core.register_address("eAddr_ProcessManager",eAddr_ProcessManager)


def get_process_mgr():
    """
    A helper function that returns the handle of the
    process manager service
    """
    return get_controller().get_process_mgr()

