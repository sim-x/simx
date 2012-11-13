import sys
import cPickle
import inspect



import simx

from cStringIO import StringIO

class DebugStream():
    def __init__(self,stream_type):
        self.stream_type = stream_type
        self.debug_str = StringIO()
        # define a logger function dictionary here    
        self.logger = {'debug1':simx.debug1,
                       'debug2':simx.debug2,
                       'debug3':simx.debug3,
                       'info':simx.debug_info,
                       'warn':simx.warn,
                       'error':simx.error,
                       'failure':simx.failure }

     # define debug stream writer method   
    def write(self,*message):
        for token in message:
            self.debug_str.write(str(token));
            self.debug_str.write(" ");
        self.logger[self.stream_type](self.debug_str.getvalue())
        self.debug_str.truncate(0)

#create debug streams here
debug1 = DebugStream('debug1')
debug2 = DebugStream('debug2')
debug3 = DebugStream('debug3')
info = DebugStream('info')
error = DebugStream('error')
warn = DebugStream('warn')
failure = DebugStream('failure')

class OutputStream():
    def __init__(self):
        self.output_str = StringIO()

    # define output stream writer method   
    # sim_obj should be derived from either PyEntity or PyService    
    def write(self,sim_obj,record_type,*message):
        for token in message:
            self.output_str.write(str(token));
            self.output_str.write(" ");
        simx.output(sim_obj,record_type,self.output_str.getvalue())    
        self.output_str.truncate(0)

output = OutputStream()

simx.init_mpi("HelloWorld")
#simx.init_config("hello.cfg")
simx.set_config_value("NUMBER_LPS","0")
simx.set_config_value("MINDELAY","1")
simx.set_config_value("END_TIME","1000000")
simx.set_config_value("OUTPUT_FILE","output_HelloWorld.out")


# simx.set_config_value("SERVICE_FILES","TestData/service_HelloWorld.dat")
# simx.set_config_value("ENTITY_FILES","TestData/person_HelloWorld.dat")
simx.set_config_value("INFO_FILES","TestData/info_HelloWorld.dat")

simx.set_config_value("LOG_COUT_LEVEL","warn")
simx.set_config_value("LOG_LEVEL","debug3")
simx.set_config_value("LOG_FILE","helloworld.log")

d = {'SERVICES':'eAddr_HelloHandlerPerson=eServ_HelloHandlerPerson'}
simx.create_profile("EntityProfile","10001",d)

# TODO: move class definition to separate module
# class ServiceData(object):
#     def __init__(self,name=None, class_type=None, profile_id=None,data=""):
#         self.name_ = name
#         self.type_ = class_type
#         self.profile_id_ = profile_id
#         self.data_ = data


services = []
# TODO: write an intermediate function that makes sure the passed-in data types
# are what C++ expects. Or better, export a pyServiceData class 
services.append(simx.ServiceData('eServ_HelloHandlerPerson','HelloHandlerPerson',1,[1,2]))

simx.init_main(1)
simx.prepare_services(services)

#ed1 = simx.EntityData(('p',0),'person',10001,'[(p 1)]')
#ed2 = simx.EntityData(('p',1),'person',10001,'[(p 0)]')

# create controller. end-user shouldn't have to remember to do this 
simx.create_controller()

class Person(simx.PyEntity):
    def __init__(self,ID,lp,entity_input):
        super(Person,self).__init__(ID,lp,entity_input,self)
        debug3.write("I'm a Python Person Entity with id", 
                     self.getId(),"created at time ",self.getNow())
        #print entity_input.data_
        print "person",id(self)
        self.create_services(entity_input)

    def say_hello(self,args=None):
        print "person ",self.getId()," says hello from python and args is ", args
        output.write(self,100,'this is a tuple',(1,2,3))

eAddr_HelloHandlerPerson = 11101;

simx.register_entity("person",Person)
simx.register_address("eAddr_HelloHandlerPerson",eAddr_HelloHandlerPerson)

class HelloMessage:
    def __init__(self,neighbors):
        self.neighbors = neighbors

class ReplyMessage:
    def __init__(self,data=None):
        self.data = data

    
#x = cPickle.dumps(ReplyMessage)

class HelloHandlerPerson(simx.PyService):
    def __init__(self,name, person, service_input ):
        print "XXXXXXX"
        print person.getId()
        print person.say_hello()
        print type(person)
        print type(person.__class__)
        print inspect.getmro(person.__class__)
        #print dir(person)
        #help(person.getId)
        #print type(entity)
        #print type(entity.__class__)
        #print inspect.getmro(entity.__class__)
        #print "entity id is ",entity.getId()
        #print "person id is ",person.getId()
        print "XXXXXXXX"
        #person = entity
        #print dir(entity)
        #print person.i_
        #person.say_hello()
        #debug3.write("I'm a Python HelloHandlerPerson service",name,
        #       " on person ",person.getId())
        super(HelloHandlerPerson,self).__init__( name, person, service_input,self )
        #quit()
        print service_input.data_   
        #print self.i_
        #self.i_ = 123
        #self.print_i()
        #print self.i_
        print "hellohandlerperson ",id(person)
        self.person = person
        print "YYYYY"
        print id(self.person)
        #person.say_hello()
        #print "self.person.i_ ",self.person.i_
        self.dummy = "sunil"
        #person.say_hello()
  
    def set_dummy(self,val):
        self.dummy = val


    def recv(self,msg,is_pickled):
        if is_pickled:
            msg = cPickle.loads(msg)
        #print id(self)
        print type(self.person)
        self.person.say_hello()
        print "inside recv: self.person.i_ ",self.person.i_    
        print "self.person id is ", self.person.getId()
        print "self.getEntity id is ",self.get_entity().getId()
        print "self.person c++ address is ",self.person.print_address()
        print "self.getEntity c++ address is ",self.get_entity().print_address()
        output.write(self,200,'this is service output')
        output.write(self,200,'this is random number',self.get_random().
                     get_uniform(1000000,2000000000) )
        hs = self.person.get_service(eAddr_HelloHandlerPerson)
        print type(hs)
        print hs.person.getId()
        hs = self.get_entity().get_service(eAddr_HelloHandlerPerson)
        print type(hs)
        print dir(hs)
        hs.print_i()
        print inspect.getmro(hs.__class__)
        print hs.person.getId()

        print hs.dummy
        print self.dummy
        hs.set_dummy("namu")
        print hs.dummy
        print self.dummy
        print id(self)
        print id(hs)

        ent = self.person
        print type(ent)
        print id(ent)
        ent = self.get_entity()
        print type(ent)

        #print self.person.getId()
        #print self.get_entity().getId()
      
        #s = cPickle.loads(x)
        #print s.__name__
        debug3.write("HelloHandlerPerson: Inside receive method")
        #print msg
        msg_type = get_msg_type(msg)
        if (msg_type == 'HelloMessage'):
            debug3.write("hello message received")
            neighbor = msg.neighbors[0]
            #neighbor = msg[1]
            self.send_info(ReplyMessage(range(10)),10,neighbor,eAddr_HelloHandlerPerson)
        elif ( msg_type == 'ReplyMessage'):
            debug3.write("reply message received")
            debug3.write("reply message data",msg.data)

def get_msg_type(msg):
    return msg.__class__.__name__
    #return msg[0]   
simx.register_service("HelloHandlerPerson",HelloHandlerPerson)
#simx.register_packer(cPickle.dumps)
#simx.register_unpacker(cPickle.loads)

#for i in range(1000000):
simx.create_entity( simx.EntityData(('p',0),'person',10001,[('p',1)]))
simx.create_entity( simx.EntityData(('p',1),'person',10001,[('p',0)]))


#for i in range(1):
import random
for i in range(1):
    j = random.choice([0,1])
    #j = 0
    simx.create_input_info( simx.InfoData(i,('p',j),eAddr_HelloHandlerPerson,1000,1,HelloMessage([('p',1-j)])))

# d1 = simx.debug1()
print simx.get_sim_phase()
print simx.get_processor_name()
simx.probe_entities('p',Person.say_hello,(1,))
simx.init_main(2)
