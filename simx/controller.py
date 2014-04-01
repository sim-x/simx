# Copyright (c) 2013 Los Alamos National Security, LLC. 

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

import core
import core.DebugStream as ds
import processmgr as pm


class Controller(core.PyEntity):

    """

    A special kind of entity, only one per process, that has
    an id of the form ('!', n) where n is the rank of the process.
    The purpose of the controller is to house any and all services that
    are global in nature. Thus any service that should not be tied to
    a particular entity should live on the controller entity.

    """
    
    def __init__(self,ID,lp,entity_input,py_obj=None):
        if py_obj is None:
            py_obj = self
        super(Controller,self).__init__(ID,lp,entity_input,py_obj)
        ds.debug2.write("Controller",self.get_id(),"is being created on rank: ", 
                     core.get_rank())
        # all other controllers are its neighbors
        nm = core.get_num_machines()
        self.neighbors_ = zip(['!']*nm,range(nm))
        # remove own id from neighbor list
        self.neighbors_.remove(self.get_id())
    
        #install process manager service
        self.install_service(pm.ProcessManager, pm.eAddr_ProcessManager)
        self.pmgr_ = self.get_service(pm.eAddr_ProcessManager)


    def get_process_mgr(self):
        """
        Returns handle of process manager service
        """
        return self.pmgr_


    # def __str__(self):
    #     return "Controller(%s)" %(self.get_id())



def get_controller():
    """
    
    A helper function that returns the handle of the
    controller object for this process

    """

    return core.get_entity(('!',core.get_rank()))
