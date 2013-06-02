#include <assert.h>
#include "ssfapi/outchannel.h"
#include "kernel/kernel_event.h"
#include "kernel/universe.h"
#include "kernel/timeline.h"

namespace minissf {

outChannel::outChannel(Entity* owner, VirtualTime delay) :
  portno(0), entity_owner(owner), channel_delay(delay)
{
  if(!Universe::is_initializing()) 
    SSF_THROW("only allowed to be created during simulation initialization");
  if(!entity_owner) SSF_THROW("null entity owner");
  if(channel_delay < 0) SSF_THROW("negative channel delay: " << channel_delay);
  entity_owner->add_outchannel(this);
}

outChannel::~outChannel()
{
  if(!Universe::is_finalizing()) 
    SSF_THROW("can only be deleted during simulation finalization");
  for(DEQUE(MapOutport*)::iterator iter = outports.begin();
      iter != outports.end(); iter++) delete (*iter);
  outports.clear();
  entity_owner->delete_outchannel(this);
}

void outChannel::write(Event*& evt, VirtualTime write_delay)
{
  if(write_delay < 0) SSF_THROW("negative write delay: " << write_delay);
  if(!evt) SSF_THROW("null event");

  if(Universe::is_initializing())
    entity_owner->init_write_event(evt, write_delay, this);
  else if(Universe::is_running())
    write_event(evt, write_delay);
  else SSF_THROW("can't write event when the simulation is finalizing"); 

  // explicitly sever the reference so that the user will not access it!
  evt = 0;
}

void outChannel::write_event(Event* evt, VirtualTime write_delay)
{
  assert(Universe::is_running());
  // reference scheme so that we create the same number of clones of
  // the user event as there are needed; we assume that
  // Timeline::insert_event() and Stargate::send_message() do not
  // alter the event (the event is cloned after insert_event and
  // send_message
  int refs = 0; 
  for(DEQUE(MapOutport*)::iterator iter = outports.begin();
      iter != outports.end(); iter++) {
    // there is one outport corresponding to each target timeline
    MapOutport* outport = *iter;
    if(outport->inport) { 
      // if not null, the target timeline is on the same machine,
      // although it could be on a different processor
      MapInport* inport = outport->inport; 
      assert(inport && inport->extra_delay == 0);
      ChannelEvent* chevt = new ChannelEvent
	(this, owner()->now()+write_delay+outport->min_offset, 
	 (refs++>0)?evt->clone():evt, inport);
      if(outport->stargate) { 
	// if not null, we send an event to a different timeline
	Timeline* source_timeline = outport->stargate->source_timeline;
	assert(entity_owner->timeline == source_timeline);
	Timeline* target_timeline = outport->stargate->target_timeline;
	assert(inport->ic->entity_owner->timeline == target_timeline);
	assert(source_timeline != target_timeline);
	if(target_timeline->universe == source_timeline->universe) {
	  // the target timeline is in the same universe; we can
	  // directly insert the event into the target timeline
	  source_timeline->record_stats_local_messages();
	  target_timeline->insert_event(chevt);
	} else {
	  // the target timeline is in a different universe on the
	  // same machine; we put in the stargate's mailbox
	  outport->stargate->send_message(chevt);
	}
      } else { // we send an event to the same timeline
	entity_owner->timeline->record_stats_local_messages();
	entity_owner->timeline->insert_event(chevt);
      }
    } else { // the target timeline is on another machine
      ChannelEvent* chevt = new ChannelEvent
	(this, owner()->now()+write_delay+outport->min_offset, 
	 (refs++>0)?evt->clone():evt, portno);
      assert(outport->stargate);
      outport->stargate->send_message(chevt);
    }
  }
  if(!refs) delete evt; // if the event is not being sent, reclaim it
}

void outChannel::mapto(inChannel* ic, VirtualTime map_delay)
{
  if(!Universe::is_initializing()) 
    SSF_THROW("only allowed during simulation initialization");
  if(!ic) SSF_THROW("null inchannel");
  if(map_delay < 0) SSF_THROW("negative map delay: " << map_delay);
  Universe::add_mapping(this, ic, map_delay);
}

void outChannel::mapto(const char* icname, VirtualTime map_delay)
{
  if(!Universe::is_initializing()) 
    SSF_THROW("only allowed during simulation initialization");
  if(!icname) SSF_THROW("empty name");
  if(map_delay < 0) SSF_THROW("negative map delay: " << map_delay);
  Universe::add_mapping(this, icname, map_delay);
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
