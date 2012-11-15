import sys
from cStringIO import StringIO

import simx


# Define debug streams
class DebugStream:
    def __init__(self,stream_type):
        self.stream_type = stream_type
        self.debug_str = StringIO()
        # define a logger function dictionary here    
        self.logger = {'debug1':simx.debug1,
                       'debug2':simx.debug2,
                       'debug3':simx.debug3,
                       'info':simx.debug_info,
                       'warn':simx.warn,
                       'error':simx.error,
                       'failure':simx.failure }

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
