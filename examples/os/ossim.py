# Copyright (c) 2014. Los Alamos National Security, LLC. 

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
A script which simulates a computer node with an OS, running processes
"""

import simx
import simx.os as os


class BarProcess(os.Process):
     def __init__(self):
        print "Bar process ",id(self)," created on node",self.os.get_entity_id()
        
     def run(self):
         print "Bar process",id(self),"on node",self.os.get_entity_id(), \
             " started running on resource ", self.resource, \
             " at time ", simx.get_now()
         
        
         self.compute(100)
         print "Bar process ",id(self)," on node",self.os.get_entity_id(), \
            "finished computing on resource ",self.resource, \
            " at time ", simx.get_now()


#define a process
class FooProcess(os.Process):
    
    def __init__(self):
        print "Foo process created on node",self.os.get_entity_id()


    def run(self):
        print "Foo process",id(self),"on node",self.os.get_entity_id(),  \
             " started running on resource ", self.resource, \
             " at time ", simx.get_now()

        

        # spawns Bar process, and continues executing
        bp = self.spawn(BarProcess)
        #self.kill(bp)
        self.compute(100)
        self.kill(bp)
        print "Foo process ",id(self)," on node",self.os.get_entity_id(), \
            "finished computing on resource ",self.resource, \
            " at time ", simx.get_now()
        
        
        bp = self.os.create_process(BarProcess)
        # waits for Bar process to finish before resuming
        self.waitfor(bp)

        print "Foo process ",id(self)," on node",self.os.get_entity_id(), \
            "returned from waitfor. Acquired resource ",self.resource, \
            " at time ", simx.get_now()



#initialize
simx.init("ossim")
simx.set_end_time(1000)
simx.set_min_delay(1)
simx.set_log_level("debug3")
simx.init_env()

num_nodes = 1

#create nodes
for i in xrange(num_nodes):
    simx.create_entity(('n',i), os.Node,ent_data=({'num_cores':1}))

num_processes = 1
#on each node create foo processes
for i in xrange(num_nodes):
    node = simx.get_entity(('n',i))
    # the node might not live in this memory space
    # in a distributed memory simulation

    if node is not None: 
         
         for j in range(num_processes):
              fp = node.os.create_process(FooProcess)
              node.os.schedule_process(fp)

simx.run()

