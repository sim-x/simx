import simx
import random

endTime = 500
count = 100000
maxSleep = 100

#Initialize
simx.init("GREEN")
simx.set_end_time(endTime)
simx.set_min_delay(1)
simx.set_log_level("warn")
simx.init_env()

#Create a message generation process
class processApp(simx.Process):
    """
    Schedule the compute nodes to run their applications (Booting Up)
    """
    def run(self):
        while True:
            x = int(maxSleep*random.random())
            #print "Sleeping for", x
            self.sleep(x)
            #print "Woke"

for i in xrange(count):
    #print "Adding process", i
    mg = processApp()
    simx.schedule_process(mg)

#raw_input("Press Enter to continue...")
print "Starting Run"
simx.run()
