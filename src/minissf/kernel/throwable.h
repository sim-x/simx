/*
 * \file throwable.h
 * \brief Header file for the SSF_THROW macro.
 *
 * This file is used internally, which defines the facility to nicely
 * throw an exception with a useful error message.
 */
#ifndef __MINISSF_THROWABLE_H__
#define __MINISSF_THROWABLE_H__

#include <iostream>
#include <sstream>
#include <stdlib.h>

#if defined(__GNUC__) && (__GNUC__ >= 2)
#define __func__ __PRETTY_FUNCTION__
#else
#define __func__ "<unknown_function>"
#endif

/*
 * \brief Print a message and throw an exception.
 *
 * This macro is used by minissf internally to print a message and
 * throw an exception to abort the program. The message is actually a
 * string stream, so the user can pipe it conveniently. The caller's
 * file location (including file name, line number, and the enclosing
 * function name) is also added the message.
 * 
 * This macro is written by Nathanael Van Vorst.
 */
#define SSF_THROW(msg)  { minissf::ThrowableStream(__FILE__,__LINE__,__func__) << msg; }

namespace minissf {

/*
 * \brief Helper class for the SSF_THROW macro. 
 * 
 * This class supports piping a string stream as the error message. It
 * is used by minissf internally.
 */
class ThrowableStream {
 public:
  ThrowableStream(const char* filename, int line, const char* funcname) :
    ss(std::stringstream::in | std::stringstream::out) {
    ss << "ERROR: [" << funcname << " @ " << filename << ":" << line << "]\nERROR: ";
  }

  ThrowableStream(const ThrowableStream& other) :
    ss(std::stringstream::in | std::stringstream::out) {
    ss << other.ss.str();
  }

  ~ThrowableStream() { 
    std::cout << ss.str() << std::endl;
    exit(1); //throw ss.str().c_str();
  }

  ThrowableStream& operator<<(std::ostream& (*el)(std::ostream&)) {
    ss << *el;
    return *this;
  }
	
  template<class T>
  ThrowableStream& operator<<(T& val) {
    ss << val;
    return *this;
  }
	
  template<class T>
  ThrowableStream& operator<<(const T& val) {
    ss << val;
    return *this;
  }

 private:
  std::stringstream ss;
}; /*class ThrowableStream*/

}; /*namespace minissf*/

#endif /*__MINISSF_THROWABLE_H__*/

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
