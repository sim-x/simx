import networkx as nx
import matplotlib.pyplot as plt

import simx

#[5,5,50,200,1024,0.125]
#[5,5,50,400,1024,0.0825]

min_delay = 5
update_interval = 5
#min_delay =1 
#update_interval = 1
#velocity scale factor
vel_scale_factor = 50
#vel_scale_factor = 10
num_nodes = 200
end_time = 1024
#graph_radius = 0.0825
graph_radius = 0.125
#graph_radius = 0.05

#position info for nodes created on this process
local_pos = {}

#setting the seed below ensures that all simulation
#generate the same graph
import random
random.seed(1)

G=nx.random_geometric_graph(num_nodes,graph_radius)
# position is stored as node attribute data for random_geometric_graph
# find node near center (0.5,0.5)


Gpos=nx.get_node_attributes(G,'pos')
dmin=1
ncenter=0
for n in Gpos:
    x,y=Gpos[n]
    d=(x-0.5)**2+(y-0.5)**2
    if d<dmin:
        ncenter=n
        dmin=d
p=nx.single_source_shortest_path_length(G,ncenter)

class LocListMsg:
    def __init__(self, pos_info):
        self.pos_info = pos_info


eAddr_RECVR = 999

class Receiver(simx.PyService):
    """
    Lives on the controller entity. Handles location messages from other processes and
    updates GPos data structure
    """

    def __init__(self, name, entity, service_input):
        super(Receiver,self).__init__(name, entity, service_input, self )
        simx.debug2.write("receiver in constructor",name, entity.get_id(), service_input.data_)
        self.recv_function = {'LocMsg': self.recv_loc_msg, 'LocListMsg':self.recv_loc_list_msg}
        
        
    def recv(self, msg):
        self.recv_function[msg.__class__.__name__](msg)
    
    def recv_loc_msg(self, msg):
        assert(False)
        try:
            self.node.neighbor_pos[msg.id] = msg.loc
        except KeyError:
            print "Neighbor ", id, " does not exist for node ",self.node.get_id()
    
    def recv_loc_list_msg(self,msg):
        for node,location in msg.pos_info.iteritems():
            Gpos[node] = location

simx.register_service(Receiver)
simx.register_address("eAddr_RECVR",eAddr_RECVR)


class Node(simx.PyEntity):

    def __init__(self,ID, lp, entity_input, py_obj=None):
        if py_obj is None:
            py_obj = self
        super(Node,self).__init__(ID,lp,entity_input,py_obj)
        simx.debug2.write("Node",self.get_id(),"is being created with input ",
                          entity_input.data_," at time ",self.get_now())
        self.attr = entity_input.data_
        self.gnode_id = self.attr['gnode_id']
        self.loc = Gpos[self.gnode_id]
        self.vel = (0.0,0.0) 
        self.last_update_time = 0
        self.neighbors = G.neighbors(self.gnode_id)
        self.neighbor_pos={}
        for neighbor in self.neighbors:
            self.neighbor_pos[neighbor] = Gpos[neighbor]


    def update_pos(self,dummy):
        """
        Updates position of this node
        Sends new position information to neighbors
        """
        #print "Node ",self.get_id(),"in updater at time ",self.get_now()
        elapsed_time = simx.get_now() - self.last_update_time
        self.last_update_time = simx.get_now()
        self.loc  = (self.loc[0]+self.vel[0]*elapsed_time,
                     self.loc[1]+self.vel[1]*elapsed_time)
        Gpos[self.gnode_id] = self.loc
        local_pos[self.gnode_id] = self.loc


    def update_velocity(self,dummy):
        """
        updates velocity of this node based on neighbor positions
        """
        self.vel = (0.0,0.0)
        
        for neighbor in self.neighbors:
            self.neighbor_pos[neighbor] = Gpos[neighbor]

        for k,v in self.neighbor_pos.iteritems():
            (nx,ny) = v
            self.vel  = (self.vel[0] + nx - self.loc[0],
                          self.vel[1] + ny - self.loc[1])
        self.vel = (self.vel[0]/vel_scale_factor,self.vel[1]/vel_scale_factor)


class Updater(simx.Process):
    """
    A process that probes entities at a fixed interval. the executed method will
    calculate new entity position, and new entity velocity based on updated coordinates
    and send this to its neighbors
    """
    
    def run(self):
        while(simx.get_now() < end_time):
            simx.probe_entities('n',Node.update_pos,())
            simx.probe_entities('n',Node.update_velocity,())
            #send location info of nodes on this process to all other LPs
            if (simx.get_num_machines() > 1):
                for i in range(simx.get_num_machines()):
                    if i == simx.get_rank():
                        continue
                    simx.get_controller().send_info(LocListMsg(local_pos),min_delay,
                                                  ('!',i),eAddr_RECVR)
            self.sleep(update_interval)
            
            


class Plotter(simx.Process):
    """
    Draws the network and updates it every 'update_interval' time units
    """
    def __init__(self):
        import matplotlib.pyplot as plt
        fig = plt.figure(figsize=(8,8))
        fig.patch.set_facecolor('black')
        plt.xlim(-0.05,1.05)
        plt.ylim(-0.05,1.05)
        plt.axis('off')
        plt.ion()
        plt.show()
    
    def run(self):
        while(simx.get_now() < end_time):
            plt.clf()
            nx.draw_networkx_edges(G,Gpos,nodelist=[ncenter],alpha=0.4,edge_color='grey')
            nx.draw_networkx_nodes(G,Gpos,node_size=80,nodelist=p.keys(),node_color=p.values(),cmap=plt.cm.Reds_r)
            plt.xlim(-0.05,1.05)
            plt.ylim(-0.05,1.05)
            plt.axis('off')
            plt.draw()
            self.sleep(update_interval)


simx.init("consensus")
simx.set_end_time(end_time)
simx.set_min_delay(min_delay)
#simx.set_log_level("debug3")
simx.init_env()

for i in xrange(num_nodes):
    simx.create_entity(('n',i),Node,ent_data=({'gnode_id':i}))
    
simx.get_controller().install_service(Receiver,eAddr_RECVR)
simx.schedule_process(Updater())
#only rank 0 does the plotting
if simx.get_rank() == 0:
    simx.schedule_process(Plotter())
simx.run()
