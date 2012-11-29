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

#import sys



import simx
from simx_init import *
from DebugStream import *
from OutputStream import *



from Person import *
from HelloHandler import *

#import gc
#gc.disable()

####### Initialize MPI and configuration framework#######
init("HelloWorld")

end_time = 100

##### set simulation configuration values ########
simx.set_config_value("NUMBER_LPS","0")
simx.set_config_value("MINDELAY","10")
simx.set_config_value("END_TIME",str(end_time))
simx.set_config_value("OUTPUT_FILE","output_HelloWorld.out")
simx.set_config_value("LOG_COUT_LEVEL","warn")
simx.set_config_value("LOG_LEVEL","info")
simx.set_config_value("LOG_FILE","helloworld.log")

####### Initialize environment (logging, output etc) ###########
simx.init_env()


##### Add services to be used in the simulation ########
# the second argument is a profile (should be dictionary (can be empty) or None)
hh = add_service('HelloHandlerPerson',None,[]) 

##### Create Entities ##########

# create an entity profile (optional)
ep = { 'SERVICES':{eAddr_HelloHandlerPerson:hh}}


for i in xrange(2):
    # the third argument is a profile (should be dictionary (can be empty) or None)
    create_entity(('p',i),'Person',ep,[('p',1-i)])

##### Schedule initial events, if any ###############
import random
for evt_time in xrange(end_time):
    # pick a random entity for receiving hello
    hello_rcpt = random.choice([0,1])
    # who should the reply be sent to ?
    reply_rcpt = random.choice([0,1])
    schedule_event( evt_time, ('p',hello_rcpt), eAddr_HelloHandlerPerson, 
                    HelloMessage(source_id=('p',reply_rcpt)))

##### Run Simulation #################
#import cProfile
simx.run()
