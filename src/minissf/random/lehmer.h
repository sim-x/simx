/**
 * \file lehmer.h
 * \brief Header file for Lehmer random number generator.
 *
 * This header file contains the definition of the LehmerRandom class;
 * Users do not need to include this header file directly; it is
 * included from ssf.h.
 */

#ifndef __MINISSF_LEHMER_H__
#define __MINISSF_LEHMER_H__

#ifndef __MINISSF_RANDOM_H__
#error "DO NOT INCLUDE THIS CLASS DIRECTLY"
#endif

#include <assert.h>
#include "kernel/throwable.h"

namespace minissf {

/**
 * \brief Implementation of the linear congruent method for random number generation.
 */
template<int a, int m, int ms, int js>
class LehmerTemplate : public Random {
 public:
  static const int multiplier = a;
  static const int modulus = m;
  static const int max_streams = ms;
  static const int jump_multiplier = js;
  static const int quotient = m/a;
  static const int remainder = m%a;
  static const int jump_quotient = m/js;
  static const int jump_remainder = m%js;

  /** 
   * \brief The constructor.
   * 
   * One must provide a seed to start with; also, one can create
   * separate random streams using the same seed. If the seed is zero,
   * one will be picked based on current machine time.
   *
   * \param myseed the seed for the random streams to be created
   * \param streamidx the index of the random streams (ranging from 0 to nstreams-1)
   * \param nstreams the number of random streams to be created (default is 1)
   */
  LehmerTemplate(int myseed, int streamidx = 0, int nstreams = 1) {
    assert(multiplier < modulus);
    assert(remainder < quotient); // must be modulus compatible
    setSeed(myseed);
    stream_index = streamidx;
    num_streams = nstreams;
    if(stream_index < 0 || stream_index >= num_streams) 
      SSF_THROW("invalid arguments: stream_index=" << stream_index << ", num_streams=" << num_streams);
    if(num_streams > max_streams)
      SSF_THROW("exceeding the max number of streams");
    for (int j=0; j<stream_index; j++) {
      seed = jump_multiplier*(seed%jump_quotient)-
	jump_remainder*(seed/jump_quotient);
      if(seed <= 0) seed += modulus;
    }
  }

  /** \brief Set the random seed; if 0, pick one using the system clock. */
  virtual void setSeed(int myseed) {
    if(myseed == 0) seed = (int)make_new_seed();
    else seed = myseed+global_seed;
    seed = multiplier*(seed%quotient)-remainder*(seed/quotient); // draw one number so the seed is not predictable
    if(seed <= 0) seed += modulus;
    init_seed = seed;
  }

  /** \brief Return a random number uniformly distributed between 0 and 1. */
  virtual double operator()() {
    seed = multiplier*(seed%quotient)-remainder*(seed/quotient);
    if(seed <= 0) seed += modulus;
    return double(seed)/modulus;
  }

  /** 
   * \brief Create separate random streams. 
   *
   * \param n the number of random streams to be created
   * \param s a preallocated array, which will hold the random streams upon return of this function
   */
  virtual void spawnStreams(int n, Random** s) {
    LehmerTemplate** streams = (LehmerTemplate**)s;
    if(n <= 0 || !streams) SSF_THROW("invalid arguments: n=" << n << ", streams=" << streams);
    if(n > max_streams) SSF_THROW("exceeding the max number of streams");
    for(int i=0; i<n; i++) {
      streams[i] = new LehmerTemplate(); assert(streams[i]);
      streams[i]->init_seed = streams[i]->seed = init_seed;
      streams[i]->stream_index = num_streams + num_streams*(i+1);
      streams[i]->num_streams = num_streams*(n+1); // skip a lot!!!
      for (int j=0; j<streams[i]->stream_index; j++) {
	streams[i]->seed = jump_multiplier*(streams[i]->seed%jump_quotient)-
	  jump_remainder*(streams[i]->seed/jump_quotient);
	if(streams[i]->seed <= 0) streams[i]->seed += modulus;
      }
    }
  }

 private:
  int seed;
  int init_seed;
  int stream_index;
  int num_streams;
    
  LehmerTemplate() {} // called by spawnStream, doing nothing
}; /*template class LehmerTemplate*/

/**
 * \brief Lehmer random number generator.
 *
 * The Lehmer random number generator uses linear congruent methods
 * for random number generation. The generator uses a multiplier of
 * 48271 and a modulus of 2^31-1. The generator has 256 random streams
 * (with the jump multiplier being 22925). For details, consult the
 * book "Discrete-Event Simulation: a First Course", by Lawrence
 * Leemis and Steve Park, Prentice Hall 2005.
 */
typedef LehmerTemplate<48271, 2147483647, 256, 22925> LehmerRandom;

}; /*namespace minissf*/

#endif /*__MINISSF_LEHMER_H__*/

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
