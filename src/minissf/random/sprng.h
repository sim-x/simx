/**
 * \file sprng.h
 * \brief Header file for SPRNG random number generator.
 *
 * This header file contains the definition of the SPRNGRandom class; Users
 * do not need to include this header file directly; it is included
 * from ssf.h.
 */

#ifndef __MINISSF_SPRNG_H__
#define __MINISSF_SPRNG_H__

#ifndef __MINISSF_RANDOM_H__
#error "DO NOT INCLUDE THIS CLASS DIRECTLY"
#endif

extern "C" { 
#include "random/sprng_sprng.h"
}

namespace minissf {

/**
 * \brief Scalable Parallel Random Number Generators Library (SPRNG).
 *
 * the random number generators are modified from the SPRNG library
 * (version 2.0), developed by Michael Mascagni et al. at Florida
 * State University (http://sprng.cs.fsu.edu), which features six
 * classes of random number generators:
 *
 * 1. Combined Multiple Recursive Generator (CMRG): the period of this
 * generator is around 2^219; the number of distinct streams available
 * is over 10^8.
 *
 * 2. 48 Bit Linear Congruential Generator with Prime Addend
 * (LCG): the period of this generator is 2^48; the number of
 * distinct streams available is of the order of 2^19.
 *
 * 3. 64 Bit Linear Congruential Generator with Prime Addend
 * (LCG64): the period of this generator is 2^64; the number of
 * distinct streams available is over 10^8.
 *
 * 4. Modified Lagged Fibonacci Generator (LFG): the period of
 * this generator is 2^31(2^l-1) where l is the lag; the number
 * of distinct streams available is 2^[31(l-1)-1].
 *
 * 5. Multiplicative Lagged Fibonacci Generator (MLFG): the
 * period of this generator is 2^61(2^l-1), where l is the lag;
 * the number of distinct streams available is 2^[63(l-1)-1].
 *
 * 6. Prime Modulus Linear Congruential Generator (PMLCG): the
 * period of this generator is 2^61-2; the number of distinct
 * streams available is roughly 258. This generator is not
 * included in our library as it requires the GNU multiprecision
 * arithmetic library (libgmp.a).
 *
 * For more information, refer to the paper: M. Mascagni and
 * A. Srinivasan (2000), "Algorithm 806: SPRNG: A Scalable Library for
 * Pseudorandom Number Generation," ACM Transactions on Mathematical
 * Software, 26: 436-461.
 */
class SPRNGRandom : public Random {
 public:
  /** \brief A list of random number generators implemented in SPRNG. */
  enum {
    /** \brief Combined multiple recursive generator (CMRG), a = 0x27BB2EE687B0B0FD. */
    SPRNG_TYPE_CMRG_LECU1,

    /** \brief Combined multiple recursive generator(CMRG), a = 0x2C6FE96EE78B6955. */
    SPRNG_TYPE_CMRG_LECU2,

    /** \brief Combined multiple recursive generator(CMRG), a = 0x369DEA0F31A53F85. */
    SPRNG_TYPE_CMRG_LECU3,      

    /** \brief 48-bit linear congruential generator with prime addend (LCG), a = 0x2875A2E7B175. */
    SPRNG_TYPE_LCG_CRAYLCG,

    /** \brief 48-bit linear congruential generator with prime addend (LCG), a = 0x5DEECE66D. */
    SPRNG_TYPE_LCG_DRAND48,

    /** \brief 48-bit linear congruential generator with prime addend (LCG), a = 0x3EAC44605265. */
    SPRNG_TYPE_LCG_FISH1,

    /** \brief 48-bit linear congruential generator with prime addend (LCG), a = 0x1EE1429CC9F5. */
    SPRNG_TYPE_LCG_FISH2,      

    /** \brief 48-bit linear congruential generator with prime addend (LCG), a = 0x275B38EB4BBD. */
    SPRNG_TYPE_LCG_FISH3,

    /** \brief 48-bit linear congruential generator with prime addend (LCG), a = 0x739A9CB08605. */
    SPRNG_TYPE_LCG_FISH4,

    /** \brief 48-bit linear congruential generator with prime addend (LCG), a = 0x3228D7CC25F5. */
    SPRNG_TYPE_LCG_FISH5,

    /** \brief 64-bit linear congruential generator with prime addend (LCG64), a = 0x27BB2EE687B0B0FD. */
    SPRNG_TYPE_LCG64_LECU1,

    /** \brief 64-bit linear congruential generator with prime addend (LCG64), a = 0x2C6FE96EE78B6955. */
    SPRNG_TYPE_LCG64_LECU2,

    /** \brief 64-bit linear congruential generator with prime addend (LCG64), a = 0x369DEA0F31A53F85. */
    SPRNG_TYPE_LCG64_LECU3,

    /** \brief Modified lagged fibonacci generator (LFG), l = 1279, k = 861. */
    SPRNG_TYPE_LFG_LAG1279,

    /** \brief Modified lagged fibonacci generator (LFG), l = 17, k = 5. */
    SPRNG_TYPE_LFG_LAG17,

    /** \brief Modified lagged fibonacci generator (LFG), l = 31, k = 6. */
    SPRNG_TYPE_LFG_LAG31,

    /** \brief Modified lagged fibonacci generator (LFG), l = 55, k = 24. */
    SPRNG_TYPE_LFG_LAG55,

    /** \brief Modified lagged fibonacci generator (LFG), l = 63, k = 31. */
    SPRNG_TYPE_LFG_LAG63,

    /** \brief Modified lagged fibonacci generator (LFG), l = 127, k = 97. */
    SPRNG_TYPE_LFG_LAG127,

    /** \brief Modified lagged fibonacci generator (LFG), l = 521, k = 353. */
    SPRNG_TYPE_LFG_LAG521,

    /** \brief Modified lagged fibonacci generator (LFG), l = 521, k = 168. */
    SPRNG_TYPE_LFG_LAG521B,

    /** \brief Modified lagged fibonacci generator (LFG), l = 607, k = 334. */
    SPRNG_TYPE_LFG_LAG607,

    /** \brief Modified lagged fibonacci generator (LFG), l = 607, k = 273. */
    SPRNG_TYPE_LFG_LAG607B,

    /** \brief Modified lagged fibonacci generator (LFG), l = 1279, k = 419. */
    SPRNG_TYPE_LFG_LAG1279B,

    /** \brief Multiplicative lagged fibonacci generator (MLFG), l = 1279, k = 861. */
    SPRNG_TYPE_MLFG_LAG1279,

    /** \brief Multiplicative lagged fibonacci generator (MLFG), l = 1279, k = 419. */
    SPRNG_TYPE_MLFG_LAG1279B,

    /** \brief Multiplicative lagged fibonacci generator (MLFG), l = 17, k = 5. */
    SPRNG_TYPE_MLFG_LAG17,

    /** \brief Multiplicative lagged fibonacci generator (MLFG), l = 31, k = 6. */
    SPRNG_TYPE_MLFG_LAG31,

    /** \brief Multiplicative lagged fibonacci generator (MLFG), l = 55, k = 24. */
    SPRNG_TYPE_MLFG_LAG55,

    /** \brief Multiplicative lagged fibonacci generator (MLFG), l = 63, k = 31. */
    SPRNG_TYPE_MLFG_LAG63,

    /** \brief Multiplicative lagged fibonacci generator (MLFG), l = 127, k = 97. */
    SPRNG_TYPE_MLFG_LAG127,

    /** \brief Multiplicative lagged fibonacci generator (MLFG), l = 521, k = 353. */
    SPRNG_TYPE_MLFG_LAG521,
    
    /** \brief Multiplicative lagged fibonacci generator (MLFG), l = 521, k = 168. */
    SPRNG_TYPE_MLFG_LAG521B,

    /** \brief Multiplicative lagged fibonacci generator (MLFG), l = 607, k = 334. */
    SPRNG_TYPE_MLFG_LAG607,

    /** \brief Multiplicative lagged fibonacci generator (MLFG), l = 607, k = 273. */
    SPRNG_TYPE_MLFG_LAG607B,

    // the following macros are used to help define range
    SPRNG_TOTAL,
    SPRNG_LOW   = SPRNG_TYPE_CMRG_LECU1,
    SPRNG_HIGH  = SPRNG_TYPE_MLFG_LAG607B,

    /** \brief Default combined multiple recursive generator. */
    SPRNG_TYPE_CMRG = SPRNG_TYPE_CMRG_LECU1,

    /** \brief Default 48-bit linear congruential generator with prime addend (LCG). */
    SPRNG_TYPE_LCG = SPRNG_TYPE_LCG_CRAYLCG,

    /** \brief Default 64-bit linear congruential generator with prime addend (LCG64). */
    SPRNG_TYPE_LCG64 = SPRNG_TYPE_LCG64_LECU1,

    /** \brief Default modified lagged fibonacci generator (LFG). */
    SPRNG_TYPE_LFG = SPRNG_TYPE_LFG_LAG1279,

    /** \brief Default multiplicative lagged fibonacci generator (MLFG). */
    SPRNG_TYPE_MLFG = SPRNG_TYPE_MLFG_LAG17
  };

  /**
   * \brief The constructor.
   *
   * One must provide a seed to start with and select the type of the
   * random number generator; also, one can create separate random
   * streams using the same seed. If the seed is zero, one will be
   * picked based on current machine time.
   *
   * \param seed the seed for the random streams to be created
   * \param type the type of the random number generator (such as SPRNG_TYPE_CMRG_LECU1)
   * \param streamidx the index of the random streams (ranging from 0 to nstreams-1)
   * \param nstreams the number of random streams to be created (default is 1)
   */
  SPRNGRandom(int seed, int type = SPRNG_TYPE_CMRG, int streamidx = 0, int nstreams = 1);

  /** \brief The destructor. */
  virtual ~SPRNGRandom() { free_sprng(sprng_kernel); }

  /** \brief Return the type id to indicate which random number generator is used. */
  int type() { return sprng_type; }

  /** \brief Return a string to indicate which random number generator is used. */
  char const* type_string() const;

  /** 
   * \brief Set the random seed.
   *
   * SPRNG does not support reseeding the random stream once it has
   * been created. Here we simply ignore it; the function is a no-op.
   */
  virtual void setSeed(int seed) {}

  /** \brief Return a random number uniformly distributed between 0 and 1. */
  virtual double operator()() { return sprng(sprng_kernel); }

  /** 
   * \brief Create separate random streams. 
   *
   * \param n the number of random streams to be created
   * \param s a preallocated array, which will hold the random streams upon return of this function
   */
  virtual void spawnStreams(int n, Random** s);
    
 protected:
  int sprng_type; 
  int* sprng_kernel;

  // used for spawning random streams
  SPRNGRandom(int t, int* k) : sprng_type(t), sprng_kernel(k) {}
}; /*class SPRNGRandom*/

}; /*namespace minissf*/

#endif /*__MINISSF_SPRNG_H__*/

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
