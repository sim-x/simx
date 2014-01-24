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
from cStringIO import StringIO

import core
#import simx_core


# Define debug streams
class DebugStream:
    def __init__(self,stream_type):
        self.stream_type = stream_type
        self.debug_str = StringIO()
        # define a logger function dictionary here    
        self.logger = {'debug1':core.debug1,
                       'debug2':core.debug2,
                       'debug3':core.debug3,
                       'info':core.debug_info,
                       'warn':core.warn,
                       'error':core.error,
                       'failure':core.failure }

    # define debug stream writer method   
    def write(self,*message):
        for token in message:
            self.debug_str.write(str(token));
            #self.debug_str.write(" ");
        self.logger[self.stream_type](self.debug_str.getvalue())
        self.debug_str.truncate(0)

#create debug streams here
debug1 = DebugStream('debug1')
debug2 = DebugStream('debug2')
debug3 = DebugStream('debug3')
info = DebugStream('info')
error = DebugStream('error')
warn = DebugStream('warn')
failure = DebugStream('failure')
