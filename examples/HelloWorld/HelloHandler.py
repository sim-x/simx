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
import cPickle

import simx

#from DebugStream import *
#from OutputStream import *
from Message import *

import random
import math

#### Define Services ##########
eAddr_HelloHandlerPerson = 11101;


class HelloHandlerPerson(simx.PyService):

    def __init__(self,name, person, service_input ):
        super(HelloHandlerPerson,self).__init__( name, person, service_input,self )
        simx.debug2.write("HelloHandlerPerson in constructor",
                     name, person.get_id(), service_input.data_)   
        self.person = person
        #self.person.say_hello()
        self.recv_function = {'HelloMessage':self.recv_HelloMessage,
                              'ReplyMessage':self.recv_ReplyMessage
                              }
        #self.id_num = self.get_entity_id()[1]
        #print "hello handler entity id number is",self.id_num
        #self.neighbor_id = ('p',1-self.id_num)
        #print "neighbor id is ",self.neighbor_id

    def recv(self,msg):
        
        #debug3.write("HelloHandlerPerson: Inside receive method")
        self.recv_function[msg.__class__.__name__](msg)
        #rf = random.choice([self.recv_hello, self.recv_reply])
        #rf(msg)

    def recv_HelloMessage(self,msg):    
        simx.debug3.write("HelloHandler received hello",msg)
        simx.output.write(self,100,'Received a hello')
        simx.output.write(self,200,"Random number:",
                     self.get_random().get_uniform() )
        #simulate some computation
        # x = int(random.expovariate(0.00001))
        # for i in xrange(x):
        #     j = math.sqrt(i)
        self.send_info(ReplyMessage(source_id=self.get_entity_id(),
                                    dest_id=msg.source_id),
                       simx.get_min_delay(), msg.source_id,eAddr_HelloHandlerPerson)

    def recv_ReplyMessage(self,msg):
        simx.debug3.write("HelloHandlerPerson::receive(Reply):",msg)
        simx.output.write(self,100,"Received a reply")

    def __str__(self):
        return "HelloHandler(%s)" %(self.get_entity_id())

# should we make this one call? i.e register_service and register_addr?
simx.register_service(HelloHandlerPerson)
simx.register_address("eAddr_HelloHandlerPerson",eAddr_HelloHandlerPerson)
