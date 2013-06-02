// binque is a calendar queue tailored for composite
// synchronization. There is one such bin queue for each processor
// (universe) for storing future events (beyond the current
// synchronization window). The size of the bins is the same as the
// size of the synchronization window.

#ifndef __MINISSF_BINQUE_H__
#define __MINISSF_BINQUE_H__

#include "ssfapi/ssf_common.h"
#include "kernel/kernel_event.h"

namespace minissf {

class BinQueue {
public:
  // the constructor
  BinQueue();

  // when it's time, construct the binque permanently
  void settle(VirtualTime binsize, int nbins, VirtualTime now);

  // the destructor
  ~BinQueue();

  // insert the event into the appropriate bin
  void insert_event(ChannelEvent* evt);

  // retrieve all events (as a linked list) from the bins below the given time
  ChannelEvent* retrieve_events(VirtualTime upper_time);

private:
  VirtualTime binsize; // size of the bin (in virtual time)
  int nbins; // number of bins for the calendar queue
  int curbin; // the current bin for processing
  VirtualTime lower_edge; // lower window edge of the current bin
  VirtualTime offset; // it may not start from time 0

  ChannelEvent** bin_array; // the calendar queue itself
  ChannelEvent* tmp_holder; // used before the binque is settled
  KernelEventList splay; // events far into the future is stored here
}; /*BinQueue*/

}; // namespace minissf

#endif /*__MINISSF_BINQUE_H__*/

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
