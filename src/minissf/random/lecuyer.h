/**
 * \file lecuyer.h
 * \brief Header file for random number generator developed by
 * L'Ecuyer et al.
 *
 * This header file contains the definition of the LecuyerRandom
 * class; Users do not need to include this header file directly; it
 * is already included from ssf.h.
 */

#ifndef __MINISSF_LECUYERRANDOM_H__
#define __MINISSF_LECUYERRANDOM_H__

#ifndef __MINISSF_RANDOM_H__
#error "DO NOT INCLUDE THIS CLASS DIRECTLY"
#endif

#include <string>

namespace minissf {

class LecuyerRandom : public Random {
public:
  /**
   * \brief The constructor, which creates a new random stream.
   *
   * The constructor initializes its seed Ig, and sets Bg and Cg to
   *  Ig. It also sets its anti and incPrec switches to false.
   */
  LecuyerRandom (int seed);

  /**
   * \brief Reinitializes the stream to its initial state: Cg and Bg
   * are set to Ig.
   */
  void ResetStartStream ();

  /*
   * \brief Reinitializes the stream to the beginning of its current
   * substream: Cg is set to Bg.
   */
  void ResetStartSubstream ();

  /*
   * \brief Reinitializes the stream to the beginning of its next
   * substream.
   */
  void ResetNextSubstream ();

  /*
   * \brief Advances the state by n steps (see below for the meaning
   * of n), without modifying the states of other streams or the
   * values of Bg and Ig in the current object.
   */
  void AdvanceState (long e, long c);

  /**
   * \brief Set antithetic attribute.
   *
   * \param a is the antithetic attribute. If a = true, the stream
   * will start generating antithetic variate, until this method is
   * called again with a = false.
   */
  void SetAntithetic (bool a);

  /**
   * \brief Set precision attribute.
   *
   * \param incp is the precision attribute. If incp = true, after
   * calling this method each call to the generator for this stream
   * will return a uniform random number with more bits of resolution
   * instead of 32 bits, and will advance the state of the stream by 2
   * steps instead of 1.
   */
  void IncreasedPrecis (bool incp);

  /**
   * \breif Returns the current state Cg of this stream.
   *
   * \param seed contains the current state CG of this stream.
   */
  void GetState (unsigned long seed[6]) const;

  /**
   * \brief Writes (to standard output) the current state Cg of this
   * stream.
   */
  void WriteState () const;

  /**
   * \brief Writes (to standard output) the value of all the internal
   * variables of this stream: anti, incPrec, Ig, Bg, Cg.
   */
  void WriteStateFull () const;

  /**
   * \brief Returns a (pseudo) random number from the uniform
   * distribution over the interval (0, 1).
   */
  double RandU01 ();

  /**
   * \brief Returns a (pseudo) random number from the discrete uniform
   * distribution over the integers i to j.
   *
   * \param i is the lower boundary
   * \param j is the higher boundary
   */
  int RandInt (int i, int j);

 public:
  /** \brief Set the random seed; if 0, pick one using the system clock. */
  virtual void setSeed(int seed);

  /**
   * \brief Create separate random streams.
   *
   * \param n the number of random streams to be created
   * \param s a preallocated array, which will hold the random streams upon return of this function
   */
  virtual void spawnStreams(int n, Random** s) {
    for(int i=0; i<n; i++) {
      s[i] = new LecuyerRandom((int)((*this)()*0x07fff0000));
    }
  }

  /** \brief Return a random number uniformly distributed between 0 and 1. */
  virtual double operator()() { return RandU01(); }

 private:
  /*
   * Vectors to store:
   *  Cg: the current seed
   *  Bg: the beginning of the current substream
   *  Ig: the beginning of the current stream.
   */
  double Cg[6], Bg[6], Ig[6];

  // variables to indicate whether to generate antithetic or increased precision random numbers
  bool anti, incPrec;

  // static vector to store the beginning state of the next LecuyerRandom to be created
  static double nextSeed[6];

  // The backbone uniform random number generator
  double U01 ();

  // the backbone uniform random number generator with increased precision
  double U01d ();

}; /*class LecuyerRandom*/

}; /*namespace minissf*/

#endif /*__MINISSF_LECUYERRANDOM_H__*/

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

