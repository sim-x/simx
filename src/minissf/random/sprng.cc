#include <assert.h>
#include "random/random.h"
#include "kernel/throwable.h"

namespace minissf {

SPRNGRandom::SPRNGRandom(int s, int t, int idx, int n) : 
  sprng_type(t) 
{
  if(s == 0) s = make_new_seed();
  else s += global_seed;

  switch(t) {
  case SPRNG_TYPE_CMRG_LECU1:
  case SPRNG_TYPE_CMRG_LECU2:
  case SPRNG_TYPE_CMRG_LECU3:
    sprng_kernel = init_sprng(SPRNG_CMRG, idx, n, s, t-SPRNG_TYPE_CMRG_LECU1);
    break;

  case SPRNG_TYPE_LCG_CRAYLCG:
  case SPRNG_TYPE_LCG_DRAND48:
  case SPRNG_TYPE_LCG_FISH1:
  case SPRNG_TYPE_LCG_FISH2:
  case SPRNG_TYPE_LCG_FISH3:
  case SPRNG_TYPE_LCG_FISH4:
  case SPRNG_TYPE_LCG_FISH5:
    sprng_kernel = init_sprng(SPRNG_LCG, idx, n, s, t-SPRNG_TYPE_LCG_CRAYLCG);
    break;

  case SPRNG_TYPE_LCG64_LECU1:
  case SPRNG_TYPE_LCG64_LECU2:
  case SPRNG_TYPE_LCG64_LECU3:
    sprng_kernel = init_sprng(SPRNG_LCG64, idx, n, s, t-SPRNG_TYPE_LCG64_LECU1);
    break;

  case SPRNG_TYPE_LFG_LAG1279:
  case SPRNG_TYPE_LFG_LAG17:
  case SPRNG_TYPE_LFG_LAG31:
  case SPRNG_TYPE_LFG_LAG55:
  case SPRNG_TYPE_LFG_LAG63:
  case SPRNG_TYPE_LFG_LAG127:
  case SPRNG_TYPE_LFG_LAG521:
  case SPRNG_TYPE_LFG_LAG521B:
  case SPRNG_TYPE_LFG_LAG607:
  case SPRNG_TYPE_LFG_LAG607B:
  case SPRNG_TYPE_LFG_LAG1279B:
    sprng_kernel = init_sprng(SPRNG_LFG, idx, n, s, t-SPRNG_TYPE_LFG_LAG1279);
    break;
    
  case SPRNG_TYPE_MLFG_LAG1279:
  case SPRNG_TYPE_MLFG_LAG17:
  case SPRNG_TYPE_MLFG_LAG31:
  case SPRNG_TYPE_MLFG_LAG55:
  case SPRNG_TYPE_MLFG_LAG63:
  case SPRNG_TYPE_MLFG_LAG127:
  case SPRNG_TYPE_MLFG_LAG521:
  case SPRNG_TYPE_MLFG_LAG521B:
  case SPRNG_TYPE_MLFG_LAG607:
  case SPRNG_TYPE_MLFG_LAG607B:
  case SPRNG_TYPE_MLFG_LAG1279B:
    sprng_kernel = init_sprng(SPRNG_MLFG, idx, n, s, t-SPRNG_TYPE_MLFG_LAG1279);
    break;

  default: SSF_THROW("invalid type: type=" << t);
  }

  // draw one random number so that seed is not predictable
  sprng(sprng_kernel);
}

char const* SPRNGRandom::type_string() const
{
  switch(sprng_type) {
  case SPRNG_TYPE_CMRG_LECU1: return "SPRNG/CMRG LECU1";
  case SPRNG_TYPE_CMRG_LECU2: return "SPRNG/CMRG LECU2";
  case SPRNG_TYPE_CMRG_LECU3: return "SPRNG/CMRG LECU3";
  case SPRNG_TYPE_LCG_CRAYLCG: return "SPRNG/LCG CRAYLCG";
  case SPRNG_TYPE_LCG_DRAND48: return "SPRNG/LCG DRAND48";
  case SPRNG_TYPE_LCG_FISH1: return "SPRNG/LCG FISH1";
  case SPRNG_TYPE_LCG_FISH2: return "SPRNG/LCG FISH2";
  case SPRNG_TYPE_LCG_FISH3: return "SPRNG/LCG FISH3";
  case SPRNG_TYPE_LCG_FISH4: return "SPRNG/LCG FISH4";
  case SPRNG_TYPE_LCG_FISH5: return "SPRNG/LCG FISH5";
  case SPRNG_TYPE_LCG64_LECU1: return "SPRNG/LCG64 LECU1";
  case SPRNG_TYPE_LCG64_LECU2: return "SPRNG/LCG64 LECU2";
  case SPRNG_TYPE_LCG64_LECU3: return "SPRNG/LCG64 LECU3";
  case SPRNG_TYPE_LFG_LAG1279: return "SPRNG/LFG LAG1279";
  case SPRNG_TYPE_LFG_LAG17: return "SPRNG/LFG LAG17";
  case SPRNG_TYPE_LFG_LAG31: return "SPRNG/LFG LAG31";
  case SPRNG_TYPE_LFG_LAG55: return "SPRNG/LFG LAG55";
  case SPRNG_TYPE_LFG_LAG63: return "SPRNG/LFG LAG63";
  case SPRNG_TYPE_LFG_LAG127: return "SPRNG/LFG LAG127";
  case SPRNG_TYPE_LFG_LAG521: return "SPRNG/LFG LAG521";
  case SPRNG_TYPE_LFG_LAG521B: return "SPRNG/LFG LAG521B";
  case SPRNG_TYPE_LFG_LAG607: return "SPRNG/LFG LAG607";
  case SPRNG_TYPE_LFG_LAG607B: return "SPRNG/LFG LAG607B";
  case SPRNG_TYPE_LFG_LAG1279B: return "SPRNG/LFG LAG1279B";
  case SPRNG_TYPE_MLFG_LAG1279: return "SPRNG/MLFG LAG1279";
  case SPRNG_TYPE_MLFG_LAG17: return "SPRNG/MLFG LAG17";
  case SPRNG_TYPE_MLFG_LAG31: return "SPRNG/MLFG LAG31";
  case SPRNG_TYPE_MLFG_LAG55: return "SPRNG/MLFG LAG55";
  case SPRNG_TYPE_MLFG_LAG63: return "SPRNG/MLFG LAG63";
  case SPRNG_TYPE_MLFG_LAG127: return "SPRNG/MLFG LAG127";
  case SPRNG_TYPE_MLFG_LAG521: return "SPRNG/MLFG LAG521";
  case SPRNG_TYPE_MLFG_LAG521B: return "SPRNG/MLFG LAG521B";
  case SPRNG_TYPE_MLFG_LAG607: return "SPRNG/MLFG LAG607";
  case SPRNG_TYPE_MLFG_LAG607B: return "SPRNG/MLFG LAG607B";
  case SPRNG_TYPE_MLFG_LAG1279B: return "SPRNG/MLFG LAG1279B";
  default: assert(0);
  }
  return "SPRNG/Unknown Type";
}

void SPRNGRandom::spawnStreams(int n, Random** s)
{
  SPRNGRandom** streams = (SPRNGRandom**)s;
  if(n <= 0 || !streams) SSF_THROW("invalid arguments: n=" << n << ", streams=" << streams);
  int** r; int rn = spawn_sprng(sprng_kernel, n, &r);
  if(rn != n) {
    for(int i=0; i<rn; i++) free_sprng(r[i]); free(r);
    SSF_THROW("failed to spawn random number streams");
  }
  for(int i=0; i<n; i++) streams[i] = new SPRNGRandom(sprng_type, r[i]);
  free(r);
}

}; /*namespace minissf*/

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
