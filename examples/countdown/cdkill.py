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

"""
Demo of process spawn, sleep, kill
"""

import simx


class CountDown(simx.Process):
    """
    A simple countdown process that will count down from 
    a given integer to 0. Process will sleep for a specified
    amount of time between count-down iterations.
    """

    def __init__(self, id_,from_):
        self.from_ = from_
        self.id_ = id_
        self.foo_procs = []
    def run(self):
        i = self.from_
        while i > 0:
            print "Countdown process: ",self.id_,"Time: %s: Value %s  " %(simx.get_now(),i)
            self.sleep(1)
            print "Countdown process: ",self.id_," wake up at time:", simx.get_now()
            #foo = fooprocess()
            #self.waitfor(fooprocess())
            foo = fooprocess(i)
            self.spawn(foo)
            self.foo_procs.append(foo)
            i -= 1
        
        for foo in self.foo_procs:
            self.kill_all(foo)

class fooprocess(simx.Process):
    """
    Just another process
    """
    def __init__(self,id_):
        self.id_ = id_

    def run(self):
        print "foo process: ",self.id_," Time: ",simx.get_now()
        self.sleep(2)
        print "foo process: ",self.id_," wake up at time:",simx.get_now()
        self.spawn(fooprocess(self.id_))

    def end(self):
        print "foo process: ",self.id_," ends here"

simx.init("cdkill")

simx.set_min_delay(1)
simx.set_end_time(100)
simx.set_log_level("debug3")

simx.init_env()

for i in range(1):
    cd = CountDown(i,10)
    simx.schedule_process( cd )

simx.run()
