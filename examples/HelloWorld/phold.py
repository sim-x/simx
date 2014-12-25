import simx
import random, math

count = 4
min_delay = 1

random.seed(0)

def exponential(mean):
    return -math.log(random.random())/mean


addr_RECEIVER = 1000
class Receiver(simx.PyService):
    def __init__(self,name,node,service_input):
        super(Receiver,self).__init__(name,node,service_input,self)

    def recv(self, msg):
        targetId = random.randrange(count)
        offset = int(exponential(1) + min_delay)
        #print "node ",self.get_entity_id()," sending message with offset ",\
        #    offset," at time ",simx.get_now(), "to target ", targetId
        self.send_info(None,offset,('n',targetId),
                       addr_RECEIVER)
                       
simx.register_service(Receiver)
simx.register_address("addr_RECEIVER",addr_RECEIVER)
        
class Node(simx.PyEntity):
    def __init__(self,ID,lp,entity_input,py_obj=None):
        super(Node,self).__init__(ID,lp,entity_input,self)
        self.install_service(Receiver, addr_RECEIVER)
        #print "node ",ID," being created"


simx.init("phold")
simx.set_end_time(500000)
simx.set_min_delay(1)
simx.init_env()
        
for i in xrange(count):
    simx.create_entity(('n',i),Node)

for i in xrange(count):
    simx.schedule_event(0,('n',i),addr_RECEIVER,None)

simx.run()
