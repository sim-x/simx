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

_service_names = {}

"""


Provides commonly used utility functions 

"""

from DebugStream import error

def get_profile_id( profile_obj):
    """
    Returns an integer id for a dictionary profile that will be used
    to identify this profile inside core

    """
    # todo (critical). This has to be fixed!
    return int(id(profile_obj) % 1000000)


def get_internal_service_name( serv_name ):
    """
    Creates and returns a core internal service name for serv_name
    
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




def get_class_name(obj):
    """
    Returns class name of given object

    """
    return obj.__class__.__name__


def check_type(class_type, obj):
    """
    Checks if obj is an instance of class and
    raises a Type error if not
    """

    if not isinstance(obj, class_type):
        err_str = ''.join[obj.__name__,"must be of type",class_type.__name__]
        DebugStream.error.write(err_str)
        raise TypeError, err_str

        
