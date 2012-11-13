import sys

import simx

from DebugStream import *
from OutputStream import *

###### Define Entities ########
class Person(simx.PyEntity):
    def __init__(self,ID,lp,entity_input):
        super(Person,self).__init__(ID,lp,entity_input,self)
        debug3.write("I'm a Python Person Entity with id", 
                     self.getId(),"created at time ",self.getNow())
        self.create_services(entity_input)

    def say_hello(self,args=None):
        output.write(self,100,"Person ",self.getId(),"says hello")

# register entity
simx.register_entity(Person)
