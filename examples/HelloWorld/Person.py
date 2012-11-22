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

import simx

from DebugStream import *
from OutputStream import *

###### Define Entities ########
class Person(simx.PyEntity):
    def __init__(self,ID,lp,entity_input):
        super(Person,self).__init__(ID,lp,entity_input,self)
        debug2.write("Person", self.getId(),"is being created with input ",
                     entity_input.data_,"at time",self.getNow())
        self.neighbor_list = entity_input.data_
        self.create_services(entity_input)
        debug3.write("Person",self.getId(),"done",self)

    def say_hello(self,args=None):
        output.write(self,100,"Person ",self.getId(),"says hello")

    def __str__(self):
        return "Person(%s)" %(self.neighbor_list)

# register entity
simx.register_entity(Person)
