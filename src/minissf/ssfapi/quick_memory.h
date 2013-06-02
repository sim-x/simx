/**
 * \file quick_memory.h
 * \brief Header file for quick memory management.
 *
 * Quick memory is a memory management layer that provides fast memory
 * allocation and deallocation services at each processor. The speed
 * comes at the cost of additional memory consumption due to
 * fragmentation. The memory blocks are chosen from free memory chunks
 * with sizes rounded up to the power of two.
 *
 * The header file also contains the definition of the QuickObject
 * class; Users do not need to include this header file directly; it
 * is included from ssf.h.
 */

#ifndef __MINISSF_QUICK_MEMORY_H__
#define __MINISSF_QUICK_MEMORY_H__

#include <stdio.h>

namespace minissf {

// initialize quick memory at the beginning of simulation; this
// function must be called by each processor; not to be called by user
extern void ssf_quickmem_init(int pid);

// finalize quick memory in the end of simulation; again, called by
// each processor; not to be called by user
extern void ssf_quickmem_wrapup(int pid);

/** 
 * \brief Allocate memory of the given size from quick memory.
 *
 * Calling this function is expected to be faster than calling
 * malloc() or new(). A memory allocated from quick memory should not
 * be deallocated using free() or delete(). The user should use
 * ssf_quickmem_free() to reclaim it. Alternatively, the user can use
 * the QuickObject class for convenience.
 *
 * \param size the size of the memory block (in bytes) to be allocated
 * \returns points to the memory block newly allocated
 */
extern void* ssf_quickmem_malloc(size_t size);

/**
 * \brief Deallocate memory and return it to quick memory.
 *
 * Calling this function is expected to be faster than calling free()
 * or delete(). One can deallocate a memory block using this function
 * only if the memory block is allocated from quick memory using
 * ssf_quickmem_malloc().
 *
 * \param p points to the memory block to be reclaimed to quick memory.
 */
extern void ssf_quickmem_free(void* p);

/**
 * \brief Fast memory allocation and deallocation services.
 *
 * Quick memory is a memory management layer that provides faster
 * memory allocation and deallocation services with some additional
 * memory overhead due to fragmentation.
 *
 * This class is expected to be used as the base class of all objects
 * that require fast memory allocation and deallocation. This class
 * overloads the new and delete operators, and provides two other
 * static functions for allocating and deallocating quick memory. The
 * user can simply derive from this class to enjoy the quick memory
 * service.
 */
class QuickObject {
 public:
  /**
   * \brief Overload the default new operator.
   *
   * The new operator is called whenever an object of this class or
   * its derived class needs to to be created dynamically. The memory
   * block is allocated from the quick memory.
   *
   * \param sz the size of the object to be created (which is provided by C++ when an object is created)
   * \returns a pointer to the allocated memory
   */
  static void* operator new(size_t sz) { return ssf_quickmem_malloc(sz); }

  /**
   * \brief Overload the default delete operator.
   *
   * The delete operator is called whenever an object of this class or
   * its derived class is reclaimed. The memory block is returned to
   * the quick memory.
   *
   * \param p points to the memory block to be reclaimed
   */
  static void operator delete(void* p) { ssf_quickmem_free(p); }

 public:
  /**
   * \brief Allocate quick memory of given size.
   *
   * This static method is provided to the user to obtain a quick
   * memory block of the given size. The quick memory block can only
   * be reclaimed using the correponding quick_delete() method.
   *
   * \param sz the size of the memory block
   * \returns a pointer to the allocated quick memory block
   */
  static void* quick_new(size_t sz) { return ssf_quickmem_malloc(sz); }

  /**
   * \brief Deallocate quick memory.
   *
   * This static method is provided to the user to reclaim a quick
   * memory block. The memory block must be allocated using the
   * quick_new() method in the first place.
   *
   * \param p points to the memory block to be reclaimed
   */
  static void quick_delete(void* p) { ssf_quickmem_free(p); }
}; /*class QuickObject*/

}; /*namespace minissf*/

#endif /*__MINISSF_QUICK_MEMORY_H__*/

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
