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

import simx
from Person import *
from HelloHandler import *

num_entities = 32
end_time = 1024

# initialize
simx.init("helloworldv2")
simx.set_end_time(end_time)
simx.set_min_delay(1)
simx.init_env()



# create Persons
for i in xrange(num_entities):
    simx.create_entity(('p',i), Person)
    
# create a message generation process
class MessageGen (simx.Process):
    """
    Schedule simple message sending and receiving
    """
    def run(self):
        for evt_time in range(1,end_time):
            hello_rcvr = ('p',random.choice(xrange(num_entities)))
            reply_rcvr = ('p',random.choice(xrange(num_entities)))
            simx.schedule_event(evt_time, hello_rcvr, eAddr_HelloHandlerPerson,
                                HelloMessage(source_id=reply_rcvr))
            #schedule in chunks of 10 time units
            if (evt_time % 10 == 0):
                # go to sleep, and wake up in time to schedule
                # next batch of events
                 self.sleep(evt_time - simx.get_now())
            
            
mg = MessageGen()
simx.schedule_process(mg)
simx.run()
