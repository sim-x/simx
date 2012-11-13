import sys
import cPickle

import simx

from DebugStream import *
from OutputStream import *
from Message import *

import random

#### Define Services ##########
eAddr_HelloHandlerPerson = 11101;


class HelloHandlerPerson(simx.PyService):

    def __init__(self,name, person, service_input ):
        super(HelloHandlerPerson,self).__init__( name, person, service_input,self )
        debug3.write(self.get_name(),service_input.data_)   
        self.person = person
        self.person.say_hello()
        self.recv_function = {'HelloMessage':self.recv_hello,
                              'ReplyMessage':self.recv_reply
                              }
        self.id_num = self.get_entity_id()[1]
        #print "hello handler entity id number is",self.id_num
        #self.neighbor_id = ('p',1-self.id_num)
        #print "neighbor id is ",self.neighbor_id

    def recv(self,msg):
        output.write(self,200,'this is service output',self.get_random().
                     get_uniform() )
        debug3.write("HelloHandlerPerson: Inside receive method")
        self.recv_function[msg.__class__.__name__](msg)
        #rf = random.choice([self.recv_hello, self.recv_reply])
        #rf(msg)

    def recv_hello(self,msg):    
        debug3.write("hello message received")
        neighbor = msg.neighbors[0]
        #neighbor = self.neighbor_id
        self.send_info(ReplyMessage(range(10)),
                       simx.get_min_delay(),neighbor,eAddr_HelloHandlerPerson)

    def recv_reply(self,msg):
        debug3.write("reply message received")
        debug3.write("reply message data",msg.data)

# should we make this one call? i.e register_service and register_addr?
simx.register_service(HelloHandlerPerson)
simx.register_address("eAddr_HelloHandlerPerson",eAddr_HelloHandlerPerson)
