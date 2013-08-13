#include <assert.h>
#include "ssfapi/event.h"

namespace minissf {

SET(STRING)* Event::registered_event_names = 0;
VECTOR(EventFactory)* Event::registered_event_factories = 0;

// all event classes must be registered including the base class
SSF_REGISTER_EVENT(Event, Event::create_baseclass_event);

Event* Event::clone() { return new Event(*this); }

int Event::register_event(const char* classname, EventFactory factory)
{
  if(!classname) SSF_THROW("missing event class name for event registration");
  if(!factory) SSF_THROW("missing event factory method for event registration");

  if(!registered_event_names) {
    registered_event_names = new SET(STRING);
    registered_event_factories = new VECTOR(EventFactory);
  }
  if(!registered_event_names->insert(classname).second)
    SSF_THROW("duplicate event class: " << classname);
  registered_event_factories->push_back(factory);
  return registered_event_factories->size(); // start from one!
}

Event* Event::create_baseclass_event(char* buf, int siz) 
{
  assert(siz == 0); // we don't have data for the base class
  return new Event();
}

Event* Event::create_registered_event(int id, char* buf, int siz) 
{
  if(registered_event_factories && 0 < id && 
     id <= (int)registered_event_factories->size()) {
    return ((*registered_event_factories)[id-1])(buf, siz);
  } else return 0;
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
