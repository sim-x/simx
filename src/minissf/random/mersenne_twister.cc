#include <math.h>
#include "random/random.h"

namespace minissf {

MersenneTwisterRandom::MersenneTwisterRandom(const uint32 one_seed)
{ seed(one_seed); randInt(); }

MersenneTwisterRandom::MersenneTwisterRandom(uint32 *const big_seed, const uint32 seed_len)
{ seed(big_seed,seed_len); randInt(); }

double MersenneTwisterRandom::rand()
{ return double(randInt()) * (1.0/4294967295.0); }

double MersenneTwisterRandom::rand(const double& n)
{ return rand() * n; }

double MersenneTwisterRandom::randExc()
{ return double(randInt()) * (1.0/4294967296.0); }

double MersenneTwisterRandom::randExc(const double& n)
{ return randExc() * n; }

double MersenneTwisterRandom::randDblExc()
{ return (double(randInt()) + 0.5) * (1.0/4294967296.0); }

double MersenneTwisterRandom::randDblExc(const double& n)
{ return randDblExc() * n; }

double MersenneTwisterRandom::rand53() 
{
  uint32 a = randInt() >> 5, b = randInt() >> 6;
  return (a * 67108864.0 + b) * (1.0/9007199254740992.0);  // by Isaku Wada
}

double MersenneTwisterRandom::randNorm(const double& mean, const double& variance)
{
  // Return a real number from a normal (Gaussian) distribution with given
  // mean and variance by Box-Muller method
  double r = sqrt(-2.0 * log(1.0-randDblExc())) * variance;
  double phi = 2.0 * 3.14159265358979323846264338328 * randExc();
  return mean + r * cos(phi);
}

MersenneTwisterRandom::uint32 MersenneTwisterRandom::randInt()
{
  // pull a 32-bit integer from the generator state; every other
  // access function simply transforms the numbers extracted here

  if(left == 0) reload();
  --left;
		
  register uint32 s1;
  s1 = *pNext++;
  s1 ^= (s1 >> 11);
  s1 ^= (s1 <<  7) & 0x9d2c5680UL;
  s1 ^= (s1 << 15) & 0xefc60000UL;
  return (s1 ^ (s1 >> 18));
}

MersenneTwisterRandom::uint32 MersenneTwisterRandom::randInt(const uint32& n) 
{
  // find which bits are used in n; optimized by Magnus Jonsson
  // (magnus@smartelectronix.com)
  uint32 used = n;
  used |= used >> 1;
  used |= used >> 2;
  used |= used >> 4;
  used |= used >> 8;
  used |= used >> 16;

  // draw numbers until one is found in [0,n]
  uint32 i;
  do {
    i = randInt() & used;  // toss unused bits to shorten search
  } while(i > n);
  return i;
}

void MersenneTwisterRandom::seed(int one_seed) 
{
  if(one_seed == 0) one_seed = (uint32)make_new_seed();
  initialize(one_seed+global_seed);
  reload();
}

void MersenneTwisterRandom::seed(uint32 *const big_seed, const uint32 seed_len) 
{
  // seed the generator with an array of uint32's; there are 2^19937-1
  // possible initial states, this function allows all of those to be
  // accessed by providing at least 19937 bits (with a default seed
  // length of N = 624 uint32's); any bits above the lower 32 in each
  // element are discarded
  initialize(19650218UL);
  register int i = 1;
  register uint32 j = 0;
  register int k = (N > seed_len ? N : seed_len);
  for(; k; --k) {
    state[i] = state[i] ^ ((state[i-1] ^ (state[i-1] >> 30)) * 1664525UL);
    state[i] += ((big_seed[j]+global_seed) & 0xffffffffUL) + j;
    state[i] &= 0xffffffffUL;
    ++i;  ++j;
    if(i >= N) { state[0] = state[N-1];  i = 1; }
    if(j >= seed_len) j = 0;
  }
  for(k = N - 1; k; --k) {
    state[i] = state[i] ^ ((state[i-1] ^ (state[i-1] >> 30)) * 1566083941UL);
    state[i] -= i;
    state[i] &= 0xffffffffUL;
    ++i;
    if(i >= N) { state[0] = state[N-1];  i = 1; }
  }
  state[0] = 0x80000000UL;  // MSB is 1, assuring non-zero initial array
  reload();
}

void MersenneTwisterRandom::initialize(const uint32 seed)
{
  // initialize generator state with seed; see Knuth TAOCP Vol 2, 3rd
  // Ed, p.106 for multiplier; in previous versions, most significant
  // bits (MSBs) of the seed affect only MSBs of the state array
  // (modified 9 Jan 2002 by Makoto Matsumoto)
  register uint32 *s = state;
  register uint32 *r = state;
  register int i = 1;
  *s++ = seed & 0xffffffffUL;
  for(; i < N; ++i) {
    *s++ = (1812433253UL * (*r ^ (*r >> 30)) + i) & 0xffffffffUL;
    r++;
  }
}
  
void MersenneTwisterRandom::reload() 
{
  // generate N new values in state; made clearer and faster by
  // Matthew Bellew (matthew.bellew@home.com)
  register uint32 *p = state;
  register int i;
  for(i = N - M; i--; ++p)
    *p = twist(p[M], p[0], p[1]);
  for(i = M; --i; ++p)
    *p = twist(p[M-N], p[0], p[1]);
  *p = twist(p[M-N], p[0], state[0]);
    
  left = N, pNext = state;
}
  
void MersenneTwisterRandom::save(uint32* saveArray) const 
{
  register uint32 *sa = saveArray;
  register const uint32 *s = state;
  register int i = N;
  for(; i--; *sa++ = *s++) {}
  *sa = left;
  }

void MersenneTwisterRandom::load(uint32 *const loadArray) 
{
  register uint32 *s = state;
  register uint32 *la = loadArray;
  register int i = N;
  for(; i--; *s++ = *la++) {}
  left = *la;
  pNext = &state[N-left];
}
 
/*
std::ostream& operator<<(std::ostream& os, const MersenneTwisterRandom& mtrand) {
  register const MersenneTwisterRandom::uint32 *s = mtrand.state;
  register int i = mtrand.N;
  for(; i--; os << *s++ << "\t") {}
  return os << mtrand.left;
}

std::istream& operator>>(std::istream& is, MersenneTwisterRandom& mtrand) {
  register MersenneTwisterRandom::uint32 *s = mtrand.state;
  register int i = mtrand.N;
  for(; i--; is >> *s++) {}
  is >> mtrand.left;
  mtrand.pNext = &mtrand.state[mtrand.N-mtrand.left];
  return is;
}
*/

}; /*namespace minissf*/

// Mersenne Twister random number generator -- a C++ class MTRand
// Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
// Richard J. Wagner  v1.0  15 May 2003  rjwagner@writeme.com

// The Mersenne Twister is an algorithm for generating random numbers.  It
// was designed with consideration of the flaws in various other generators.
// The period, 2^19937-1, and the order of equidistribution, 623 dimensions,
// are far greater.  The generator is also fast; it avoids multiplication and
// division, and it benefits from caches and pipelines.  For more information
// see the inventors' web page at http://www.math.keio.ac.jp/~matumoto/emt.html

// Reference
// M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally
// Equidistributed Uniform Pseudo-Random Number Generator", ACM Transactions on
// Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp 3-30.

// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
// Copyright (C) 2000 - 2003, Richard J. Wagner
// All rights reserved.                          
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//   3. The names of its contributors may not be used to endorse or promote 
//      products derived from this software without specific prior written 
//      permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// The original code included the following notice:
//
//     When you use this, send an email to: matumoto@math.keio.ac.jp
//     with an appropriate reference to your work.
//
// It would be nice to CC: rjwagner@writeme.com and Cokus@math.washington.edu
// when you write.
