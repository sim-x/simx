/**
 * \file mersenne_twister.h
 * \brief Header file for Mersenne Twister random number generator.
 *
 * This header file contains the definition of the
 * MersenneTwisterRandom class; Users do not need to include this
 * header file directly; it is included from ssf.h.
 */

#ifndef __MINISSF_MERSENNE_TWISTER_H__
#define __MINISSF_MERSENNE_TWISTER_H__

#ifndef __MINISSF_RANDOM_H__
#error "DO NOT INCLUDE THIS CLASS DIRECTLY"
#endif

namespace minissf {

/**
 * \brief Mersenne Twister random number generator.
 *
 * The Mersenne Twister random number generator has a period of
 * 2^19937-1. It gives a sequence that is 623-dimensionally
 * equidistributed. The code is modified from that of Richard
 * J. Wagner, which is based on code by Makoto Matsumoto, Takuji
 * Nishimura, and Shawn Cokus. For more information, check out
 * http://www.math.keio.ac.jp/~matumoto/emt.html.
 */
class MersenneTwisterRandom : public Random {
 public:
  typedef unsigned int uint32;  // unsigned integer type, at least 32 bits
  enum { N = 624 };       // length of state vector
  enum { SAVE = N + 1 };  // length of array for save()

 protected:
  enum { M = 397 };  // period parameter
	
  uint32 state[N];   // internal state
  uint32 *pNext;     // next value to get from state
  int left;          // number of values left before reload needed

 public:
  /** \brief The constructor using a simple unsigned integer as seed. */
  MersenneTwisterRandom(const uint32 one_seed);

  /** \brief The constructor using an array of integers as seed. */
  MersenneTwisterRandom(uint32* const big_seed, const uint32 seed_len = N);

  /** \brief Set the random seed; if 0, pick one using the system clock. */
  virtual void setSeed(int one_seed) { seed(one_seed); }

  /** \brief Return a random number uniformly distributed between 0 and 1. */
  virtual double operator()() { return randDblExc(); }

  /** 
   * \brief Create separate random streams. 
   *
   * \param n the number of random streams to be created
   * \param s a preallocated array, which will hold the random streams upon return of this function
   */
  virtual void spawnStreams(int n, Random** s) {
    // Mersenne Twister does not really support multiple random
    // streams (no need); here's a stupid hack
    for(int i=0; i<n; i++) {
      s[i] = new MersenneTwisterRandom((uint32)((*this)()*0x07fff0000));
    }
  }
    
 protected:
  // access to 32-bit random numbers
  double rand();                          // real number in [0,1]
  double rand(const double& n);           // real number in [0,n]
  double randExc();                       // real number in [0,1)
  double randExc(const double& n);        // real number in [0,n)
  double randDblExc();                    // real number in (0,1)
  double randDblExc(const double& n);     // real number in (0,n)
  uint32 randInt();                       // integer in [0,2^32-1]
  uint32 randInt(const uint32& n);        // integer in [0,n] for n < 2^32
	
  // access to 53-bit random numbers (capacity of IEEE double precision)
  double rand53();  // real number in [0,1)
	
  // access to nonuniform random number distributions
  double randNorm(const double& mean = 0.0, const double& variance = 0.0);
	
  // re-seeding functions with same behavior as initializers
  void seed(int one_seed);
  void seed(uint32* const big_seed, const uint32 seed_len = N);

  // saving and loading generator state
  void save(uint32* saveArray) const;  // to array of size SAVE
  void load(uint32* const loadArray);  // from such array
  
  /*
  friend std::ostream& operator<<(std::ostream& os, const MersenneTwisterRandom& mtrand);
  friend std::istream& operator>>(std::istream& is, MersenneTwisterRandom& mtrand);
  */

  void initialize(const uint32 one_seed);
  void reload();
  uint32 hiBit(const uint32& u) const { return u & 0x80000000UL; }
  uint32 loBit(const uint32& u) const { return u & 0x00000001UL; }
  uint32 loBits(const uint32& u) const { return u & 0x7fffffffUL; }
  uint32 mixBits(const uint32& u, const uint32& v) const
    { return hiBit(u) | loBits(v); }
  uint32 twist(const uint32& m, const uint32& s0, const uint32& s1) const
    { return m ^ (mixBits(s0,s1)>>1) ^ (-loBit(s1) & 0x9908b0dfUL); }
}; /*class MersenneTwisterRandom*/

}; // namespace minissf

#endif /*__MINISSF_MERSENNE_TWISTER_H__*/

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
