// Copyright (c) 2012. Los Alamos National Security, LLC. 

// This material was produced under U.S. Government contract DE-AC52-06NA25396
// for Los Alamos National Laboratory (LANL), which is operated by Los Alamos 
// National Security, LLC for the U.S. Department of Energy. The U.S. Government 
// has rights to use, reproduce, and distribute this software.  

// NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, 
// EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  
// If software is modified to produce derivative works, such modified software should
// be clearly marked, so as not to confuse it with the version available from LANL.

// Additionally, this library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License v 2.1 as published by the 
// Free Software Foundation. Accordingly, this library is distributed in the hope that 
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See LICENSE.txt for more details.

//---------------------------------------------------------------------
// $Id: Random.C,v 1.1.1.1 2011/08/18 22:19:45 nsanthi Exp $
//---------------------------------------------------------------------
//  Module: Random
//  File: Random.C
//  Revision: 1.7
//  Date: 02/08/01 16:09:40
//  Author: K.P. Berkbigler, Sunil Thulasidasan
//
//  @@
//
//---------------------------------------------------------------------

#include "simx/Random/Random.h"
#include "simx/Common/Assert.h"
#include "simx/Log/Logger.h"

#include <cstring>
#include <iostream>
#include <sys/timeb.h>

//---------------------------------------------------------------------

using namespace std;

//---------------------------------------------------------------------

namespace Random
{

  /*
    sprng.h includes the following definitions, which denote
    "generators".

    #define SPRNG_LFG   0
    #define SPRNG_LCG   1
    #define SPRNG_LCG64 2
    #define SPRNG_CMRG  3
    #define SPRNG_MLFG  4
    #define SPRNG_PMLCG 5

    sprng.h also defines the following default "parameter".

    #define SPRNG_DEFAULT 0
  */

  //  The class has a maximum number of streams.
  int TRandom::fgStreams = 1;

  //  The class uses a seed.
  int TRandom::fgSeed = 985456376;

  // The class uses a module ID for logging.
  int TRandom::fgModuleId = 0;
  
  //  If true, the class will use the logging mechanism for reporting
  //  information and errors.
  bool TRandom::fgUseLogging = true;

  //  The output stream to use when logging is not used for reporting
  //  information and errors.  Defaults to cout.
  ostream* TRandom::fgLogStream = &cout;

  //---------------------------------------------------------------------
 
  //  Construct a random number stream.  The id must be between 0 and 
  //  fgStreams-1.
  TRandom::TRandom(int id)
    :	fId(id)
  {
    if (fgUseLogging && fgModuleId == 0)
      fgModuleId = Log::log().registerModule("RANDOM");

    if (id < 0 || id >= fgStreams) {
      if (fgUseLogging) {
	Log::log().error(fgModuleId) 
	  << "Random: Invalid random stream id " << id
	  << endl;
      } else {
	*fgLogStream 
	  << "Random: Invalid random stream id " << id
	  << endl;
      }
      return;
    }
    
    fStream[0] = 0x330E;	//< arbitrary number
    fStream[1] = id;		//< so that different computers differ in their random nums.
    fStream[2] = fgSeed;
  }

  //---------------------------------------------------------------------

  //  Destroy a random number stream.
  TRandom::~TRandom()
  {
  }

  //---------------------------------------------------------------------
 
  //  Return uniformly distributed double in range [0.0, 1.0).
  
  double TRandom::GetUniform( float min, float max )
  {
    return min + (max - min) * erand48( fStream );
  }

  //  Return uniformly distributed integral type T in range [min,
  // max): a template defined in Random.h.

  


  //---------------------------------------------------------------------

  //  Return exponentially distributed double in range [0, infinity).
  //  Parameter is the mean.
  double TRandom::GetExponential(float mean)
  {
    if (mean <= 0.0) {
      if (fgUseLogging) {
	Log::log().warn(fgModuleId) 
	  << "Random: GetExponential called with mean = " << mean
	  << endl;
      } else {
	*fgLogStream 
	  << "Random: GetExponential called with mean = " << mean
	  << endl;
      }
      return 0.0;
    }
    return -mean * log(erand48(fStream));
  }

  //---------------------------------------------------------------------

  //  Return normally distributed double in range (-infinity,
  //  infinity).  Parameters are mean and standard deviation.
  double TRandom::GetNormal(float mean, float std)
  {
    // Using Leva algorithm (ACM Trans. on Math. Software, 18:449-455,
    // 1992)
    double u, v;
    double r = 2 * sqrt(2.0 / M_E);
    bool done = false;
    while (!done) {
      u = erand48(fStream);
      v = erand48(fStream);
      v = r * (v - 0.5);
      double x = u - 0.449871;
      double y = fabs(v) + 0.386595;
      double q = x * x + y * (0.19600 * y - 0.25472 * x);
      if (q < 0.27597) 
	done = true;
      else if ( q > 0.27846) 
	done = false;
      else if (v * v <= -4 * log(u) * u * u) 
	done = true;
    }
    return mean + std * v / u;
  }

  //---------------------------------------------------------------------

  ///  Return log-normally distributed double in range (0, infinity).
  ///  Parameters are mean and standard deviation on logarithmic scale
  ///  (mean and sd of the underlining normal distr).
  double TRandom::GetLognormal(float meanlog, float stdlog)
  {
    return exp( GetNormal(meanlog, stdlog) );
  }

  //---------------------------------------------------------------------

  //  Return gamma distributed double in range [0, infinity).
  //  Parameters are alpha and beta shape parameters.
  double TRandom::GetGamma(float alpha, float beta)
  {
    if (beta <= 0.0) {
      if (fgUseLogging) {
	Log::log().warn(fgModuleId) 
	  << "Random: GetGamma called with beta = " << beta
	  << endl;
      } else {
	*fgLogStream 
	  << "Random: GetGamma called with beta = " << beta
	  << endl;
      }
      return 0.0;
    }
    if (alpha == 1.0)
      return GetExponential(alpha) * beta;

    if (alpha > 0.25) {
      // Using Cheng and Feast GBH algorithm (CACM, 23:389-394, 1980)
      double a = alpha - .25;
      double c = 2. / a;
      double t = 1. / sqrt(alpha);
      double h1 = (0.4417 + 0.0245 * t / alpha) * t;
      double h2 = (0.222 - 0.043 * t) * t;
      double w = 0;
      bool done = false;
      while (!done) {
	double u1 = erand48(fStream);
	double u = erand48(fStream);
	double u2 = u1 + h1 * u - h2;
	if (u2 <= 0.0 || u2 >= 1.0) 
	  continue;
	double r = u1 / u2;
	r *= r;       // (u1 / u2) ^ 2 
	r *= r;       // (u1 / u2) ^ 4
	w = alpha / a * r;
	if (((c * u2 - c - 2. + w + 1. / w) <= 0.0) 
	    || ((c * log(u2) - log(w) + w - 1.) < 0.0) ) 
	  done = true;
      }
      return a * w * beta;

    } else {	 // alpha <= 0.25
      // Using Ahrens and Dieter GS algorithm (Computing, 12:223-246,
      // 1974)
      if (alpha <= 0.0) {
	if (fgUseLogging) {
	  Log::log().warn(fgModuleId) 
	    << "Random: GetGamma called with alpha = " << alpha
	    << endl;
	} else {
	  *fgLogStream 
	    << "Random: GetGamma called with alpha = " << alpha
	    << endl;
	}
	return 0.0;
      }
      double x = 0;
      double b = (M_E + alpha) / M_E;
      bool done = false;
      while (!done) {
	double p = b * erand48(fStream);
	if (p <= 1.0) {
	  x = exp(log(p) / alpha);
	  if (erand48(fStream) <= exp(-x)) 
	    done = true;
	} else {
	  x = -log((b - p) / alpha);
	  if (log(erand48(fStream)) <= (alpha - 1.) * log(x)) 
	    done = true;
	}
      }
      return x * beta;
    }
  }

  //---------------------------------------------------------------------

  //  Return beta distributed double in range [0, 1).  Parameters are
  //  alpha and beta shape parameters.
  double TRandom::GetBeta(float alpha, float beta)
  {
    if (alpha <= 0.0) {
      if (fgUseLogging) {
	Log::log().warn(fgModuleId) 
	  << "Random: GetBeta called with alpha = " << alpha 
	  << endl;
      } else {
	*fgLogStream 
	  << "Random: GetBeta called with alpha = " << alpha
	  << endl;
      }
      return 0.0;
    }
    if (beta <= 0.0) {
      if (fgUseLogging) {
	Log::log().warn(fgModuleId) 
	  << "Random: GetBeta called with beta = " << beta
	  << endl;
      } else {
	*fgLogStream 
	  << "Random: GetBeta called with beta = " << beta 
	  << endl;
      }
      return 0.0;
    }

    if (alpha == 1.0 && beta == 1.0)
      return erand48(fStream);

    if (alpha == 1.0)
      return 1.0 - exp(log(erand48(fStream)) / beta);

    if (beta == 1.0)
      return exp(log(erand48(fStream)) / alpha);

    double y1, y2;
    y1 = GetGamma(alpha, 1.0);
    y2 = GetGamma(beta, 1.0);
    return y1 / (y1 + y2);
  }

  //---------------------------------------------------------------------

  //  Return a triangularly distributed double in range [min, max].
  //  Parameters are minimum, maximum and mode.
  double TRandom::GetTriangular(float min, float max, float mode)
  {
    double c = (mode - min) / (max - min);
    double x;
    if (c > 0.0 && c < 1.0) {
      double u = erand48(fStream);
      if (u <= c)
	x = sqrt(c * u);
      else
	x = 1.0 - sqrt((1.0 - c) * (1.0 - u));
    }
    else if (c == 0.)	// left triangle
      x = 1.0 - sqrt(1.0 - erand48(fStream));
    else if (c == 1.)	// right triangle
      x = sqrt(erand48(fStream));
    else {
      if (fgUseLogging) {
	Log::log().warn(fgModuleId) 
	  << "Random: GetTriangular called with min = " << min
	  << " mode = " << mode
	  << " max = " << max << endl;
      } else {
	*fgLogStream 
	  << "Random: GetTriangular called with min = " << min
	  << " mode = " << mode 
	  << " max = " << max << endl;
      }
      return min;
    }
    return min + (max - min) * x;
  }

  //---------------------------------------------------------------------

  double TRandom::GetWeibull(float alpha, float gamma, float mi)
  {
    /// uses this cdf: CDF(x): p=1-exp(-((x-mi)/alpha)^gamma)
    /// => x = alpha*(-log(1-p))^(1/gamma)+mi

    const float a = 0.0;
    const float b = 1.0;
    double p = GetUniform(a,b);	///< CDF(x), where x is what we want
    SMART_ASSERT( p>=0.0 && p<1.0 )( p );

    double x = alpha*pow(-log(1.0-p),1.0/gamma)+mi;
    
    return x;
  }

  //---------------------------------------------------------------------

  double TRandom::GetCauchy(float s, float t)
  {
    /// uses this cdf: CDF(x): p=0.5+arctan((x-t)/s)/pi
    /// => x = s*tan(pi*(p-0.5))+t

    const float a = 0.000001;
    const float b = 1.0;
    double p = GetUniform(a,b);	///< CDF(x), where x is what we want
    SMART_ASSERT( p>0.0 && p<1.0 )( p );

    const double pi = 3.1415926535897932384626433832795029L;
    double x = s*tan(pi*(p-0.5))+t;
    
    return x;
  }

  //---------------------------------------------------------------------

  //  Define the number of random streams.  A value < 1 will be reset
  //  to 1.
  void TRandom::SetNumberStreams(int nstreams)
  {
    if (nstreams > 0 ) 
      fgStreams = nstreams;
    else {
      if (fgUseLogging) {
	Log::log().warn(fgModuleId) 
	  << "Random: Number of random streams must be >= 1. Setting to 1."
	  << endl;
      } else {
	*fgLogStream 
	  << "Random: Number of random streams must be >= 1. Setting to 1." 
	  << endl;
      }
      fgStreams = 1;
    }
  }

  //---------------------------------------------------------------------

  //  Return the number of random streams.
  int TRandom::GetNumberStreams()
  {
    return fgStreams;
  }

  //---------------------------------------------------------------------

  //  Define the seed.  A value of 0 will cause the MakeSeed()
  //  function to be used.
  void TRandom::SetSeed(int seed)
  {
    if (seed != 0)
      fgSeed = seed;
    else
      MakeSeed();
  }

  //---------------------------------------------------------------------

  //  Return the value of seed.
  int TRandom::GetSeed()
  {
    return fgSeed;
  }

  //---------------------------------------------------------------------

  //  Generate and return the seed based upon system date and time.
  int TRandom::MakeSeed()
  {
    struct timeb tb;
    ftime( &tb );
    fgSeed = tb.millitm;
    return fgSeed;
  }

  //---------------------------------------------------------------------


  //  Define all the static values.
  void TRandom::SetDefaults(int nstreams, int seed, int gen, int param)
  {
    SetNumberStreams(nstreams);
    SetSeed(seed);
  }

  //---------------------------------------------------------------------

  //  Define whether the class should use logging or another output
  //  stream for reporting information and errors.
  void TRandom::SetLogging(bool b)
  {
    fgUseLogging = b;
  }

  //---------------------------------------------------------------------

  //  Define the output stream to use when logging is not used for
  //  reporting information and errors.  Defaults to cout.
  void TRandom::SetLogStream(ostream* s)
  {
    fgLogStream = s;
  }

} // namespace

//---------------------------------------------------------------------
//---------------------------------------------------------------------

#include <boost/python.hpp>
//namespace simx {

//namespace Python {
    
using namespace Random;
using namespace boost;
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GetUniformD,TRandom::GetUniform,0,2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GetUniformI,TRandom::template GetUniform<int>,2,2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GetUniformL,TRandom::template GetUniform<long>,2,2);

void export_Random() {

  class_<TRandom,noncopyable>("TRandom",no_init)
    .def("get_uniform",static_cast< double(TRandom::*)(float,float)>
	 (&TRandom::GetUniform),GetUniformD() )
    .def("get_uniform", static_cast<int (TRandom::*)(int,int)>
	 (&TRandom::GetUniform<int>), GetUniformI() )
    .def("get_uniform", static_cast<long (TRandom::*)(long,long)>
	 (&TRandom::GetUniform<long>), GetUniformL() )
    .def("get_exponential",&TRandom::GetExponential)
    .def("get_normal",&TRandom::GetNormal)
    .def("get_lognormal",&TRandom::GetLognormal)
    .def("get_gamma",&TRandom::GetGamma)
    .def("get_beta",&TRandom::GetBeta)
    .def("get_triangular",&TRandom::GetTriangular)
    .def("get_weibull",&TRandom::GetWeibull)
    .def("get_cauchy",&TRandom::GetCauchy)
    .def("set_num_streams",&TRandom::SetNumberStreams)
    .def("get_num_streams",&TRandom::GetNumberStreams)
    .def("set_seed",&TRandom::SetSeed)
    .def("get_seed",&TRandom::GetSeed)
    .def("make_seed",&TRandom::MakeSeed)
    ;
}
//}
//}
