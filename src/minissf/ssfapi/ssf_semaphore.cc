#include "ssfapi/ssf_semaphore.h"
#include "ssfapi/entity.h"
#include "ssfapi/process.h"
#include "kernel/kernel_event.h"

namespace minissf {

Semaphore::Semaphore(Entity* owner, int initval) :
  entity_owner(owner), sem_count(initval) 
{
  if(!owner) SSF_THROW("null entity owner");
}

Semaphore::~Semaphore() 
{
  // one can only delete a semaphore after the simulation is done for
  if(!Universe::is_finalizing()) 
    SSF_THROW("can only be deleted during simulation finalization");
  blocked_processes.clear(); // the block processes will be reclaimed by entity
}

void Semaphore::wait()
{
  Process* p = entity_owner->timeline->current_process();
  if(!p) SSF_THROW("must be used within process context");
  PROPER_PROCEDURE(p);

  sem_count--; 
  if(sem_count < 0) {
    // get the process off the running queue
    p->suspend_process();
    // add calling process to the end of the semaphore waiting list
    blocked_processes.push_back(p);
  }
}

void Semaphore::signal() 
{
  // if there is at least one process blocked (regardless of the
  // value), release the one at the head of the linked list
  sem_count++;
  if(!blocked_processes.empty()) {
    Process *release_process = blocked_processes.front();
    blocked_processes.pop_front();
    SemaphoreEvent* evt = new SemaphoreEvent(release_process);
    entity_owner->insert_event(evt);
  }
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
