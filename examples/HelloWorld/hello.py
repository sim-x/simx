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



import simx



from Person import *
from HelloHandler import *


####### Initialize MPI and configuration framework#######
simx.init("HelloWorld")

end_time = 2**10

##### set simulation configuration values ########
simx.set_min_delay(1)
simx.set_end_time(end_time)

# These are optional
simx.set_output_file("helloworld.out")
simx.set_log_level("info")
simx.set_log_file("helloworld.log")

####### Initialize environment (logging, output etc) ###########
simx.init_env()

##### Add services to be used in the simulation ########
# the second argument is a profile (should be dictionary (can be empty) or None)

#hh = simx.add_service('HelloHandlerPerson',None,[]) 

##### Create Entities ##########

# create an entity profile (optional)
#ep = { 'SERVICES':{eAddr_HelloHandlerPerson:hh}}

num_entities = 2**5

#num_entities = 2

for i in xrange(num_entities):
    # the third argument is a profile (should be dictionary (can be empty) or None)
    #simx.create_entity(('p',i),Person,ep,[('p',1-i)])
    simx.create_entity(('p',i),Person)

########## Schedule initial events, if any ###############
import random
def create_events():
    for evt_time in range(1,end_time):
        # pick a random entity for receiving hello
        hello_rcpt = random.choice(xrange(num_entities))
        # who should the reply be sent to ?
        reply_rcpt = random.choice(xrange(num_entities))
        simx.schedule_event( evt_time, ('p',hello_rcpt), eAddr_HelloHandlerPerson, 
                        HelloMessage(source_id=('p',reply_rcpt)))
        
    
#schedule_events()
es = simx.EventScheduler(create_events)

##### Run Simulation #################
simx.run()
