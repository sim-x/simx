# Copyright (c) 2014. Los Alamos National Security, LLC. 

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

import simx
#import simx.os as os
import system
# define service addresses
from processor import Processor

class Node(simx.PyEntity):
    """
    Class that represents a compute node. A compute node might
    consist of a number of other entities, such as processors, I/O devices,
    network cards and so on.
    """
    def __init__(self, id_, lp, entity_input, py_obj=None):
        if py_obj is None:
            py_obj = self
        super(Node,self).__init__(id_, lp, entity_input, self)
        # install os.system service
        self.install_service(system.System, system.ADDR_SYSTEM)
        # set handler to system service
        self.os = self.get_service(system.ADDR_SYSTEM)
        self.cores = []
        self.attr = entity_input.data_
        self.num_cores = self.attr['num_cores']
        for i in range(self.num_cores):
             self.cores.append(Processor())
            

    def get_os(self):
        """
        Returns handle of OS service
        """
        return self.os
