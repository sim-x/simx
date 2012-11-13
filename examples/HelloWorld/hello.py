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


##### set simulation configuration values ########
simx.set_config_value("NUMBER_LPS","0")
simx.set_config_value("MINDELAY","10")
simx.set_config_value("END_TIME","1000000")
simx.set_config_value("OUTPUT_FILE","output_HelloWorld.out")
simx.set_config_value("LOG_COUT_LEVEL","warn")
simx.set_config_value("LOG_LEVEL","info")
simx.set_config_value("LOG_FILE","helloworld.log")

####### Initialize environment (logging, output etc) ###########
simx.init_env()


##### Add services to be used in the simulation ########
# the second argument is a profile (should be dictionary (can be empty) or None)
hh = add_service('HelloHandlerPerson',None,[1,2]) 

##### Create Entities ##########

# create an entity profile (optional)
ep = { 'SERVICES':{eAddr_HelloHandlerPerson:hh}}


for i in range(4):
    # the third argument is a profile (should be dictionary (can be empty) or None)
    create_entity(('p',i),'Person',ep,[('p',1-i)])

##### Schedule initial events, if any ###############
import random
for evt_time in xrange(100000):
    j = random.choice([0,1,2,3])
    k = random.choice([0,1,2,3])
    #j =random.choice([0,1])
    #k = 1 - j
    schedule_event( evt_time, ('p',j), eAddr_HelloHandlerPerson, HelloMessage([('p',k)]))

##### Run Simulation #################
#import cProfile
simx.run()
