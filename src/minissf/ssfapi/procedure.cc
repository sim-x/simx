#include <assert.h>
#include <string.h>
#include "ssfapi/procedure.h"

namespace minissf {

Procedure::Procedure(ProcedureFunction function, ProcedureContainer* container, 
		     void* retaddr, int retsize) :
  process_owner(0), parent_procedure(0), procedure_function(function), 
  procedure_container(container), entry_point(0), 
  procedure_retaddr(retaddr), procedure_retsize(retsize) 
{
  // if both container and function are null, which means the
  // procedure is process' action() method; if both container and
  // function are not null, it means we explicitly identify the
  // procedure to a generic procedure container method
  if((!function && container) || (function && !container))
    SSF_THROW("invalid process procedure");
}

Procedure::~Procedure() {}

void Procedure::call_procedure(Procedure *proc)
{
  // put the procedure into the call chain
  assert(process_owner);
  proc->process_owner = process_owner;
  proc->parent_procedure = this;

  // push the procedure on top of the stack
  assert(process_owner->process_stack == this);
  process_owner->process_stack = proc;

  // call the function
  process_owner->activate_procedure();
}

bool Procedure::call_suspended()
{
  assert(process_owner);
  return process_owner->process_state != Process::PROCESS_STATE_RUNNING;
}

void Procedure::call_return(void* retval)
{
  // copy the return value to parent's address space
  if(retval && procedure_retaddr) {
    assert(procedure_retsize > 0);
    memcpy(procedure_retaddr, retval, procedure_retsize);
  }

  if(parent_procedure) {
    // if it's in a sub-procedure, simply pop the stack and reclaim
    // the current procedure frame
    assert(process_owner);
    process_owner->process_stack = parent_procedure;
    delete this;
  } else {
    // if it's the starting procedure, just reset the entry point so
    // that the process would restart from the beginning
    entry_point = 0;
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
