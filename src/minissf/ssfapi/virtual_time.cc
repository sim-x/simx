#include <stdio.h>
#include <stdlib.h>
#include "ssfapi/ssf_common.h"

namespace minissf {

double VirtualTime::DAY = 8.64e13;
double VirtualTime::HOUR = 3.6e12;
double VirtualTime::MINUTE = 6e10;
double VirtualTime::SECOND = 1e9;
double VirtualTime::MILLISECOND = 1e6;
double VirtualTime::MICROSECOND = 1e3; 
double VirtualTime::NANOSECOND = 1.0;
double VirtualTime::INFINITY = 9.2e18; // close to max long long

void VirtualTime::setPrecision(double precision)
{
  DAY /= precision;
  HOUR /= precision;
  MINUTE /= precision;
  SECOND /= precision;
  MILLISECOND /= precision;
  MICROSECOND /= precision;
  NANOSECOND /= precision;
}

STRING VirtualTime::toString() const
{
  char buf[50];
  sprintf(buf, "%.9lfs", second());
  return buf;
}

void VirtualTime::fromString(const STRING& str)
{
  double unit = SECOND;
  size_t found = str.find_first_not_of("+-0123456789.");
  if (found != STRING::npos) {
    STRING u = str.substr(found);
    if(!u.compare("ns")) unit = NANOSECOND;
    else if(!u.compare("us")) unit = MICROSECOND;
    else if(!u.compare("ms")) unit = MILLISECOND;
    else if(!u.compare("s")) unit = SECOND;
    else if(!u.compare("m")) unit = MINUTE;
    else if(!u.compare("h")) unit = HOUR;
    else if(!u.compare("d")) unit = DAY;
    else SSF_THROW("invalid virtual time format: " << str);
  }
  ticks = (int64)(atof(str.c_str())*unit);
}

OSTREAM& operator<<(OSTREAM &os, const VirtualTime& vt)
{
  os << vt.toString();
  return os;
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
