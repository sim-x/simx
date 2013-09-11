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

#from DebugStream import *
#from OutputStream import *

from HelloHandler import *

###### Define Entities ########
class Person(simx.PyEntity):
    def __init__(self,ID,lp,entity_input,py_obj=None):
        if py_obj is None:
            py_obj = self
        super(Person,self).__init__(ID,lp,entity_input,py_obj)
        simx.debug2.write("Person", self.get_id(),"is being created with input ",
                     entity_input.data_,"at time",self.get_now())
        self.neighbor_list = entity_input.data_
        #self.create_services(entity_input)


        self.install_service(HelloHandlerPerson, eAddr_HelloHandlerPerson)

        simx.debug3.write("Person",self.get_id(),"done",self)

    def say_hello(self,args=None):
        simx.output.write(self,100,"Person ",self.get_id(),"says hello")

    # def __str__(self):
    # #     #return "Person(%s)" %(self.neighbor_list)
    #     return "Person(%s)" %(self.get_now())

# register entity
#simx.register_entity(Person)
