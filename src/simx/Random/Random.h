//---------------------------------------------------------------------
// $Id: Random.h,v 1.1.1.1 2011/08/18 22:19:45 nsanthi Exp $
//---------------------------------------------------------------------
//  Module: Random
//  File: Random.h
//  Revision: 1.4
//  Date: 02/08/01 16:09:33
//  Author: K.P. Berkbigler
//
//  @@COPYRIGHT@@

/// A random stream is initialized according to the class's static
/// variables in effect when a TRandom instance is constructed.  Two
/// streams that are initialized with the same static variables (e.g.,
/// seeds) will generate the same sequence of random numbers (unless
/// the static variables are subsequently changed and one of the
/// streams is reinitialized.) The class contains some number (default
/// one) of independent streams.

/// Because of TRandom's use of static members, changing those members
/// in one instantiation can carry over to later instantiations, even
/// if the first no longer exists. To avoid such interaction, reset
/// the values (SetDefaults with all default values) for the second
/// instantiation.

/// Note that to change the behavior of the RNG itself, TRandom's
/// static members have to be changed and then then the instance has
/// to be reinitialized.

//---------------------------------------------------------------------

#ifndef NISAC_RANDOM_RANDOM_H
#define NISAC_RANDOM_RANDOM_H

//---------------------------------------------------------------------
 
#include <cmath>
#include <iosfwd>
#include <stdlib.h>

//---------------------------------------------------------------------

/// Random number generator
namespace Random { 
  /// \class TRandom Random.h "Random/Random.h"
  ///
  /// \brief This class encapsulates the SPRNG random number
  /// generation software and generates values from several popular
  /// distributions.
  ///
  /// \sa http://sprng.cs.fsu.edu/
  class TRandom
  {
  public:
 
    ///  Construct a random number stream.  The id must be in
    ///  [0,fgStreams-1].
    TRandom(int id);
 
    ///  Destroy a random number stream.
    ~TRandom();
 
    /// Template specialization for returning uniformly distributed 
    /// double in range [min, max).
    double GetUniform( float min = 0.0, float max = 1.0 );

    // int GetUniform( int, int )
    // {
    //   return 1;
    // }

    ///  Return uniformly distributed integral type T in range [min, max).
    template<typename T> T GetUniform( T min, T max )
    {
      return min + 
    	static_cast<T>( floor((max - min + 1) * erand48(fStream)) );
    }
    
   

    ///  Return exponentially distributed double in range [0,
    ///  infinity).  Parameter is the mean.
    double GetExponential(float mean);

    ///  Return normally distributed double in range (-infinity,
    ///  infinity).  Parameters are mean and standard deviation.
    double GetNormal(float mean = 0., float std = 1.);

    ///  Return log-normally distributed double in range (0,
    ///  infinity).  Parameters are mean and standard deviation on
    ///  logarithmic scale (mean and sd of the underlining normal
    ///  distr).
    double GetLognormal(float meanlog = 0., float stdlog = 1.);

    ///  Return gamma distributed double in range [0, infinity).
    ///  Parameters are alpha and beta shape parameters.
    double GetGamma(float alpha, float beta);

    ///  Return beta distributed double in range [0, 1).
    ///  Parameters are alpha and beta shape parameters.
    double GetBeta(float alpha, float beta);

    ///  Return a triangularly distributed double in range [min, max].
    ///  Parameters are minimum, maximum and mode.
    double GetTriangular(float min, float max, float mode);

    /// Return Weibull distributed double
    /// uses 3-parameter Weibull: 
    /// \param alpha = scale param
    /// \param gamma = shape param
    /// \param mi = location param
    /// (see http://www.itl.nist.gov/div898/handbook/apr/section1/apr162.htm)
    double GetWeibull(float alpha, float gamma, float mi); 

    /// Return Cauchy distributed double
    /// uses 3-parameter Weibull: 
    /// \param s = scale param
    /// \param t = location param
    /// (see http://www.itl.nist.gov/div898/handbook/eda/section3/eda3663.htm)
    double GetCauchy(float s, float t);

    ///  Define the number of random streams.  A value < 1 will be
    ///  reset to 1.
    static void SetNumberStreams(int nstreams);

    ///  Return the number of random streams.
    static int GetNumberStreams();

    ///  Define the seed.  A value of 0 will cause the MakeSeed()
    ///  function to be used.
    static void SetSeed(int seed);

    /// Get value of seed.
    static int GetSeed();

    ///  Generate and return the seed based upon system date and time.
    static int MakeSeed();

    ///  Define all the static values.
    static void SetDefaults(int nstreams, int seed, int gen, int param);

    ///  Define whether the class should use logging or another output
    ///  stream for reporting information and errors.
    static void SetLogging(bool b);

    ///  Define the output stream to use when logging is not used for
    ///  reporting information and errors.  Defaults to cout.
    static void SetLogStream(std::ostream* s);

  private:

    // Copy constructor and assignment operator are private and
    // undefined to avoid problems due to sharing pointers.
    TRandom(const TRandom& random);
    TRandom& operator=(const TRandom& random);
 
    //  The random stream id.
    int fId;

    //  Each random stream has a handle.
    unsigned short fStream[3];

    //  The class has a maximum number of streams.
    static int fgStreams;

    //  The class uses a seed.
    static int fgSeed;

    //  The class uses a generator.
    static int fgGenerator;

    //  The class uses a parameter. (Parameter definition depends on
    //  generator.)
    static int fgParam;

    //  If true, the class will use the logging mechanism for
    //  reporting information and errors.
    static bool fgUseLogging;

    //  The output stream to use when logging is not used for
    //  reporting information and errors.  Defaults to cout.
    static std::ostream* fgLogStream;

    // The module ID for logging.
    static int fgModuleId;
  };


  

} // namespace

#endif // NISAC_RANDOM_RANDOM_H

//---------------------------------------------------------------------
//---------------------------------------------------------------------
