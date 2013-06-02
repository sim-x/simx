#include <assert.h>
#include "ssfapi/entity.h"
#include "kernel/universe.h"

namespace minissf {

Entity::Entity(bool emulation) : 
  timeline(0), emulated(emulation), serialno(0), nxtevtid(0) 
{
  // the entity's timeline and serial number are not yet settled at this moment
  if(!Universe::is_initializing()) 
    SSF_THROW("entity can only be created during simulation initialization");
  Universe::register_orphan_entity(this);
}

Entity::~Entity()
{
  if(!Universe::is_finalizing()) 
    SSF_THROW("entity can only be deleted during simulation finalization");
  
  while(!processes.empty()) {
    SET(Process*)::iterator iter = processes.begin();
    assert(iter != processes.end());
    delete (*iter);
  }
  processes.clear();

  while(!inchannels.empty()) {
    SET(inChannel*)::iterator iter = inchannels.begin();
    assert(iter != inchannels.end());
    delete (*iter);
  }
  inchannels.clear();

  while(!outchannels.empty()) {
    SET(outChannel*)::iterator iter = outchannels.begin();
    assert(iter != outchannels.end());
    delete (*iter);
  }
  outchannels.clear();

  for(SET(KernelEvent*)::iterator e_iter = init_events.begin();
      e_iter != init_events.end(); e_iter++) delete (*e_iter); 
  init_events.clear();

  for(SET(InitWrite*)::iterator  w_iter = init_writes.begin();
      w_iter != init_writes.end(); w_iter++) {
    delete (*w_iter)->evt; delete (*w_iter);
  }
  init_writes.clear();

  if(timeline) timeline->delete_entity(this);
}

VirtualTime Entity::now() const
{
  if(timeline) return timeline->now();
  else return 0;
}

void Entity::alignto(Entity* entity)
{
  if(!Universe::is_initializing())
    SSF_THROW("alignment allowed during simulation initialization");
  if(!entity) SSF_THROW("null entity");

  if(timeline && !entity->timeline) {
    timeline->add_entity(entity);
    Universe::deregister_orphan_entity(entity);
  } else if(!timeline && entity->timeline) {
    entity->timeline->add_entity(this);
    Universe::deregister_orphan_entity(this);
  } else if(timeline && entity->timeline) {
    timeline->merge_timeline(entity->timeline);
  } else {
    Timeline* tmln = Universe::create_timeline();
    tmln->add_entity(entity);
    tmln->add_entity(this);
    Universe::deregister_orphan_entity(entity);
    Universe::deregister_orphan_entity(this);
  }
}

const SET(Entity*)& Entity::coalignedEntities()
{
  if(!timeline) {
    Timeline* tmln = Universe::create_timeline();
    tmln->add_entity(this);
    Universe::deregister_orphan_entity(this);
  } 
  assert(timeline);
  return timeline->get_entities(); 
}

void Entity::add_inchannel(inChannel* ic) {
  inchannels.insert(ic); 
  if(!ic->name.empty()) 
    Universe::register_named_inchannel(ic);
}

void Entity::delete_inchannel(inChannel* ic) { inchannels.erase(ic); }

void Entity::add_outchannel(outChannel* oc) { outchannels.insert(oc); }

void Entity::delete_outchannel(outChannel* oc) { outchannels.erase(oc); }

void Entity::add_process(Process* p) { processes.insert(p); }

void Entity::delete_process(Process* p) { processes.erase(p); }

void Entity::insert_event(KernelEvent* evt)
{
  if(Universe::is_initializing()) { // store locally
    init_events.insert(evt);
  } else {
    assert(timeline);
    timeline->insert_event(evt);
  }
}

void Entity::cancel_event(KernelEvent* evt)
{
  if(Universe::is_initializing()) { // store locally
    init_events.erase(evt);
  } else {
    assert(timeline);
    timeline->cancel_event(evt);
  }
}

void Entity::init_write_event(Event* evt, VirtualTime write_delay, outChannel* oc) 
{
  init_writes.insert(new InitWrite(evt, write_delay, oc));
}

void Entity::schedule_init_events()
{
  for(SET(KernelEvent*)::iterator e_iter = init_events.begin();
      e_iter != init_events.end(); e_iter++)
    timeline->insert_event(*e_iter);
  init_events.clear();
  for(SET(InitWrite*)::iterator  w_iter = init_writes.begin();
      w_iter != init_writes.end(); w_iter++) {
    (*w_iter)->oc->write_event((*w_iter)->evt, (*w_iter)->wdelay);
    delete (*w_iter);
  }
  init_writes.clear();
}

}; /*namespace minissf*/

/*
 * Copyright (c) 2011-2013 Florida International University.
 *
 * Permission is hereby granted, free of charge, to any individual or
 * institution obtaining a copy of this software and associated
 * documentation files (the "software"), to use, copy, modify, and
 * distribute without restriction.
 *
 * The software is provided "as is", without warranty of any kind,
 * express or implied, including but not limited to the warranties of
 * merchantability, fitness for a particular purpose and
 * noninfringement.  In no event shall Florida International
 * University be liable for any claim, damages or other liability,
 * whether in an action of contract, tort or otherwise, arising from,
 * out of or in connection with the software or the use or other
 * dealings in the software.
 *
 * This software is developed and maintained by
 *
 *   Modeling and Networking Systems Research Group
 *   School of Computing and Information Sciences
 *   Florida International University
 *   Miami, Florida 33199, USA
 *
 * You can find our research at http://www.primessf.net/.
 */
