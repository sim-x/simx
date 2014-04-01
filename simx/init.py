# Copyright (c) 2012, 2013. Los Alamos National Security, LLC. 

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


import core
#import core_ext
#import DebugStream as ds
#import util
import config
from controller import Controller

#from controller import Controller

""" 
Provides core intialization routines for 
python applications. Logging and debug streams must have
already been initialized before calling functions here. 
Also sets the default configuration parameters.
"""


def init(prog_name=""):
    core.init_mpi(prog_name);
    core.init_config()
    config.set_defaults(prog_name)
    

def init_env():
    """
    Sets up MPI rank (if any), initializes logging and output, and creates
    the controller entity for this process
    """
    core.init_env()
    create_controller()


def create_controller(): # TODO: should this \be moved to the init function?
    """
    Creates a controller entity on this proces. Strictly speaking, this is
    not required, but is highly useful. The controller id will be (!,0) in
    a serial simulation. In a parallel simulation, it will be (!,n) where n is 
    the MPI rank of this python process.
    """
    core.create_entity(('!',core.get_rank()),Controller)

