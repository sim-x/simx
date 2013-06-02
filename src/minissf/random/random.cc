#include <math.h>
#include <time.h>
#include "random/random.h"
#include "kernel/throwable.h"

namespace minissf {

int Random::global_seed = 0;

double Random::uniform(double a, double b)
{
  if(a > b) SSF_THROW("invalid arguments: a=" << a << ", b=" << b);
  if(a == b) return a;
  else return a+(b-a)*(*this)();
}

double Random::exponential(double x)
{
  if(x <= 0) SSF_THROW("invalid argument: x=" << x);
  double result = -1.0/x*log(1.0-(*this)());
  return result;
}

double Random::erlang(long n, double x)
{  
  if(n <= 0 || x <= 0) 
    SSF_THROW("invalid arguments: n=" << n << ", x=" << x);
  double y = 0.0;
  for(long i=0; i<n; i++) y += exponential(x);
  return y;
}

double Random::pareto(double k, double a)
{
  if(k <= 0 || a <= 0) 
    SSF_THROW("invalid arguments: k=" << k << ", a=" << a);
  return k*pow(1.0-(*this)(), -1.0/a);
}

double Random::normal(double m, double s)
{
  if(s <= 0) SSF_THROW("invalid arguments: m=" << m << ", s=" << s);

  /* Uses a very accurate approximation of the normal idf due to Odeh & Evans, */
  /* J. Applied Statistics, 1974, vol 23, pp 96-97.                            */
  double p0 = 0.322232431088;        double q0 = 0.099348462606;
  double p1 = 1.0;                   double q1 = 0.588581570495;
  double p2 = 0.342242088547;        double q2 = 0.531103462366;
  double p3 = 0.204231210245e-1;     double q3 = 0.103537752850;
  double p4 = 0.453642210148e-4;     double q4 = 0.385607006340e-2;
  double u, t, p, q, z;

  u = (*this)();
  if(u < 0.5) t = sqrt(-2.0*log(u));
  else t = sqrt(-2.0*log(1.0-u));
  p = p0+t*(p1+t*(p2+t*(p3+t*p4)));
  q = q0+t*(q1+t*(q2+t*(q3+t*q4)));
  if(u < 0.5) z = (p/q)-t;
  else z = t-(p/q);
  return m+s*z;
}

double Random::lognormal(double a, double b)
{
  if(b <= 0) SSF_THROW("invalid arguments: a=" << a << ", b=" << b);
  double result = exp(a+b*normal(0.0, 1.0));
  return result;
}

double Random::chisquare(long n)
{ 
  if(n <= 0) SSF_THROW("invalid argument: n=" << n);
  double y = 0.0;
  for(long i=0; i<n; i++) {
    double z = normal(0.0, 1.0);
    y += z*z;
  }
  return y;
}

double Random::student(long n)
{
  if(n <= 0) SSF_THROW("invalid argument: n=" << n);
  double result = normal(0.0, 1.0)/sqrt(chisquare(n)/n);
  return result;
}

long Random::bernoulli(double p)
{
  if(p < 0 || p > 1) 
    SSF_THROW("invalid argument: p=" << p);
  long result = (*this)()<p ? 1 : 0;
  return result;
}

long Random::equilikely(long a, long b)
{
  if(a > b) SSF_THROW("invalid arguments: a=" << a << ", b=" << b);
  double x = (*this)();
  long v = a+(long)floor((b-a+1)*x);
  return v;
}

long Random::binomial(long n, double p)
{ 
  if(n <= 0 || p < 0 || p > 1) 
    SSF_THROW("invalid arguments: n=" << n << ", p=" << p);
  long y = 0;
  for(long i=0; i<n; i++) y += bernoulli(p);
  return y;
}

long Random::geometric(double p)
{
  if(p <= 0 || p >= 1) 
    SSF_THROW("invalid argument: p=" << p);
  long result = 1+(long)(log(1.0-(*this)())/log(1.0-p));
  return result;
}

long Random::pascal(long n, double p)
{ 
  if(n <= 0 || p <= 0 || p >= 1) 
    SSF_THROW("invalid arguments: n=" << n << ", p=" << p);
  long y = 0;
  for(long i=0; i<n; i++) y += geometric(p);
  return y;
}

long Random::poisson(double m)
{ 
  if(m <= 0) SSF_THROW("invalid argument: m=" << m);
  long y = 0;
  double t = exp(m);
  do {
    y++;
    t *= (*this)();
  } while(t >= 1.0);
  y--;
  return y;
}

void Random::permute(long n, long* array)
{
  if(n <= 0 || !array) 
    SSF_THROW("invalid arguments: n=" << n << ", array=" << array);
  int i;
  for(i=0; i<n; i++) array[i] = i;
  for(i=0; i<n-1; i++) {
    long j = equilikely(i, n-1);
    if(i != j) {
      long tmp = array[i];
      array[i] = array[j];
      array[j] = tmp;
    }
  }
}

// copied from SPRNG for generating a random seed from the machine time
int Random::make_new_seed() 
{
  time_t tp;
  struct tm *temp;
  unsigned int temp2, temp3;
  static unsigned int temp4 = 0xe0e1;
  
  time(&tp);
  temp = localtime(&tp);
  temp2 = (temp->tm_sec<<26)+(temp->tm_min<<20)+(temp->tm_hour<<15)+
    (temp->tm_mday<<10)+(temp->tm_mon<<6);
  temp3 = (temp->tm_year<<13)+(temp->tm_wday<<10)+(temp->tm_yday<<1)+
    temp->tm_isdst;
  temp2 ^= clock()^temp3;
  temp4 = (temp4*0xeeee)%0xffff;
  temp2 ^= temp4<<16;
  temp4 = (temp4*0xaeee)%0xffff;
  temp2 ^= temp4;
  temp2 &= 0x7fffffff;
  return temp2;
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
