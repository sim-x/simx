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


import sys
from greenlet import greenlet

import core
#from simx.util import *

_evt_scheduler = None

class EventScheduler:
    """
    Class for staggered creation of initial events.
    External events will be scheduled upto a time in the future,
    and this process will be repeated till there are no more
    external events
    
    """

    def __init__(self, event_creator):
        global _evt_scheduler 
        _evt_scheduler = self
        core.set_event_scheduler(self)
        self.time_chunk = 1000 * core.get_local_min_delay()
        self.upto_time = core.get_now() + self.time_chunk
        self.event_creator = greenlet(event_creator)
        self.event_creator.switch()

    def schedule_next(self, time):
        """
        Schedules a timer for creating next chunk of events
        """
        if time <= self.upto_time:
            return
        #else
        #schedule a timer just before this event and switch back to main "thread"
        #print "SHOULD NOT"
        core.set_event_scheduler_timer(time)
        greenlet.getcurrent().parent.switch()
    # when switching back to this "thread", execution resumes from here
    
    def process_scheduler_event(self):
        #print "NEW BATCH OF EVENTS BEING SCHEDULED"
        self.upto_time = core.get_now() + self.time_chunk
        #switch to event_creator thread
        self.event_creator.switch()


def schedule_event(time,entity,address,event):
    """
    
    Schedules an event to be received by 'entity'

    Keyword Arguments:

    time    --  (When)  time at which event will be processed
    entity  --  (Who )  id of entity that will process the event
    address --  (Where) address of service on entity that will receive
                        event
    event   --  (What)  event payload

    """
    

    #if is_set_evt_scheduler():
    if not _evt_scheduler is None:
       _evt_scheduler.schedule_next(time)


    info_data = core.InfoData( time, entity, 
                                  address, 1000, 1, {}, event)
    core.create_input_info( info_data )


def is_set_evt_scheduler():
    """
    Returns true if the staggered event scheduler
    has been created
    """
    return not _evt_scheduler is None
