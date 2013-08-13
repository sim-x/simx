/**
 * \file random.h
 * \brief Header file for random number generators.
 *
 * This header file contains the definition of the random class; Users
 * do not need to include this header file directly; it is included
 * from ssf.h.
 */

#ifndef __MINISSF_RANDOM_H__
#define __MINISSF_RANDOM_H__

namespace minissf {

/**
 * \brief Random number and random variate generators.
 *
 * This is the base class of a random number generator, which contains
 * all necessary methods for generating different random variates,
 * such as exponential, erlang, pareto, etc. This is a virtual class;
 * different random generators will derive from this class.
 */
class Random {
 public:
  static int global_seed;

  /** \brief The destructor, which does nothing here, but needed by
      the compiler. */
  virtual ~Random() {}

  /** \brief Return a random number uniformly distributed between 0 and 1. */
  virtual double operator()() = 0;

  /** \brief Set the random seed; if 0, pick one using the system clock. */
  virtual void setSeed(int seed) = 0;

  /**
   * \brief Spawns more random streams from the current one.
   * \param n is the number of random streams to be spawned
   * \param streams a list of Random objects each corresponding to a new random stream
   *
   * The returned type is a pointer array of size n. The array shall
   * be reclaimed by the user afterwards.
   */
  virtual void spawnStreams(int n, Random** streams) = 0;

  /**
   * \brief Returns a random number from a uniform(a,b) distribution.
   * \param a is the lower limit.
   * \param b is the upper limit.
   *
   * The random variable has a range of a < y < b, a mean of (a+b)/2,
   * and a variance of (b-a)*(b-a)/12.
   */
  double uniform(double a = 0, double b = 1.0);

  /**
   * \brief Returns a random number from an exponential distribution.
   * \param x is the rate (i.e., 1/x is the mean).
   *
   * The range of the random variable is y > 0. The mean is
   * 1/x and the variance is 1/x^2.
   */
  double exponential(double x);

  /**
   * \brief Returns a random number from an Erlang distribution.
   * \param n is the number of stages.
   * \param x is the rate of each exponential stage (i.e., 1/x is the mean).
   *
   * Recall that Erlang distribution is the sum of iid exponential
   * distributions.  The range of the random variable is y > 0. The
   * mean is n/x and the variance is n/x^2.
   */
  double erlang(long n, double x);

  /**
   * \brief Returns a random number from a Pareto distribution.
   * \param k is the scale parameter.
   * \param a is the shape parameter.
   *
   * The probability density function is f(x) = a*k^a/x^(a+1) for x
   * >= k. The range of the random variable is y > k. The mean is
   * a*k/(a-1) a>1 and the variance a*k^2/((a-1)^2*(a-2)) for a>2.
   */
  double pareto(double k, double a);

  /**
   * \brief Returns a random number from a Normal(m, s) distribution.
   * \param m is the mean of the normal distribution.
   * \param s is the standard deviation.
   *
   * The range of the random variable is all real numbers.
   */
  double normal(double m = 0, double s = 1.0);

  /**
   * \brief Returns a random number from a log normal distribution.
   * \param m is the mean of the normal distribution.
   * \param s is the standard deviation of the normal distribution.
   *
   * The logarithm of a log normal random variable has a normal
   * distribution. Log normal distribution can be thought of as the
   * product of a large number of iid variables (in the same way
   * that a normal distribution is the sum of a large number of iid
   * variables).  The range of the random variable is y>0. The mean
   * is exp(m+0.5*s*s) and the variance is
   * (exp(s*s)-1)*exp(2*m+s*s).
   */
  double lognormal(double m, double s);

  /**
   * \brief Returns a random number from a chi-square distribution.
   * \param n is the degree of freedom.
   *
   * Recall that a chi-square distribution is the sum of independent
   * Normal(0,1) distributions squared. The range of this random
   * variable is y>0, with a mean n and a variance 2*n.
   */
  double chisquare(long n);

  /**
   * \brief Returns a random number from a student's t-distribution.
   * \param n is the degree of freedom.
   *
   * The range of the random variable is all real numbers. The mean
   * is 0 (n>1) and the variance n/(n-2) (n > 2).
   */
  double student(long n);

  /**
   * \brief Returns a random number from a Bernoulli distribution.
   * \param p is the probability of getting a head from a coin toss.
   *
   * The range is {0,1}. The mean is p and the variance is p*(1-p).
   */
  long bernoulli(double p = 0.5);

  /**
   * \brief Chooses a random number equally likely from a set of
   * integers ranging from a to b.
   *
   * The range is {a, a+1, ..., b}. The mean is (a+b)/2 and the
   * variance is variance ((b-a+1)*(b-a+1)-1)/12.
   */
  long equilikely(long a, long b);

  /**
   * \brief Returns a random number from a Binomial distribution.
   * \param n is the number of coin tosses.
   * \param p is the probability of getting a head for a coin toss.
   *
   * The Binomial distribution is the sum of n Bernoulli
   * distributions. The range of the random variable is {0, 1, ...,
   * n}. The mean is n*p and the variance is n*p*(1-p).
   */
  long binomial(long n, double p);

  /**
   * \brief Returns a random number from a geometric distribution.
   * \param p is the probability of getting a head for a coin toss.
   *
   * The geometric distribution is the number of coin tosses before
   * a head shows up.  The range is all natural numbers {1, 2,
   * ...}. The mean is 1/p and the variance is (1-p)/(p*p).
   */
  long geometric(double p);

  /**
   * \brief Returns a random number from a Pascal distribution.
   * \param n is the number of heads we'd like to see.
   * \param p is the probability of getting a head for a coin toss.
   *
   * The Pascal distribution, also known as negative binomial
   * distribution, is the total number of tails until n heads show
   * up. That is, the probability of y=k means we give the
   * probability of n-1 heads and k failures in k+n-1 trials, and
   * success on the (k+n)th trial. The range is all natural numbers
   * {1, 2, ...}. The mean is n/p and the variance is
   * n*(1-p)/(p*p). Note that Pascal(1, p) is identical to
   * geometric(p).
   */
  long pascal(long n, double p);

  /**
   * \brief Returns a random number from a Poisson distribution.
   * \param m is the mean and the variance.
   *
   * The Poisson distribution has a range of all non-negative
   * integers {0, 1, ...}. The mean is m and the variance is also m.
   */
  long poisson(double m);

  /**
   * \brief Return a random permutation of n integers.
   * \param n the size of the array
   * \param array the array that contains a list of long integers to be permuted
   */
  void permute(long n, long* array);

  //protected:
  static int make_new_seed(); // make one from current machine time

 protected:
  // calculate log(n!), used by poisson()
  double logfactorial(int n);

}; /*class Random*/

}; /*namespace minissf*/

#include "random/lehmer.h"
#include "random/mersenne_twister.h"
#include "random/sprng.h"
#include "random/lecuyer.h"

#endif /*__MINISSF_RANDOM_H__*/

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
