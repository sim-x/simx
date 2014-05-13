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

class Process(simx.Process):
    """
    Class for modeling a computer process
    """

    def __init__(self, os, req_resource=None ):
        """
        sets handler to os.system service
        and processor affinity to None
        """
        self.os = os
        self.req_res = req_resource
        self.resource = None #no allocation to resource has been made yet
        self.res_handler = os #resource handler


    def compute(self, time):
        self.sleep(time)


    def waitfor(self, process):
        """
        Suspends process till the given process finishes 
        executing
        """
        # calls os waitfor
        self.os.waitfor_process(self, process)


    def spawn(self, process_class, *args, **kwargs):
        """
        spawns a process and continues executing
        """
        p = self.os.create_process(process_class, *args, **kwargs)
        self.os.schedule_process(p)
        return p
    
    # def kill_all(self, process):
    #     """
    #     kills process and all of its sub processes (process tree)
    #     """
    #     pass


    # def kill(self, process):
    #     """
    #     kills process
    #     """
    #     pass
