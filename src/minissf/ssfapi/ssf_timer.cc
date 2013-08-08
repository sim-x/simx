#include <assert.h>
#include "ssfapi/ssf_timer.h"

namespace minissf {

Timer::Timer(Entity *owner, void (Entity::*cb)(Timer*)) :
  entity_owner(owner), timer_entity_callback(cb), timer_callback(0),
  fire_time(0), timer_event(0) 
{
  if(!owner) SSF_THROW("null entity owner");
  if(!cb) SSF_THROW("null callback function");
}

Timer::Timer(Entity *owner, void (*cb)(Timer*)) :
  entity_owner(owner), timer_entity_callback(0), timer_callback(cb),
  fire_time(0), timer_event(0) 
{
  if(!owner) SSF_THROW("null entity owner");
  if(!cb) SSF_THROW("null callback function");
}

Timer::Timer(Entity *owner) :
  entity_owner(owner), timer_entity_callback(0), timer_callback(0),
  fire_time(0), timer_event(0) 
{
  if(!owner) SSF_THROW("null entity owner");
}

Timer::~Timer() { cancel(); }

void Timer::schedule(VirtualTime delay) 
{
  if(delay < 0) SSF_THROW("negative delay: " << delay);
  fire_time = entity_owner->now()+delay;
  if(timer_event) {
    SSF_THROW("attempt to schedule a running timer; use reschedule instead");
  } else {
    timer_event = new TimerEvent(fire_time, this);
    entity_owner->insert_event(timer_event);
  }
}

void Timer::reschedule(VirtualTime delay)
{
  if(delay < 0) SSF_THROW("negative delay: " << delay);
  fire_time = entity_owner->now()+delay;
  if(timer_event) {
    timer_event->setTime(fire_time); // this will trigger adjustment in the eventlist
  } else {
    timer_event = new TimerEvent(fire_time, this);
    entity_owner->insert_event(timer_event);
  }
}

void Timer::cancel()
{
  if(timer_event) {
    entity_owner->cancel_event(timer_event); // will delete this event
    timer_event = 0;
  } // otherwise, it's a no-op
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
