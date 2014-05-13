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

#import simx

#import simx
#import simx.DebugStream as ds

import simx
import simx.core as core
import simx.core.DebugStream as ds
from simx.processmgr import ProcStatus
from process import Process
import simx.processmgr


class System(core.PyService):
    """
    Mimics a simple computer operating system. Lives on a Node object.
    """
    def __init__(self, name, node, service_input):
        super(System, self).__init__(name, node, service_input, self)
        ds.debug1.write("OS.System ", name, "being created on entity",node.get_id())
        self.node = node
        self.pm = simx.processmgr.get_process_mgr()
        self.ready_processes = [] #processes waiting for CPU time
        
    def create_process(self, process_class, *args, **kwargs):
        """
        creates a process of process_class with given arguments
        """
        # this way of creating the class does not call
        # the class's init method
        new_proc = process_class.__new__(process_class)
        core.util.check_type(Process, new_proc)
        #initialize os.Process members by invoking parent class's init()
        super(process_class,new_proc).__init__(self, req_resource=None)
        # then call the class's init()
        new_proc.__init__(*args, **kwargs)
        #self.schedule_process(new_proc)
        return new_proc


    def create_process_on_resource(self, req_resource, process_class, *args, **kwargs):
        """
        Creates a process class with given arguments and requests a specific resource
        """  
        
        # this way of creating the class does not call
        # the class's init method
        new_proc = process_class.__new__(process_class)
        core.util.check_type(Process, new_proc)
        #initialize os.Process members by invoking parent class's init()
        super(process_class,new_proc).__init__(self, req_resource)
        # then call the class's init()
        new_proc.__init__(*args, **kwargs)
        #self.schedule_process(new_proc)
        return new_proc
        


    def schedule_process(self, process, delay = core.get_local_min_delay() ):
        """
        """
        self.pm.proc_schedule(process)


    def assign_process(self, proc_info):
        """
        If process already has an assigned resource, simply switch to it.
        Otherwise, assigns process to a processor resource, unless
        the process might have requested to be 
        explicitly assigned to a process in which case assign to that resource
        (since program knows best); else assign to a global queue
        """
        process = proc_info.object_

        if not process.resource is None:
            proc_info.gobject_.switch()
            #print "proc info memory location",proc_info
            return

        # else need to find a resource and assign to it.
        core = None
        if process.req_res is None:
            #core = min(self.node.cores, key = lambda core : len(core.process_queue)) 
            if self.ready_processes:  #non-empty process queue
                #print "appending process ",id(process),"to non-empty main queue"
                self.ready_processes.append(process)
                #print self.ready_processes

            else: # empty process queue
                #check if an idle resource exists, and assign it to that one
                idle_res = filter(lambda core : core.busy == False, self.node.cores)
                if idle_res:
                    core = idle_res[0] 
                else: #append to main process queue
                    #print "appending process ",id(process),"to empty main queue, all resources busy"
                    self.ready_processes.append(process)
                    #print self.ready_processes

        else: # process had explicity requested a resource
            if  process.req_res.busy == True: # requested resource is busy
                process.req_res.process_queue.append(process)
            else: # requested resource is available
                core = process.req_res
                
        if not core is None: # core has been assigned
            assert(core.busy == False)
            # it is possible that an idle core might have a non-empty private queue
            # due to some pathological sequence of events. so check private queue
            if core.process_queue: #non empty private queue
                assert(False)
                core.process_queue.append(process)
            else:
                core.busy = True
                core.process = process
                process.resource = core
                proc_info.gobject_.switch() #switch to greenlet associated with process
                # switches back to main greenlet here
        # else, no core was available. it has been assigned to a queue (either the main one
        # or a private queue. 


    def end_process(self, proc_info):
        """
        Call's process's end() method and frees up resource allocated to process
        """
        process = proc_info.object_
        process.end()
        if (not process.resource is None):
            self.free_resource(process)
        else:
            # wasn't allocated to a resource. The process is either 
            # waiting in a resource queue, or waiting for another process
            if self.pm.get_proc_status(process) == ProcStatus._waitfor:
                return
            # possibly in main queue
            try:
                self.ready_processes.remove(process)
                return
            except ValueError: 
                for core in self.node.cores:
                    try:
                        core.process_queue.remove(process)
                        return
                    except ValueError:
                        continue
            # should never reach here. If it does, it means process wasn't assigned
            # a resource, but is neither in any of the queues nor in a waitfor_ state
            ds.failure.write("Process",id(process)," cannot be found anywhere!")
            

    def free_resource(self, process):
        """
        Frees resource that was allocated to the processor; schedules new process
        from resource queue or main process queue
        """
        assert(process.resource)
        core = process.resource
        process.resource = None
        core.process = None
        core.busy = False
        #check private queue and see if there are waiting processes
        next_process = None
        if core.process_queue:
            next_process = core.process_queue.pop(0)
        else:
            if self.ready_processes:
                next_process = self.ready_processes.pop(0)

        if not next_process is None:
            # this is sort of a hack. it also generates a possibly unnecessary message
            # but I don't know of a better way to do it without generating an arbitrarily
            # deep call stack.
            # The idea is to assign resource to process, and put it to sleep for 
            # local  mindelay. it
            # will get woken up, and system will switch to it in assign_process
            next_process.resource = core
            core.process = next_process
            core.busy = True
            self.pm.proc_sleep(
                next_process, 
                simx.get_local_min_delay(), switch=False)
            
            
    def waitfor_process(self, p1, p2):
        """
        p1 waits for p2 to finish
        """
        #print "freeing resource"
        self.free_resource(p1)
        # the following statement will cause a switch back to the main greenlet
        self.pm.proc_waitfor(p1, p2)
       # when execution returns a resource would have been re-acquired 
       # and process is ready to run


ADDR_SYSTEM = 100 #System service will live at this address on a node
core.register_service(System)
core.register_address("ADDR_SYSTEM", ADDR_SYSTEM)
