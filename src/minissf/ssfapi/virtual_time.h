/**
 * \file virtual_time.h
 * \brief Header file for virtual time definition.
 *
 * This header file contains the definition of the virtual time class;
 * Users do not need to include this header file directly; it is
 * included from ssf.h.
 */

#ifndef __MINISSF_VIRTUAL_TIME_H__
#define __MINISSF_VIRTUAL_TIME_H__

#ifndef __MINISSF_COMMON_H__
#error "DO NOT INCLUDE THIS CLASS DIRECTLY"
#endif

#include <math.h>
#include "kernel/timestamp.h"

#ifdef INFINITY
#undef INFINITY
#endif

namespace minissf {

/**
 * \brief Representation of simulation time with units.
 *
 * Virtual time is represented by a 64-bit integer to count the clock
 * ticks from the beginning of a simulation run. Each clock tick is
 * one nanosecond by default. The user can customize the granularity
 * of a clock tick (using the setPrecision() method). We use integers
 * to represent virtual time because the underflow problem of the
 * floating point numbers may produce non-deterministic simulation
 * results, which is considered more serious than the overflow problem
 * of a 64-bit integer.
 */
class VirtualTime {
 public:
  /**
   * \brief Set the granularity of a clock tick.
   *
   * This function is expected to be called at most once, and if so it
   * must be called before virtual time is ever used by the
   * simulator. By default, the precision is NANOSECOND, which means
   * one clock tick represents one nanosecond. In most simulation
   * scenarios, there's no need to call this function; the default is
   * good enough.
   */
  static void setPrecision(double precision);

  /** @name Commonly used constants for simulation time. */
  /** @{ */
  static double DAY; ///< One day in simulation time.
  static double HOUR; ///< One hour in simulation time.
  static double MINUTE; ///< One minute in simulation time.
  static double SECOND; ///< One second in simulation time.
  static double MILLISECOND; ///< One millisecond in simulation time.
  static double MICROSECOND; ///< One microsecond in simulation time.
  static double NANOSECOND; ///< One nanosecond in simulation time.
  static double INFINITY; ///< Infinite simulation time.
  /** @} */

  /**@name Constructors from different types.
   *
   * An optional time unit can be provided as the second argument,
   * which can be DAY, HOUR, MINUTE, SECOND, MILLISECOND, MICROSECOND,
   * or NANOSECOND. The default is NANOSECOND. The newly created
   * virtual time object will be in clock ticks.
   */
  /** @{ */

  /** \brief The default constructor for simulation time 0. */
  VirtualTime() : ticks(0) {}

  VirtualTime(const double x, double unit = 1.0) : ticks((int64)(x*unit)) {} ///< Convert from double.
  VirtualTime(const float x, double unit = 1.0) : ticks((int64)(x*unit)) {} ///< Convert from float.
  VirtualTime(const int x, double unit = 1.0) : ticks((int64)(x*unit)) {} ///< Convert from integer.
  VirtualTime(const long x, double unit = 1.0) : ticks((int64)(x*unit)) {} ///< Convert from long integer.
#ifdef HAVE_LONG_LONG_INT
  VirtualTime(const long long x, double unit = 1.0) : ticks((int64)(x*unit)) {} ///< Convert from long long integer.
#endif
  /** 
   * \brief Convert from a timestamp object. 
   *
   * A timestamp is used internally by minissf to represent virtual
   * time with addition tie-breaking fields to sort simultaneous events.
   */
  VirtualTime(const Timestamp& ts) : ticks(ts.key1) {}
  VirtualTime(const STRING& str) { fromString(str); } ///< Convert from STL string.
  VirtualTime(const char* str) { STRING s(str); fromString(s); } ///< Convert from a c-style character string.

  /** \brief The copy constructor. */
  VirtualTime(const VirtualTime& vt) : ticks(vt.ticks) {}

  /**@name Convert to normal time unit.
   *
   * Convert from a clock tick to a normal time unit, represented as a
   * double floating point number.
   */
  /** @{ */
  double second() const { return double(ticks)/SECOND; } ///< Convert to seconds.
  double millisecond() const { return double(ticks)/MILLISECOND; } ///< Convert to milliseconds.
  double microsecond() const { return double(ticks)/MICROSECOND; } ///< Convert to microseconds.
  double nanosecond() const { return double(ticks)/NANOSECOND; } ///< Convert to nanoseconds.

  /**@name Type conversion.
   * 
   * Return the number of clock ticks (nanoseconds or whatever the
   * clock granularity is set to be) in different primitive types.
   */
  /** @{ */
  operator double() const { return (double)ticks; } ///< Convert to double.
  operator float() const { return (float)ticks; } ///< Convert to float.
  operator int() const { return (int)ticks; } ///< Convert to integer.
  operator long() const { return (long)ticks; } ///< Convert to long integer.
#ifdef HAVE_LONG_LONG_INT
  operator long long() const { return (long long)ticks; } ///< Convert to long long integer.
#endif
  /** @} */

  /**@name Assignment operators.
   *
   * Assignment operations with right-hand side of different
   * types. The variable at the right-hand side of the assignment
   * operator is assumed to be the number of clock ticks.
   */
  /** @{ */
  VirtualTime operator = (const VirtualTime& vt) { ticks = vt.ticks; return *this; } ///< Assign from another virtual time.
  VirtualTime operator = (const double x) { ticks = (int64)x; return *this; } ///< Assign from a double.
  VirtualTime operator = (const float x) { ticks = (int64)x; return *this; } ///< Assign from a float.
  VirtualTime operator = (const int x) { ticks = (int64)x; return *this; } ///< Assign from an integer.
  VirtualTime operator = (const long x) { ticks = (int64)x; return *this; } ///< Assign from a long integer.
#ifdef HAVE_LONG_LONG_INT
  VirtualTime operator = (const long long x) { ticks = (int64)x; return *this; } ///< Assign from a long long integer.
#endif
  VirtualTime operator = (const STRING& str) { fromString(str); return *this; } ///< Assign from an STL string.

  VirtualTime operator += (const VirtualTime& vt) { ticks += vt.ticks; return *this; } ///< Plus-equal another virtual time.
  VirtualTime operator += (const double x) { ticks += (int64)x; return *this; } ///< Plus-equal a double.
  VirtualTime operator += (const float x) { ticks += (int64)x; return *this; } ///< Plus-equal a float.
  VirtualTime operator += (const int x) { ticks += (int64)x; return *this; } ///< Plus-equal an integer.
  VirtualTime operator += (const long x) { ticks += (int64)x; return *this; } ///< Plus-equal a long integer.
#ifdef HAVE_LONG_LONG_INT
  VirtualTime operator += (const long long x) { ticks += (int64)x; return *this; } ///< Plus-equal a long long integer.
#endif

  VirtualTime operator -= (const VirtualTime& vt) { ticks -= vt.ticks; return *this; } ///< Minus-equal another virtual time.
  VirtualTime operator -= (const double x) { ticks -= (int64)x; return *this; } ///< Minus-equal a double.
  VirtualTime operator -= (const float x) { ticks -= (int64)x; return *this; } ///< Minus-equal a float.
  VirtualTime operator -= (const int x) { ticks -= (int64)x; return *this; } ///< Minus-equal an integer.
  VirtualTime operator -= (const long x) { ticks -= (int64)x; return *this; } ///< Minus-equal a long integer.
#ifdef HAVE_LONG_LONG_INT
  VirtualTime operator -= (const long long x) { ticks -= (int64)x; return *this; } ///< Minus-equal a long long integer.
#endif

  //VirtualTime operator *= (const VirtualTime& vt) { ticks *= vt.ticks; return *this; } // does not make sense to multiply two virtual time and end up as another virtual time (it should return double)!
  VirtualTime operator *= (const double x) { ticks = (int64)((double)ticks*x); return *this; } ///< Times-equal a double.
  VirtualTime operator *= (const float x) { ticks = (int64)((double)ticks*x); return *this; } ///< Times-equal a float.
  VirtualTime operator *= (const int x) { ticks = (int64)(ticks*x); return *this; } ///< Times-equal an integer.
  VirtualTime operator *= (const long x) { ticks = (int64)(ticks*x); return *this; } ///< Times-equal a long integer.
#ifdef HAVE_LONG_LONG_INT
  VirtualTime operator *= (const long long x) { ticks = (int64)(ticks*x); return *this; } ///< Times-equal a long long integer.
#endif

  //VirtualTime operator /= (const VirtualTime& vt) { ticks /= vt.ticks; return *this; } // does not make sense to divide two virtual time and end up as another virtual time (it should return double)!
  VirtualTime operator /= (const double x) { ticks = (int64)((double)ticks/x); return *this; } ///< Divide-equal a double.
  VirtualTime operator /= (const float x) { ticks = (int64)((double)ticks/x); return *this; } ///< Divide-equal a float.
  VirtualTime operator /= (const int x) { ticks = (int64)(ticks/x); return *this; } ///< Divide-equal an integer.
  VirtualTime operator /= (const long x) { ticks = (int64)(ticks/x); return *this; } ///< Divide-equal a long integer.
#ifdef HAVE_LONG_LONG_INT
  VirtualTime operator /= (const long long x) { ticks = (int64)(ticks/x); return *this; } ///< Divide-equal a long long integer.
#endif
  /** @} */

  /**@name Other mathematical operators.
   * 
   * The operators are operating on the clock ticks.
   */
  /** @{ */

  VirtualTime operator - () const { return VirtualTime(-ticks); } ///< Unary negative operator.

  friend VirtualTime operator + (const VirtualTime& vt1, const VirtualTime& vt2) { return VirtualTime(vt1.ticks+vt2.ticks); } ///< Add two virtual times.
  friend VirtualTime operator + (const VirtualTime& vt1, const double x) { return VirtualTime(vt1.ticks+(int64)x); } ///< Add a virtual time with a double.
  friend VirtualTime operator + (const VirtualTime& vt1, const float x) { return VirtualTime(vt1.ticks+(int64)x); } ///< Add a virtual time with a float.
  friend VirtualTime operator + (const VirtualTime& vt1, const int x) { return VirtualTime(vt1.ticks+(int64)x); } ///< Add a virtual time with an integer.
  friend VirtualTime operator + (const VirtualTime& vt1, const long x) { return VirtualTime(vt1.ticks+(int64)x); } ///< Add a virtual time with a long integer.
#ifdef HAVE_LONG_LONG_INT
  friend VirtualTime operator + (const VirtualTime& vt1, const long long x) { return VirtualTime(vt1.ticks+(int64)x); } ///< Add a virtual time with a long long integer.
#endif
  friend VirtualTime operator + (const double x, const VirtualTime& vt2) { return VirtualTime((int64)x+vt2.ticks); } ///< Add a double and a virtual time.
  friend VirtualTime operator + (const float x, const VirtualTime& vt2) { return VirtualTime((int64)x+vt2.ticks); } ///< Add a float and a virtual time.
  friend VirtualTime operator + (const int x, const VirtualTime& vt2) { return VirtualTime((int64)x+vt2.ticks); } ///< Add an integer and a virtual time.
  friend VirtualTime operator + (const long x, const VirtualTime& vt2) { return VirtualTime((int64)x+vt2.ticks); } ///< Add a long integer and a virtual time.
#ifdef HAVE_LONG_LONG_INT
  friend VirtualTime operator + (const long long x, const VirtualTime& vt2) { return VirtualTime((int64)x+vt2.ticks); } ///< Add a long long integer and a virtual time.
#endif

  friend VirtualTime operator - (const VirtualTime& vt1, const VirtualTime& vt2) { return VirtualTime(vt1.ticks-vt2.ticks); } ///< Subtract two virtual times.
  friend VirtualTime operator - (const VirtualTime& vt1, const double x) { return VirtualTime(vt1.ticks-(int64)x); } ///< Subtract a double from a virtual time.
  friend VirtualTime operator - (const VirtualTime& vt1, const float x) { return VirtualTime(vt1.ticks-(int64)x); } ///< Subtract a float from a virtual time.
  friend VirtualTime operator - (const VirtualTime& vt1, const int x) { return VirtualTime(vt1.ticks-(int64)x); } ///< Subtract an integer from a virtual time.
  friend VirtualTime operator - (const VirtualTime& vt1, const long x) { return VirtualTime(vt1.ticks-(int64)x); } ///< Subtract a long integer from a virtual time.
#ifdef HAVE_LONG_LONG_INT
  friend VirtualTime operator - (const VirtualTime& vt1, const long long x) { return VirtualTime(vt1.ticks-(int64)x); } ///< Subtract a long long integer from a virtual time.
#endif
  friend VirtualTime operator - (const double x, const VirtualTime& vt2) { return VirtualTime((int64)x-vt2.ticks); } ///< Substract a virtual time from a double.
  friend VirtualTime operator - (const float x, const VirtualTime& vt2) { return VirtualTime((int64)x-vt2.ticks); } ///< Substract a virtual time from a float.
  friend VirtualTime operator - (const int x, const VirtualTime& vt2) { return VirtualTime((int64)x-vt2.ticks); } ///< Substract a virtual time from an integer.
  friend VirtualTime operator - (const long x, const VirtualTime& vt2) { return VirtualTime((int64)x-vt2.ticks); } ///< Substract a virtual time from a long integer.
#ifdef HAVE_LONG_LONG_INT
  friend VirtualTime operator - (const long long x, const VirtualTime& vt2) { return VirtualTime((int64)x-vt2.ticks); } ///< Substract a virtual time from a long long integer.
#endif

  friend double operator * (const VirtualTime& vt1, const VirtualTime& vt2) { return (double)vt1.ticks*(double)vt2.ticks; } ///< Multiplying two virtual times returns a double.
  friend VirtualTime operator * (const VirtualTime& vt1, const double x) { return VirtualTime((double)vt1.ticks*x); } ///< Multiply a virtual time and a double.
  friend VirtualTime operator * (const VirtualTime& vt1, const float x) { return VirtualTime((float)vt1.ticks*x); } ///< Multiply a virtual time and a float.
  friend VirtualTime operator * (const VirtualTime& vt1, const int x) { return VirtualTime(vt1.ticks*x); } ///< Multiply a virtual time and an integer.
  friend VirtualTime operator * (const VirtualTime& vt1, const long x) { return VirtualTime(vt1.ticks*x); } ///< Multiply a virtual time and a long integer.
#ifdef HAVE_LONG_LONG_INT
  friend VirtualTime operator * (const VirtualTime& vt1, const long long x) { return VirtualTime(vt1.ticks*x); } ///< Multiply a virtual time and a long long integer.
#endif
  friend VirtualTime operator * (const double x, const VirtualTime& vt2) { return VirtualTime(x*(double)vt2.ticks); } ///< Multiply a double and a virtual time.
  friend VirtualTime operator * (const float x, const VirtualTime& vt2) { return VirtualTime(x*(float)vt2.ticks); } ///< Multiply a float and a virtual time.
  friend VirtualTime operator * (const int x, const VirtualTime& vt2) { return VirtualTime(x*vt2.ticks); } ///< Multiply an integer and a virtual time.
  friend VirtualTime operator * (const long x, const VirtualTime& vt2) { return VirtualTime(x*vt2.ticks); } ///< Multiply a long integer and a virtual time.
#ifdef HAVE_LONG_LONG_INT
  friend VirtualTime operator * (const long long x, const VirtualTime& vt2) { return VirtualTime(x*vt2.ticks); } ///< Multiply a long long integer and a virtual time.
#endif

  friend double operator / (const VirtualTime& vt1, const VirtualTime& vt2) { return (double)vt1.ticks/(double)vt2.ticks; } ///< Dividing two virtual time returns a double.
  friend VirtualTime operator / (const VirtualTime& vt1, const double x) { return VirtualTime((double)vt1.ticks/x); } ///< Divide a double from a virtual time.
  friend VirtualTime operator / (const VirtualTime& vt1, const float x) { return VirtualTime((float)vt1.ticks/x); } ///< Divide a float from a virtual time.
  friend VirtualTime operator / (const VirtualTime& vt1, const int x) { return VirtualTime(vt1.ticks/(int64)x); } ///< Divide an integer from a virtual time.
  friend VirtualTime operator / (const VirtualTime& vt1, const long x) { return VirtualTime(vt1.ticks/(int64)x); } ///< Divide a long integer from a virtual time.
#ifdef HAVE_LONG_LONG_INT
  friend VirtualTime operator / (const VirtualTime& vt1, const long long x) { return VirtualTime(vt1.ticks/(int64)x); } ///< Divide a long long integer from a virtual time.
#endif
  friend double operator / (const double x, const VirtualTime& vt2) { return x/(double)vt2.ticks; } ///< Divide a virtual time from a double.
  friend float operator / (const float x, const VirtualTime& vt2) { return x/(float)vt2.ticks; } ///< Divide a virtual time from a float.
  friend int operator / (const int x, const VirtualTime& vt2) { return (int64)x/vt2.ticks; } ///< Divide a virtual time from an integer.
  friend long operator / (const long x, const VirtualTime& vt2) { return (int64)x/vt2.ticks; } ///< Divide a virtual time from a long integer.
#ifdef HAVE_LONG_LONG_INT
  friend long long operator / (const long long x, const VirtualTime& vt2) { return (int64)x/vt2.ticks; } ///< Divide a virtual time from a long long integer.
#endif
  /** @} */

  /**@name Comparison operators. */
  //@{
  friend int operator == (const VirtualTime& vt1, const VirtualTime& vt2) { return vt1.ticks == vt2.ticks; } ///< Comparing two virtual times.
  friend int operator == (const VirtualTime& vt1, const double x) { return vt1.ticks == (int64)x; } ///< Comparing a virtual time and a double.
  friend int operator == (const VirtualTime& vt1, const float x) { return vt1.ticks == (int64)x; } ///< Comparing a virtual time and a float.
  friend int operator == (const VirtualTime& vt1, const int x) { return vt1.ticks == (int64)x; } ///< Comparing a virtual time and an integer.
  friend int operator == (const VirtualTime& vt1, const long x) { return vt1.ticks == (int64)x; } ///< Comparing a virtual time and a long integer.
#ifdef HAVE_LONG_LONG_INT
  friend int operator == (const VirtualTime& vt1, const long long x) { return vt1.ticks == (int64)x; } ///< Comparing a virtual time and a long long integer.
#endif
  friend int operator == (const double x, const VirtualTime& vt2) { return (int64)x == vt2.ticks; } ///< Comparing a double and a virtual time.
  friend int operator == (const float x, const VirtualTime& vt2) { return (int64)x == vt2.ticks; } ///< Comparing a float and a virtual time.
  friend int operator == (const int x, const VirtualTime& vt2) { return (int64)x == vt2.ticks; } ///< Comparing an integer and a virtual time.
  friend int operator == (const long x, const VirtualTime& vt2) { return (int64)x == vt2.ticks; } ///< Comparing a long integer and a virtual time.
#ifdef HAVE_LONG_LONG_INT
  friend int operator == (const long long x, const VirtualTime& vt2) { return (int64)x == vt2.ticks; } ///< Comparing a long long integer and a virtual time.
#endif
  
  friend int operator != (const VirtualTime& vt1, const VirtualTime& vt2) { return vt1.ticks != vt2.ticks; } ///< Comparing two virtual times.
  friend int operator != (const VirtualTime& vt1, const double x) { return vt1.ticks != (int64)x; } ///< Comparing a virtual time and a double.
  friend int operator != (const VirtualTime& vt1, const float x) { return vt1.ticks != (int64)x; } ///< Comparing a virtual time and a float.
  friend int operator != (const VirtualTime& vt1, const int x) { return vt1.ticks != (int64)x; } ///< Comparing a virtual time and an integer.
  friend int operator != (const VirtualTime& vt1, const long x) { return vt1.ticks != (int64)x; } ///< Comparing a virtual time and a long integer.
#ifdef HAVE_LONG_LONG_INT
  friend int operator != (const VirtualTime& vt1, const long long x) { return vt1.ticks != (int64)x; } ///< Comparing a virtual time and a long long integer.
#endif
  friend int operator != (const double x, const VirtualTime& vt2) { return (int64)x != vt2.ticks; } ///< Comparing a double and a virtual time.
  friend int operator != (const float x, const VirtualTime& vt2) { return (int64)x != vt2.ticks; } ///< Comparing a float and a virtual time.
  friend int operator != (const int x, const VirtualTime& vt2) { return (int64)x != vt2.ticks; } ///< Comparing an integer and a virtual time.
  friend int operator != (const long x, const VirtualTime& vt2) { return (int64)x != vt2.ticks; } ///< Comparing a long integer and a virtual time.
#ifdef HAVE_LONG_LONG_INT
  friend int operator != (const long long x, const VirtualTime& vt2) { return (int64)x != vt2.ticks; } ///< Comparing a long long integer and a virtual time.
#endif

  friend int operator >= (const VirtualTime& vt1, const VirtualTime& vt2) { return vt1.ticks >= vt2.ticks; } ///< Comparing two virtual times.
  friend int operator >= (const VirtualTime& vt1, const double x) { return vt1.ticks >= (int64)x; } ///< Comparing a virtual time and a double.
  friend int operator >= (const VirtualTime& vt1, const float x) { return vt1.ticks >= (int64)x; } ///< Comparing a virtual time and a float.
  friend int operator >= (const VirtualTime& vt1, const int x) { return vt1.ticks >= (int64)x; } ///< Comparing a virtual time and an integer.
  friend int operator >= (const VirtualTime& vt1, const long x) { return vt1.ticks >= (int64)x; } ///< Comparing a virtual time and a long integer.
#ifdef HAVE_LONG_LONG_INT
  friend int operator >= (const VirtualTime& vt1, const long long x) { return vt1.ticks >= (int64)x; } ///< Comparing a virtual time and a long long integer.
#endif
  friend int operator >= (const double x, const VirtualTime& vt2) { return (int64)x >= vt2.ticks; } ///< Comparing a double and a virtual time.
  friend int operator >= (const float x, const VirtualTime& vt2) { return (int64)x >= vt2.ticks; } ///< Comparing a float and a virtual time.
  friend int operator >= (const int x, const VirtualTime& vt2) { return (int64)x >= vt2.ticks; } ///< Comparing an integer and a virtual time.
  friend int operator >= (const long x, const VirtualTime& vt2) { return (int64)x >= vt2.ticks; } ///< Comparing a long integer and a virtual time.
#ifdef HAVE_LONG_LONG_INT
  friend int operator >= (const long long x, const VirtualTime& vt2) { return (int64)x >= vt2.ticks; } ///< Comparing a long long integer and a virtual time.
#endif

  friend int operator <= (const VirtualTime& vt1, const VirtualTime& vt2) { return vt1.ticks <= vt2.ticks; } ///< Comparing two virtual times.
  friend int operator <= (const VirtualTime& vt1, const double x) { return vt1.ticks <= (int64)x; } ///< Comparing a virtual time and a double.
  friend int operator <= (const VirtualTime& vt1, const float x) { return vt1.ticks <= (int64)x; } ///< Comparing a virtual time and a float.
  friend int operator <= (const VirtualTime& vt1, const int x) { return vt1.ticks <= (int64)x; } ///< Comparing a virtual time and an integer.
  friend int operator <= (const VirtualTime& vt1, const long x) { return vt1.ticks <= (int64)x; } ///< Comparing a virtual time and a long integer.
#ifdef HAVE_LONG_LONG_INT
  friend int operator <= (const VirtualTime& vt1, const long long x) { return vt1.ticks <= (int64)x; } ///< Comparing a virtual time and a long long integer.
#endif
  friend int operator <= (const double x, const VirtualTime& vt2) { return (int64)x <= vt2.ticks; } ///< Comparing a double and a virtual time.
  friend int operator <= (const float x, const VirtualTime& vt2) { return (int64)x <= vt2.ticks; } ///< Comparing a float and a virtual time.
  friend int operator <= (const int x, const VirtualTime& vt2) { return (int64)x <= vt2.ticks; } ///< Comparing an integer and a virtual time.
  friend int operator <= (const long x, const VirtualTime& vt2) { return (int64)x <= vt2.ticks; } ///< Comparing a long integer and a virtual time.
#ifdef HAVE_LONG_LONG_INT
  friend int operator <= (const long long x, const VirtualTime& vt2) { return (int64)x <= vt2.ticks; } ///< Comparing a long long integer and a virtual time.
#endif

  friend int operator > (const VirtualTime& vt1, const VirtualTime& vt2) { return vt1.ticks > vt2.ticks; } ///< Comparing two virtual times.
  friend int operator > (const VirtualTime& vt1, const double x) { return vt1.ticks > (int64)x; } ///< Comparing a virtual time and a double.
  friend int operator > (const VirtualTime& vt1, const float x) { return vt1.ticks > (int64)x; } ///< Comparing a virtual time and a float.
  friend int operator > (const VirtualTime& vt1, const int x) { return vt1.ticks > (int64)x; } ///< Comparing a virtual time and an integer.
  friend int operator > (const VirtualTime& vt1, const long x) { return vt1.ticks > (int64)x; } ///< Comparing a virtual time and a long integer.
#ifdef HAVE_LONG_LONG_INT
  friend int operator > (const VirtualTime& vt1, const long long x) { return vt1.ticks > (int64)x; } ///< Comparing a virtual time and a long long integer.
#endif
  friend int operator > (const double x, const VirtualTime& vt2) { return (int64)x > vt2.ticks; } ///< Comparing a double and a virtual time.
  friend int operator > (const float x, const VirtualTime& vt2) { return (int64)x > vt2.ticks; } ///< Comparing a float and a virtual time.
  friend int operator > (const int x, const VirtualTime& vt2) { return (int64)x > vt2.ticks; } ///< Comparing an integer and a virtual time.
  friend int operator > (const long x, const VirtualTime& vt2) { return (int64)x > vt2.ticks; } ///< Comparing a long integer and a virtual time.
#ifdef HAVE_LONG_LONG_INT
  friend int operator > (const long long x, const VirtualTime& vt2) { return (int64)x > vt2.ticks; } ///< Comparing a long long integer and a virtual time.
#endif

  friend int operator < (const VirtualTime& vt1, const VirtualTime& vt2) { return vt1.ticks < vt2.ticks; } ///< Comparing two virtual times.
  friend int operator < (const VirtualTime& vt1, const double x) { return vt1.ticks < (int64)x; } ///< Comparing a virtual time and a double.
  friend int operator < (const VirtualTime& vt1, const float x) { return vt1.ticks < (int64)x; } ///< Comparing a virtual time and a float.
  friend int operator < (const VirtualTime& vt1, const int x) { return vt1.ticks < (int64)x; } ///< Comparing a virtual time and an integer.
  friend int operator < (const VirtualTime& vt1, const long x) { return vt1.ticks < (int64)x; } ///< Comparing a virtual time and a long integer.
#ifdef HAVE_LONG_LONG_INT
  friend int operator < (const VirtualTime& vt1, const long long x) { return vt1.ticks < (int64)x; } ///< Comparing a virtual time and a long long integer.
#endif
  friend int operator < (const double x, const VirtualTime& vt2) { return (int64)x < vt2.ticks; } ///< Comparing a double and a virtual time.
  friend int operator < (const float x, const VirtualTime& vt2) { return (int64)x < vt2.ticks; } ///< Comparing a float and a virtual time.
  friend int operator < (const int x, const VirtualTime& vt2) { return (int64)x < vt2.ticks; } ///< Comparing an integer and a virtual time.
  friend int operator < (const long x, const VirtualTime& vt2) { return (int64)x < vt2.ticks; } ///< Comparing a long integer and a virtual time.
#ifdef HAVE_LONG_LONG_INT
  friend int operator < (const long long x, const VirtualTime& vt2) { return (int64)x < vt2.ticks; } ///< Comparing a long long integer and a virtual time.
#endif
  /** @} */

  /** \brief Convert from a string. */
  void fromString(const STRING& str); 

  /** \brief Convert to a string. */
  STRING toString() const;

  /** \brief The << operator can pipe the virtual time to an output stream. */
  friend OSTREAM& operator<<(OSTREAM &os, const VirtualTime& x);
  
  /** \brief Return the clock tick as is. */
  inline int64 get_ticks() const { return ticks; }

  /** \brief Set the clock tick as given. */
  inline void set_ticks(int64 tk) { ticks = tk; }

 protected:
  int64 ticks; // the clock tick at highest simulation time precision (in nanoseconds by default)
}; /*class VirtualTime*/

}; /*namespace minissf*/

#endif /*__MINISSF_VIRTUAL_TIME_H__*/

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
