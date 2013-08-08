// stargate is the communication portal between timelines

#ifndef __MINISSF_STARGATE_H__
#define __MINISSF_STARGATE_H__

#include "ssfapi/ssf_common.h"
#include "kernel/ssfmachine.h"
#include "kernel/kernel_event.h"

namespace minissf {

class MapInport;
class MapOutport;
class Stargate;

// CHEAT SHEET:
// * The mapping structure is arranged as follows:
//   1) oc has a unique identifier (called outport number)
//   2) oc maintains a list of outports, DEQUE(MapOutport*), one for each target timeline 
//      (including itself if wired so)
//   3) oc->outports[timeline] points MapOutport, which has:
//      a) a pointer to stargate (if the source and target timelines are not the same); 
//         the stargate is used for timeline (or LP) scheduling purposes
//      b) a pointer to inport (MapInport), if it's on the same machine (maybe belonging
//         to different universe than the source timeline's.
//   4) MapInport is itself a linked list, there's one node per mapped inchannel at the 
//      same timeline
//   5) the stargate is used to represent the link between different timelines
//
// * The delays are arranged as follows:
//   oc->channel_delay (channel delay)
//   oc->outports[timeline]->min_delay (min map_delay among all that mapped 
//     to the same target timeline)
//   oc->outports[timeline]->inport->extra_delay, or
//   oc->outports[timeline]->stargate->inports[oc->portno]->extra_delay 
//     (linked list and sorted incrementally; it's the EXTRA delay after the 
//      previous node)
//  oc->outports[timeline]->stargate->min_delay is the min delay for all mapping from 
//    the source timeline to the target timeline

// this data structure represents a mapping originating from an
// outchannel at the sending timeline; there are as many outports as
// the target timelines
class MapOutport {
 public:
  Stargate* stargate; // if not null, this is the stargate between the timelines
  MapInport* inport; // points to the inport if the target timeline is on the same machine
  VirtualTime min_offset; // minimum channel delay plus map delay
  MapOutport(Stargate* sg, MapInport* ip, VirtualTime md);
  ~MapOutport();
}; /*class MapOutport*/

// this data structure represents the ending of a map to one or more
// inchannels at the target timeline
class MapInport {
 public:
  inChannel* ic; // points to the inchannel
  VirtualTime extra_delay; // additional delay to MapOutport::min_offset
  MapInport* next; // next mapped inchannel in the linked list
  MapInport(inChannel* ic);
  ~MapInport();
}; /*class MapInport*/

// this is a directed edge in the LP graph
class Stargate {
 public:
  // the constructor for the case that both ends are on the same
  // machine (not necessarily on the same processor)
  Stargate(Timeline* src, Timeline* tgt);

  // the constructor for the sending side of a stargate that connects
  // to the target timeline on a remote machine
  Stargate(Timeline* src, int tgtid, int outportno);

  // the constructor for the receiving end of a stargate that links
  // from a source time on a remote machine
  Stargate(int srcid, int outportno, Timeline* tgt);

  // the destructor
  virtual ~Stargate();

  // register the delay for all mappings between the two designated
  // timelines; we select the minimum as lookahead
  void set_delay(VirtualTime delay);

  // the stargate keeps the timestamp lower bound of future events
  // that will be sent across the channel; the boolean
  // called_by_sender is used to differentiate whether set_time() is
  // called by the source timeline (by the
  // Timeline::update_subsequent_timelines() method), or the target
  // universe (by the Universe::handle_io_events() method) upon
  // receiving a null message
  void set_time(VirtualTime t, bool called_by_sender);
  inline VirtualTime get_time() { return time; }

  // deliver and retrieve messages across the channel
  void send_message(ChannelEvent* evt);
  void receive_messages();

 protected:
  Timeline* source_timeline; // null if not in this address space
  Timeline* target_timeline; // null if not in this address space
  int source_timeline_id; // the serial number of the source timeline
  int target_timeline_id; // the serial number of the target timeline
  int outportno; // id for one of the outchannels mapped through this stargate (used for connecting remote machine)
  bool in_sync; // true if this link is synchronous (for composite synchronization)
  VirtualTime min_delay; // min among all channel mappings from the source to the target timeline
  VirtualTime time; // time of this channel (updated by the source timeline)
  ssf_thread_mutex_t mailbox_mutex; // protect simultaneous access to the mailbox
  ChainedEvent* mailbox; // a linked list of channel events sent from source to target timeline
  ChainedEvent* mailbox_tail; // points to the end of the linked list for appending new events

  void constructor(); // do the common work for all constructors

  friend class outChannel;
  friend class Timeline;
  friend class Universe;
}; /*class Stargate*/

}; /*namespace minissf*/

#include "kernel/timeline.h"

#endif /*__MINISSF_STARGATE_H__*/

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
