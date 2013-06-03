#include <assert.h>
#include <string.h>
#include "kernel/binque.h"
#include "kernel/universe.h"

namespace minissf {

BinQueue::BinQueue() : bin_array(0), tmp_holder(0) {}

void BinQueue::settle(VirtualTime bs, int nb, VirtualTime now)
{
  binsize = bs;
  nbins = nb; assert(nbins > 0);
  offset = now;

  curbin = 0;
  lower_edge = 0;
  bin_array = new ChannelEvent*[nb]; assert(bin_array);
  memset(bin_array, 0, nb*sizeof(ChannelEvent*));

  /*
  printf("settle binque: binsize=%lg, nbins=%d, offset=%lg, lower_edge=%lg\n", 
	 binsize.second(), nbins, offset.second(), lower_edge.second());
  */
}

BinQueue::~BinQueue()
{
  while(tmp_holder) {
    ChannelEvent* nxt = (ChannelEvent*)tmp_holder->get_next_event();
    delete tmp_holder;
    tmp_holder = nxt;
  }

  if(bin_array) {
    for(int i=0; i<nbins; i++) {
      ChannelEvent* e = bin_array[i];
      while(e) {
	ChannelEvent* nxt = (ChannelEvent*)e->get_next_event();
	delete e;
	e = nxt;
      }
    }
    delete[] bin_array;
  }
}

void BinQueue::insert_event(ChannelEvent* evt)
{
  assert(evt);
  if(!bin_array) { // before settlement
    evt->get_next_event() = tmp_holder;
    tmp_holder = evt;
  } else {
    VirtualTime key = evt->time()-offset;
    VirtualTime next_lower_edge = lower_edge+binsize;
    assert(lower_edge <= key);
    if(key < lower_edge+binsize*nbins) { // not too far into the future
      int n = key.get_ticks()/binsize.get_ticks()%nbins;
      assert(0 <= n && n < nbins);
      evt->get_next_event() = bin_array[n];
      bin_array[n] = evt;
      //printf("%d: insert evt %lg into bin %d (binsize=%lg)\n", Universe::args_rank, key.second(), n, binsize.second());
    } else {
      splay.insert(evt);
      //printf("%d: insert evt %lg into splay tree (binsize=%lg)\n", Universe::args_rank, key.second(), binsize.second());
    }
  }
}

ChannelEvent* BinQueue::retrieve_events(VirtualTime upper_time)
{
  if(!bin_array) { // before settlement
    ChannelEvent* evt = tmp_holder;
    tmp_holder = 0;
    return evt;
  } else {
    // get the next binful of events from the calendar queue
    upper_time -= offset;
    if(lower_edge >= upper_time) {
      // it's possible this happens due to readjustment of the local
      // upper window edge, simply return null
      return 0;
    }
    // four cases to follow!
    ChannelEvent* evtlist;
    VirtualTime next_lower_edge = lower_edge+binsize;
    if(upper_time < next_lower_edge) {
      // return the current bin, maintain the current bin
      evtlist = bin_array[curbin];
      bin_array[curbin] = 0;
    } else if(upper_time == next_lower_edge) {
      // return the current bin, advance the current bin
      evtlist = bin_array[curbin];
      bin_array[curbin] = 0;
      curbin = (curbin+1)%nbins;
      lower_edge = next_lower_edge;
    } else {
      // curbin may be skipped only once, at the every beginning (when
      // no training of thresholds is needed); keep in mind this is
      // tightly dependent on the logic of the main event loop
      assert(bin_array[curbin] == 0);
      // advance current bin
      curbin = (curbin+1)%nbins;
      lower_edge = next_lower_edge;
      next_lower_edge += binsize;
      if(upper_time < next_lower_edge) {
	// return the current bin (advanced already), keep it unchanged
	evtlist = bin_array[curbin];
	bin_array[curbin] = 0;
      } else {
	// return the current bin (advanced already), advance it further
	assert(upper_time == next_lower_edge);
	evtlist = bin_array[curbin];
	bin_array[curbin] = 0;
	curbin = (curbin+1)%nbins;
	lower_edge = next_lower_edge;
      }
    }

    //printf("%d: retrieve evts from bin %d (l=%lg, u=%lg, binsize=%lg)\n", 
    //	 Universe::args_rank, curbin, lower_edge.second(), upper_time.second(), binsize.second());

    // look for events on splay tree as well
    while(splay.size() > 0) {
      ChannelEvent* evt = (ChannelEvent*)splay.getMin();
      if(evt->time() < upper_time) {
	splay.deleteMin();
	evt->get_next_event() = evtlist;
	evtlist = evt;
      } else break;
    }

    // if we still have events from the unsettled period (during
    // training of the thresholds), prepend them all, as well
    while(tmp_holder) {
      ChannelEvent* evt = tmp_holder;
      tmp_holder = (ChannelEvent*)evt->get_next_event();
      evt->get_next_event() = evtlist;
      evtlist = evt;
    }

    return evtlist;
  }
}

}; // namespace minissf

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
