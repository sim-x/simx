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

import sys

import simx
from DebugStream import *

sys.path.append("/Library/Python/2.6/site-packages/greenlet-0.4.0-py2.6-macosx-10.6-universal.egg")

from greenlet import greenlet

""" 
Provides simx intialization routines for 
python applications. Logging and debug streams must have
already been initialized before calling functions here. 

"""

_service_names = {}
#_evt_scheduler = None

def init(prog_name=""):
    simx.init_mpi(prog_name);
    simx.init_config()

def add_service( serv_name, serv_profile={}, serv_data=() ):
    """
    Creates a simx service data object for the service 
    defined by the arguments. Calls simx.prepare_services
    on service data object. Returns the internal simx name
    for serv_name

    Keyword arguments:

    serv_name -- Name of service
    serv_profile -- A dictionary that defines the profile for serv_name
    serv_data  --  Custom data for an insantation of serv_name

    """
    if serv_profile == None:
        serv_profile = {}
    if not type(serv_profile) == dict:
        failure.write("Python: Error in add_service while adding",serv_name,
                      "Python profiles must be dictionaries")
        return None
    
    sn = get_internal_service_name(serv_name)
    # TODO (critical) profile-id generation has to be fixed!
    prof_id = get_profile_id(serv_profile)
    sd = simx.ServiceData(sn,serv_name,prof_id,
                             serv_profile,serv_data )
    debug2.write("Service profile",serv_profile,"has id",id(serv_profile))
    simx.prepare_services([sd])
    return sn


def create_entity( ent_name, ent_class, ent_profile={}, ent_data=() ):

    """

    Creates a simx entity with id = ent_name and type ent_class.
    First creates an EntityData object and then passes the data object
    to the create_py_entity function in simx.

    Keyword arguments:

    ent_name    --   id of entity
    ent_class   --   entity class type
    ent_profile --   entity profile, can be None (will be set to 
                     an empty dictionary if None)
    ent_data    --   any custom data to be passed to entity                 
    """

    if not issubclass(ent_class, simx.PyEntity):
        error.write("Argument ",ent_class," not of type PyEntity")
        return None

    if ent_profile == None:
        ent_profile = {}

    if not type(ent_profile) == dict:
        failure.write("Python: Error. Entity profiles must be dictionaries.") 
        return None

    prof_id = get_profile_id(ent_profile)
    ed = simx.EntityData(ent_name,ent_class,prof_id,
                            ent_profile,ent_data)
    debug2.write("Entity profile",ent_profile,"has id",prof_id)
    simx.create_pyentity(ed)


def get_profile_id( profile_obj):
    """
    Returns an integer id for a dictionary profile that will be used
    to identify this profile inside simx

    """
    # todo (critical). This has to be fixed!
    return int(id(profile_obj)/1000000)


def get_internal_service_name( serv_name ):
    """
    Creates and returns a simx internal service name for serv_name
    
    Keyword Arguments:
    serv_name -- A service name for which an internal name will be generated
    
    Output:
    A string having the format 'eServ_<serv_name>.<id>'
    where id starts at 0 and is incremented for each  
    entry of serv_name.
    
    """ 
    global _service_names
    if _service_names.has_key(serv_name):
        s_id = _service_names[serv_name]
    else:
        s_id = 0
    int_service_name = 'eServ_'+serv_name+'.'+str(s_id)
    _service_names[serv_name] = s_id+1
    return int_service_name


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


    info_data = simx.InfoData( time, entity, 
                                  address, 1000, 1, {}, event)
    simx.create_input_info( info_data )


def is_set_evt_scheduler():
    """
    Returns true if the staggered event scheduler
    has been created
    """
    return not _evt_scheduler is None


def get_class_name(obj):
    """
    Returns class name of given object

    """
    return obj.__class__.__name__


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
        simx.set_event_scheduler(self)
        self.time_chunk = 1000 * simx.get_local_min_delay()
        self.upto_time = simx.get_now() + self.time_chunk
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
        simx.set_event_scheduler_timer(time)
        greenlet.getcurrent().parent.switch()
    # when switching back to this "thread", execution resumes from here
    
    def process_scheduler_event(self):
        #print "NEW BATCH OF EVENTS BEING SCHEDULED"
        self.upto_time = simx.get_now() + self.time_chunk
        #switch to event_creator thread
        self.event_creator.switch()
