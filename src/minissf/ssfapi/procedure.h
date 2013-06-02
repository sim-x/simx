/**
 * \file procedure.h
 * \brief Header file for the procedure and procedure container classes.
 *
 * This header file contains the definition of the procedure and
 * procedure container classes; Users do not need to include this
 * header file directly; it is included from ssf.h.
 */

#ifndef __MINISSF_PROCEDURE_H__
#define __MINISSF_PROCEDURE_H__

#include "ssfapi/ssf_common.h"
#include "ssfapi/quick_memory.h"

namespace minissf {

class ProcedureContainer;

/**
 * \brief Prototype of a function to be used as a starting procedure.
 *
 * A starting procedure is the first function of a process. The user
 * must provide a pointer to the starting procedure when creating a
 * process. The only exception is that a starting procedure can be the
 * Process::action() method, in which case one does not need to
 * explicitly pass a pointer to the process to the starting procedure.
 *
 * @see Process constructors for details on how it is used.
 */
typedef void (ProcedureContainer::*ProcedureFunction)(Process*);

/** 
 * \brief Base class for all that contain procedures.
 *
 * A method of this class and its derived classes can be used as a
 * procedure. A procedure is a part of a simulation process; it's a
 * function that can be blocked during the execution of its body, by
 * waiting on an inchannel, waiting for a period of simulation time to
 * pass, or calling another procedure which does the same. Both Entity
 * and Process classes are derived from this class since their methods
 * can be used as procedures. User can define a class with methods
 * used as procedures as long as it's an subclass of this class.
 */
class ProcedureContainer {
 public:
  /** \brief The constructor of a procedure container. */
  ProcedureContainer() {}

  /** \brief The destructor of a procedure container. */
  virtual ~ProcedureContainer() {}
}; /*class ProcedureContainer*/

/*
 * \brief Procedures as stack frames of a process.
 *
 * A procedure is the data structure that represents the stack frame
 * of a function invocation in a process. During the source code
 * translation, each identified procedure will have a procedure class
 * derived from this base class, which contains arguments and local
 * variables of the corresponding function. It also maintains
 * information including the function pointer, program counter, return
 * address, among other things.
 */
class Procedure : public QuickObject {
 public:
  // the contructor
  Procedure(ProcedureFunction function, ProcedureContainer* container, 
	    void* retaddr = 0, int retsize = 0);

  // The destructor
  virtual ~Procedure();

  // push the procedure frame onto the stack and invoke the procedure 
  void call_procedure(Procedure* proc);
  
  // whether the process already suspended on a wait call
  bool call_suspended();

  // install the return value into the parent's procedure frame
  void call_return(void* retval = 0);

 public:
  Process* process_owner; // a procedure belongs to a process
  Procedure* parent_procedure; // parent (or calling) procedure in the stack
  ProcedureFunction procedure_function; // procedure function
  ProcedureContainer* procedure_container; // container object that owns the procedure function
  int entry_point; // program counter remembers where the procedure needs to resume execution
  void* procedure_retaddr; // address to hold the return value in the parent procedure
  int procedure_retsize; // size of the return value
}; /*class Procedure*/

}; /*namespace minissf*/

#include "ssfapi/process.h"

#endif /*__MINISSF_PROCEDURE_H__*/

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
