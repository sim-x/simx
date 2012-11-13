import sys
from cStringIO import StringIO

import simx

# Define output streams
class OutputStream():
    def __init__(self):
        self.output_str = StringIO()
         # define output stream writer method   
    # sim_obj should be derived from either PyEntity or PyService    
    def write(self,sim_obj,record_type,*message):
        for token in message:
            self.output_str.write(str(token));
            self.output_str.write(" ");
        simx.output(sim_obj,record_type,self.output_str.getvalue())    
        self.output_str.truncate(0)

# create output stream
output = OutputStream()
