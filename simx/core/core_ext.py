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

"""
Extensions to simx.core
"""

import core
#from  simx import DebugStream as ds
import DebugStream as ds

#from  simx import util
import util

#from simx.controller import Controller
#from controller import Controller
#from simx.processmgr import get_process_mgr





    


def add_service( serv_name, serv_profile={}, serv_data=() ):
    """
    Creates a core service data object for the service 
    defined by the arguments. Calls core.prepare_services
    on service data object. Returns the internal core name
    for serv_name

    Keyword arguments:

    serv_name -- Name of service
    serv_profile -- A dictionary that defines the profile for serv_name
    serv_data  --  Custom data for an insantation of serv_name

    """
    if serv_profile == None:
        serv_profile = {}
    if not type(serv_profile) == dict:
        ds.failure.write("Python: Error in add_service while adding",serv_name,
                      "Python profiles must be dictionaries")
        return None
    
    sn = util.get_internal_service_name(serv_name)
    # TODO (critical) profile-id generation has to be fixed!
    prof_id = util.get_profile_id(serv_profile)
    sd = core.ServiceData(sn,serv_name,prof_id,
                             serv_profile,serv_data )
    ds.debug2.write("Service profile",serv_profile,"has id",id(serv_profile))
    core.prepare_services([sd])
    return sn


def create_entity( ent_name, ent_class, ent_profile={}, ent_data=() ):

    """

    Creates a core entity with id = ent_name and type ent_class.
    First creates an EntityData object and then passes the data object
    to the create_py_entity function in core.

    Keyword arguments:

    ent_name    --   id of entity
    ent_class   --   entity class type
    ent_profile --   entity profile, can be None (will be set to 
                     an empty dictionary if None)
    ent_data    --   any custom data to be passed to entity                 
    """

    if not issubclass(ent_class, core.PyEntity):
        ds.error.write("Argument ",ent_class," not of type PyEntity")
        return None

    if ent_profile == None:
        ent_profile = {}

    if not type(ent_profile) == dict:
        ds.failure.write("Python: Error. Entity profiles must be dictionaries.") 
        return None

    prof_id = util.get_profile_id(ent_profile)
    ed = core.EntityData(ent_name,ent_class,prof_id,
                            ent_profile,ent_data)
    ds.debug2.write("Entity profile",ent_profile,"has id",prof_id)
    core.create_pyentity(ed)
