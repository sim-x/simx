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

from greenlet import greenlet
from collections import namedtuple

import core
import util
import DebugStream as ds
import controller
from process import Process

eAddr_ProcessManager = 1000

class _ProcStatus:
    _scheduled = 0
    _active = 1
    _wait_for = 2
    _sleep = 3
    _inactive = 4


"""
_ProcInfo is a named tuple only for use by ProcessManager. It represents
 a proces table entry.
 object_ : the actual process object
 gobject_: the greenlet associated with this process
 status_ : the status of this process (valid values are defined in _ProcStatus)
 parent_ : the parent process that called this process. Processes called by the 
           main simulation process will have parent as None.
"""
#_ProcInfo = namedtuple('ProcInfo',['object_','gobject_','status_','parent_'])


class _ProcInfo:
    def __init__(self,object_=None,gobject_=None,
                 status_=_ProcStatus._inactive,parent_=None):
        self.object_ = object_
        self.gobject_ = gobject_
        self.status_ = status_
        self.parent_ = parent_


class _ProcActivateMsg:
    def __init__(self,pid):
        self.pid = pid
        
        
        
class _ProcWakeUpMsg:
    def __init__(self,pid):
        self.pid = pid




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
        ds.debug1.write("Process Manager ", name, "being created on entity",
                          entity)
        self.proc_table = {}
        self.recv_function = {'_ProcActivateMsg':
                                  self.recv_activate,
                              '_ProcWakeUpMsg':
                                  self.recv_wakeup}
        

    def recv(self, msg):
        self.recv_function[msg.__class__.__name__](msg)


    
    def recv_activate(self, msg):
        proc_info = self.proc_table[msg.pid]
        if proc_info.status_ != _ProcStatus._scheduled:
            ds.failure.write("ProcessManager: Invalid activation mesage received")
            return
        process = proc_info.object_
        pg = greenlet(process.run)
        proc_info.gobject_ = pg
        self.proc_switch(proc_info)

    
    def recv_wakeup(self, msg):
        proc_info = self.proc_table[msg.pid]
        if proc_info.status_ != _ProcStatus._sleep:
            ds.failure.write("ProcessManager: Invalid wake-up message received")
        self.proc_switch(proc_info)


    def proc_switch(self, proc_info):
        proc_info.status_ = _ProcStatus._active
        #if not bool(proc_info.gobject_):
        #    ds.failure.write("Invalid greenlet state for process")
#                             proc_info.object_.__name__)
        proc_info.gobject_.switch()
        if proc_info.gobject_.dead:
            self.proc_deactivate( proc_info )


    def proc_deactivate(self, proc_info):
        proc_info.gobject_ = None
        proc_info.status_ = _ProcStatus._inactive
        

    
    def proc_activate( self, process, delay = core.get_local_min_delay() ):
        """
        Schedules a process to get executed after time given by delay
        The default delay is the value of local_min_delay (1 time unit)
        """
        util.check_type(Process, process)
        if self.proc_table.has_key(id(process)):
            if self.proc_table[id(process)].status_ != _ProcStatus._inactive:
                ds.failure.write("ProcessManager: Process must either be new or inactive in order to be activated")
        pi = _ProcInfo( object_ = process,
                       gobject_ = None,
                       status_ = _ProcStatus._scheduled,
                       parent_ = None )
        self.proc_table[id(process)] = pi
        msg = _ProcActivateMsg( pid = id(process) )
        self.send_to_self( msg, delay )
                                

    
    def proc_sleep( self, process, duration ):
        """
        Puts a process to sleep for time units specified 
        by duration.
        """
        util.check_type(Process, process)
        proc_info = self.proc_table[id(process)]
        if proc_info.status_ != _ProcStatus._active:
            ds.failure.write("ProcessManager: Cannot sleep a process that is not active")
        msg = _ProcWakeUpMsg( pid = id(process) )
        self.send_to_self( msg, duration )
        proc_info.status_ = _ProcStatus._sleep
        greenlet.getcurent().parent.switch()


    def proc_wait_for(self, p1, p2):
        """
        Suspends process p1 till p2 finishes
        execution
        """
        pass


    def send_to_self(self, msg, delay):
        """
        Sends a message to self with given delay
        """
        self.send_info( msg, delay, 
                        self.get_entity_id(), eAddr_ProcessManager)



core.register_service(ProcessManager)
core.register_address("eAddr_ProcessManager",eAddr_ProcessManager)



def get_process_mgr():
    """
    A helper function that returns the handle of the
    process manager service
    """
    return controller.get_controller().get_process_mgr()

