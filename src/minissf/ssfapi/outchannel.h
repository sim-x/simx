/**
 * \file outchannel.h
 * \brief Header file for the outchannel class.
 *
 * This header file contains the definition of the outchannel class;
 * Users do not need to include this header file directly; it is
 * included from ssf.h.
 */

#ifndef __MINISSF_OUTCHANNEL_H__
#define __MINISSF_OUTCHANNEL_H__

#include "ssfapi/ssf_common.h"
#include "kernel/stargate.h"

namespace minissf {

class MapOutport;

/**
 * \brief Outchannel as portal for sending events.
 *
 * An outchannel is the sending end of the communication link between
 * entities. Messages, known as events, are traveling from an
 * outchannel to all inchannels that are mapped to the outchannel. The
 * events sent from an outchannel experience delays before reaching a
 * mapped inchannel. The delay is the sum of the channel delay
 * (specified when the outchannel is created), a mapping delay
 * (specified when the outchannel is mapped to an inchannel), and a
 * per-write delay (specified when the event is written to the
 * outchannel).
 */
class outChannel {
 public:
  /**
   * \brief The constructor of the outchannel.
   *
   * The constructor must be called by passing the owner entity as its
   * argument.  The ownership of an outchannel is immutable. An
   * outchannel has a channel delay associated with it (which defaults
   * to be zero): any events written to the outchannel are delayed by
   * this value. 
   *
   * \param owner points to the owner entity of this outchannel.
   * \param channel_delay the fixed delay applied to all events sent from this channel (default is zero).
   */
  outChannel(Entity* owner, VirtualTime channel_delay = 0);

  /**
   * \brief The destructor of the outchannel.
   *
   * Detroying an outchannel means to separate the links between the
   * outchannel and all its mapped inchannels. Note that the user
   * shall not reclaim an outchannel directly: all outchannels are
   * reclaimed by the system when the owner entity is destroyed at the
   * end of the simulation.
   */
  virtual ~outChannel();

  /** \brief Return the entity owner of this outchannel.
   *
   * The entity owner is permanently set upon creation of the
   * outchannel. The ownership is immutable.
   */
  inline Entity* owner() { return entity_owner; }

  /**
   * \brief Map this outchannel to an inchannel.
   *
   * This method creates a mapping between this outchannel and the
   * specified inchannel. This method can only be called at the
   * simulation initialization stage. This method requires a pointer
   * to the inchannel, which means that the inchannel must have been
   * created in the same address space. A non-negative mapping delay
   * can be associated with each mapping. If the delay is not
   * specified, the mapping delay will default to zero. Note that SSF
   * requires that the sum of the channel delay and the mapping delay
   * must be strictly greater than zero if the mapping crosses
   * different timelines.
   *
   * \param ic points to the inchannel to be mapped to
   * \param map_delay the mapping delay to be added to the event's total transfer time
   */
  void mapto(inChannel* ic, VirtualTime map_delay = 0);

  /**
   * \brief Map this outchannel to an inchannel.
   *
   * This method creates a mapping between this outchannel and the
   * specified inchannel. This method is similar to the previous mapto
   * method except that we refer to the inchannel here using a string
   * name rather than a pointer to the inchannel object. This allows
   * us to establish a mapping between channels at different address
   * spaces in a distributed memory environment.
   *
   * Note that the method can only be called at the simulation
   * initialization stage. A non-negative mapping delay can be
   * associated with each mapping. If the delay is not specified, the
   * mapping delay will default to zero. Note that SSF requires that
   * the sum of the channel delay and the mapping delay must be
   * strictly greater than zero if the mapping crosses different
   * timelines.
   *
   * \param icname the name of the inchannel to be mapped to
   * \param map_delay the mapping delay to be added to the event's total transfer time
   */
  void mapto(const char* icname, VirtualTime map_delay = 0);

  /**
   * \brief Send an event out from this outchannel to all mapped inchannels.
   *
   * A process may write an event to any outchannel as long as the
   * process' owner entity and the channel's owner entity are either
   * identical or coaligned with each other. The written event is
   * scheduled to arrive at its destination after the sum of the
   * channel delay, the mapping delay, and the per-write delay (given
   * by the second argument). The event will be taken by the system
   * and the user should not access it (the write_delay will be forced
   * to null reference) after the function returns
   *
   * \param evt a reference of the pointer to the event to be sent out
   * \param write_delay the per-write delay that will be added to the event's total transfer delay
   */
  void write(Event*& evt, VirtualTime write_delay = 0);

 private:
  // each outchannel has a globally unique port number
  int portno;

  // each outchannel is owned by an entity; the ownership is immutable
  Entity* entity_owner;

  // the channel delay is part of the total delay it takes to send an
  // event from the outchannel to a mapped inchannel
  VirtualTime channel_delay;

  // the outchannel keeps a list of MapOutport objects, one for each
  // target timeline mapped from this outchannel
  DEQUE(MapOutport*) outports;

  //inline int get_portno() { return portno; }
  //inline void set_portno(int s) { portno = s; }

  // this is a helper function to send an event
  void write_event(Event* evt, VirtualTime write_delay);

  friend class Entity;
  friend class Process;
  friend class Event;
  friend class inChannel;
  friend class Timeline;
  friend class ChannelEvent;
  friend class Universe;
}; /*class outChannel*/

}; /*namespace minissf*/

#include "ssfapi/entity.h"
#include "ssfapi/event.h"
#include "ssfapi/inchannel.h"

#endif /*__MINISSF_OUTCHANNEL_H__*/

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
