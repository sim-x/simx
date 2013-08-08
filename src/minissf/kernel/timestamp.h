// the timestamp type (with three keys) used by the eventlist managed
// within timeline; the header file can only be included by
// "ssfapi/virtual_time.h", which is included by "ssfapi/ssf_common.h"

#ifndef __MINISSF_TIMESTAMP_H__
#define __MINISSF_TIMESTAMP_H__

#ifndef __MINISSF_VIRTUAL_TIME_H__
#error "DO NOT INCLUDE THIS CLASS DIRECTLY"
#endif

namespace minissf {

// this is the key used by priority queue eventlist
class Timestamp {
 public:
  int64 key1; // virtual time
  uint32 key2; // entity serial number
  uint32 key3; // event id

  friend int operator < (const Timestamp& t1, const Timestamp& t2) 
  { return ((t1.key1 < t2.key1) || ((t1.key1 == t2.key1) && ((t1.key2 < t2.key2) || ((t1.key2 == t2.key2) && (t1.key3 < t2.key3))))); }
  friend int operator > (const Timestamp& t1, const Timestamp& t2) 
  { return ((t1.key1 > t2.key1) || ((t1.key1 == t2.key1) && ((t1.key2 > t2.key2) || ((t1.key2 == t2.key2) && (t1.key3 > t2.key3))))); }
  friend int operator <= (const Timestamp& t1, const Timestamp& t2) { return !(t1>t2); }
  friend int operator >= (const Timestamp& t1, const Timestamp& t2) { return !(t1<t2); }

  Timestamp() {}
  Timestamp(int64 k1, uint32 k2, uint32 k3) : key1(k1), key2(k2), key3(k3) {}
}; /*class Timestamp*/

}; /*namespace minissf*/

#endif /*__MINISSF_TIMESTAMP_H__*/

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
