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
import process
from process import Process
#import process.Process as Process

eAddr_ProcessManager = 1000

class _ProcStatus:
    _scheduled = 0
    _active = 1
    _waitfor = 2
    _sleep = 3
    _inactive = 4


_gr_pm_map = {} #a greenlet to proces manager map.

#TODO (high) debug messages for process functions

class _ProcInfo:

    """
    _ProcInfo: This class is only for use by ProcessManager. It represents
    a proces table entry.
    object_ : the actual process object
    gobject: the greenlet associated with this process
    status : the status of this process (valid values are defined in _ProcStatus)
    parent : the parent process that called this process. Processes called by the 
              main simulation process will have parent as None.
    waitfor:  A process that is being waited upon by this process
    """

    def __init__(self,object_=None,gobject=None,
                 status=_ProcStatus._inactive,parent=None, waitfor = None):
        self.object_ = object_
        self.gobject_ = gobject
        self.status_ = status
        self.parent_ = parent
        self.waitfor_ = waitfor

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
        self.proc_activate( proc_info )

    
    def recv_wakeup(self, msg):
        proc_info = self.proc_table[msg.pid]
        if proc_info.status_ != _ProcStatus._sleep:
            ds.failure.write("ProcessManager: Invalid wake-up message received")
        self.proc_switch(proc_info)


    def proc_switch(self, proc_info):
        proc_info.status_ = _ProcStatus._active
        if  proc_info.gobject_.dead:
            ds.failure.write("Invalid greenlet state for process object ",
                             proc_info.object_.__class__.__name__)
        proc_info.gobject_.switch()
        # Execution of main thread resumes here when the greenlet 
        # switches  back.

        # check if process went from active to waitfor
        if proc_info.status_ == _ProcStatus._waitfor:
            #self.proc_activate( process = proc_info.waitfor_, parent=proc_info.object_ )
            #pn = self.proc_new( process = proc_info.waitfor_, parent=proc_info.object_)
            #self.proc_switch( pn )
            self.proc_schedule( process = proc_info.waitfor_, parent = proc_info.object_,
                                delay = 0)

        # check if process finished
        if proc_info.gobject_.dead:
            # check if another process was waiting on this one
            parent = proc_info.parent_
            if not parent is None:
                # first deactivate finished process
                self.proc_deactivate( proc_info )
                # then initiate switch to parent
                self.proc_switch(self.proc_table[id(parent)])
            else:
                # else just deactivate finished process
                self.proc_deactivate( proc_info )


    def proc_deactivate(self, proc_info):
        proc_info.gobject_ = None
        proc_info.status_ = _ProcStatus._inactive
        

    
    def proc_schedule( self, process, delay = core.get_local_min_delay(), parent = None ):
        """
        Schedules a process to get executed after time given by delay
        The default delay is the value of local_min_delay (1 time unit)
        """
        pn = self.proc_new( process, parent)
        if delay > 0:
            msg = _ProcActivateMsg( pid = id(process) )
            self.send_to_self( msg, delay )
        else:
            self.proc_activate( pn )



    def proc_activate(self, proc_info):
        """
        Creates the greenlet that will run the process
        and switches to the process
        """
        process = proc_info.object_
        pg = greenlet(process.run)
        #updates the global greenlet-to-process manager object map
        global _gr_pm_map
        if _gr_pm_map.has_key(pg):
            ds.failure.write("ProcessManager: New greenlet has entry in map!!")
        _gr_pm_map[pg] = self
        proc_info.gobject_ = pg
        self.proc_switch(proc_info)


    def proc_new( self, process, parent = None):
        """
        Creates a new process table entry for process
        """
        util.check_type(Process, process)
        if self.proc_table.has_key(id(process)):
            if self.proc_table[id(process)].status_ != _ProcStatus._inactive:
                ds.failure.write("ProcessManager: Process must either be new or inactive in order to be activated")
        pi = _ProcInfo( object_ = process,
                        gobject = None,
                        status = _ProcStatus._scheduled,
                        parent = parent, 
                        waitfor = None)
        self.proc_table[id(process)] = pi
        return pi


    def proc_sleep( self, process, duration = None ):
        """
        Puts a process to sleep for time units specified 
        by duration. If no duration is specified, process
        is put to sleep indefinitely till woken up by the
        main thread
        """
        util.check_type(Process, process)
        proc_info = self.proc_table[id(process)]
        if proc_info.status_ != _ProcStatus._active:
            ds.failure.write("ProcessManager: Invalid State for process ", 
                             process.__class__.__name__,
                             " Cannot sleep a process that is not active")
        if duration > 0:
            msg = _ProcWakeUpMsg( pid = id(process) )
            self.send_to_self( msg, duration )
        proc_info.status_ = _ProcStatus._sleep
        greenlet.getcurrent().parent.switch()


    def proc_waitfor(self, p1, p2):
        """
        Suspends process p1 till p2 finishes
        execution
        """
        util.check_type(Process, p1)
        p1_info = self.proc_table[id(p1)]
        if not p1_info.gobject_ == greenlet.getcurrent():
            ds.failure.write("ProcessManager: Invalid greenlet call state")
        if not p1_info.status_ == _ProcStatus._active:
            ds.failure.write("ProcessManager: Process not in active state")
        p1_info.status_ = _ProcStatus._waitfor
        p1_info.waitfor_ = p2
        greenlet.getcurrent().parent.switch()


    def send_to_self(self, msg, delay):
        """
        Sends a message to self with given delay
        """
        self.send_info( msg, delay, 
                        self.get_entity_id(), eAddr_ProcessManager)


    def get_gr_process(self, grlt):
        """
        Returns the process object associated with a 
        greenlet.
        """
        for pid in self.proc_table:
            if self.proc_table[pid].gobject_ == grlt:
                return self.proc_table[pid].object_
        return None


core.register_service(ProcessManager)
core.register_address("eAddr_ProcessManager",eAddr_ProcessManager)



def get_process_mgr():
    """
    A helper function that returns the handle of the
    process manager service
    """
    return controller.get_controller().get_process_mgr()



def get_current_process():
    """
    Called from within the execution frame of a Process.
    Returns the current active process.
    """
    try:
        proc_mgr = _gr_pm_map[greenlet.getcurrent()]
    except KeyError:
        ds.failure.write("Cannot find a process manager service for greenlet.")
    return proc_mgr.get_gr_process(greenlet.getcurrent())

