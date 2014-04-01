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

"""

Provides wrapper functions for setting configuration
parameters in simx

"""

import core

def set_num_proc( np ):
    """

    Sets the number of parallel simulation processes. The default is
    the number of MPI processes. 
    Argument must be an integer

    """
    core.set_config_value( "NUMBER_LPS", str(np))


def set_min_delay( min_delay ):
    """

    Sets the value for Look-Ahead in a parallel simulation.
    Determines how often processes have to synchronize
    This is a required parameter.
    Argument must be an integer.

    """
    core.set_config_value("MINDELAY",str(min_delay))


def set_end_time( end_time ):
    """
    
    Sets the end-time of the simulation. 
    This is a required parameter.
    Argument must be an integer.

    """
    core.set_config_value("END_TIME", str(end_time))


def set_output_file( of_name ):
    """

    Sets the prefix of the output file name.
    The suffix will be of the form AA, AB, AC etc depending on how many
    simulation processes there are.
    Argument must be a string

    """
    core.set_config_value("OUTPUT_FILE", of_name )


def set_log_cout_level( log_level ):

    """
    
    Undocumented

    """
    core.set_config_value("LOG_COUT_LEVEL", log_level )


def set_log_level( log_level ):

    """
    
    Sets the verbosity of the debug messages in the simulation. In increasing order of
    verbosity, these are:
    "info","debug1","debug2","debug3"
    Argument must be a string

    """
    core.set_config_value("LOG_LEVEL", log_level )


def set_log_file( lf_name ):
    """

    Sets the prefix for the log file name.
    The suffix will be of the form AA, AB, AC etc depending on how many
    simulation processes there are.
    Argument must be a string

    """
    core.set_config_value("LOG_FILE", lf_name )


def set_defaults( prog_name ):
    """

    Sets the default values for the following simulation parameters.

    - Numnber of simulation processes = number of MPI processes
    - Log level = "info"
    - Log cout level = "warn"
    - Log file name prefix = <prog_name>.log
    - Output file name prefix = <prog_name>.out
    
    Argument must be a string.

    """

    core.set_config_value( "NUMBER_LPS", str(0))
    core.set_config_value("LOG_LEVEL", "info" )
    core.set_config_value("LOG_COUT_LEVEL", "warn" )
    core.set_config_value("OUTPUT_FILE", prog_name+".out")
    core.set_config_value("LOG_FILE", prog_name+".log")
    
