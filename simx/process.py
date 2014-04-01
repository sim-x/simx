# Copyright (c) 2013, 2014 Los Alamos National Security, LLC. 

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

import processmgr as pm

class Process(object):
    """
    Base class for all processes in simx.
    Provides functionality for process oriented simulation
    *Needs more description*
    """

    # def __init__(self,pid):
    #     self.pid = pid


    def waitfor(self, process):
        """
        Suspends process till the given process finishes
        executing. 
        """
        pm.get_process_mgr().proc_waitfor(self, process)


    def waiton(self, resource):
        """
        Waits on a resource.
        """
        pm.get_process_mgr().proc_waiton(self, resource)
    
    
    
    def spawn(self, process):
        """
        Spawns a process and continues executing
        """
        pm.get_process_mgr().proc_schedule(process, parent=self)


    def sleep(self, duration):
        """
        Suspends for specified amount of time. Duration has to be atleast
        one second
        """
        pm.get_process_mgr().proc_sleep(self, max(1,duration))


    def run(self): 
        """
        All classes that inherit from Process class should
        define this method. This is the method that gets
        run when the process is activated. This method should
        _never_ be directly called by the user.
        """
        pass

    
    def end_(self):
        """
        Called by simx process manager when a process is terminated. Should
        never be directly called by the user
        """
        self.end()
    

    
    def end(self):
        """
        Called by SimX process manager when a process is terminated. Should _never_
        be directly called by the user. Can't use Python destructor __del__
        method for this, since __del__ might not be called when a process is deleted
        (due to Python's reference counting mechanism)
        """
        pass
    
    
    def kill_all(self, process):
        """
        Kills process and all its sub processes (process tree).
        """
        pm.get_process_mgr().proc_kill_all( process )
        
        
    def kill(self, process):
        """
        kills process.
        """
        pm.get_process_mgr().proc_kill( process )
