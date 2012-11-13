//--------------------------------------------------------------------------
// $Id: Timer.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    Timer.h
// Module:  Common
// Author:  Keith Bisset
// Created: January 28 1997
// Description: Compute elapsed time.
//
// @COPYRIGHT@
//
//--------------------------------------------------------------------------

#ifndef NISAC_COMMON_TIMER
#define NISAC_COMMON_TIMER

#include "simx/Log/Logger.h"
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <boost/cast.hpp>

//--------------------------------------------------------------------------

namespace Common {
  
  /// \class Timer Timer.h "Common/Timer.h"
  ///
  /// \brief Compute elapsed time.
  class Timer
  {
  public:
    /// Constructor.  If desc is provides, it will be output along with
    /// the Timer state when the Timer object is destroyed.
    Timer(const std::string& desc="");
    ~Timer();

    /// Start the timer.
    void Start();
  
    /// Stop the timer.
    void Stop();

    /// Reset all counters to 0.
    void Reset();
  
    /// Return the total time during which the timer was running.
    float Elapsed() const;

    /// Number of times Stop() is called.
    int Count() const;
  
    void Pause();
    void Resume();

    /// Print the current state of the counter to the stream.
    void Print(std::ostream& o) const;

    /// Return the system time.
    double CurrentTime() const;

    /// Return true if the timer is running (started), otherwise return
    /// false (stopped).
    bool IsRunning() const;

  private:
    std::string fDesc;
    int time_offset;
    float start_time;
    float elapsed;
    bool isRunning;
    int count;
  };

  //--------------------------------------------------------------------------

  /// Timer stream insertion operator
  inline std::ostream& operator<<(std::ostream& os, const Timer& t)
  {
    t.Print(os);
    return os;
  }

  //--------------------------------------------------------------------------

  inline Timer::Timer(const std::string& desc)
    : fDesc(desc),
      time_offset(0),
      start_time(0.0),
      elapsed(0.0),
      isRunning(false),
      count(0)
  {
    time_offset = boost::numeric_cast<int>( CurrentTime() );
  }

  //--------------------------------------------------------------------------

  inline Timer::~Timer()
  {
    if (fDesc != "")
      Log::log().info(0) << "Timer " << fDesc << std::endl; 
  }

  //--------------------------------------------------------------------------

  inline void Timer::Start()
  {
    static std::string functionName("Timer::Start()");

    if (isRunning == true) {
      Log::log().info(0)
	<< functionName
	<< ": Warning: Timer "
	<< fDesc
	<< " has already been started."
	<<  std::endl; 
    } else {
      start_time = boost::numeric_cast<float>( CurrentTime() );
      isRunning = true;
    }
  }

  //--------------------------------------------------------------------------

  inline void Timer::Stop()
  {
    static std::string functionName("Timer::Stop()");

    if (isRunning == false) {
      Log::log().info(0)
	<< functionName
	<< ": Warning: Timer "
	<< fDesc << " has already been stopped." 
	<<  std::endl; 
    } else {
      elapsed += boost::numeric_cast<float>( CurrentTime() ) - start_time;
      isRunning = false;
      count++;
    }
  }

  //--------------------------------------------------------------------------

  inline bool
  Timer::IsRunning() const
  {
    return isRunning;
  }

  //--------------------------------------------------------------------------

  inline void Timer::Reset()
  {
    elapsed = 0.0;
    start_time = 0;
    count = 0;
  }

  //--------------------------------------------------------------------------

  inline float Timer::Elapsed() const
  {
    static std::string functionName("inline float Timer::Elapsed() const");

    if (isRunning == true) {
      Log::log().info(0) 
	<< functionName
	<< ": Warning: Timer "
	<< fDesc 
	<< " is still running." 
	<<  std::endl; 
      return elapsed + boost::numeric_cast<float>( CurrentTime() ) - start_time;
    }

    return elapsed;
  }

  //--------------------------------------------------------------------------

  inline int Timer::Count() const {return count;}

  //--------------------------------------------------------------------------

  inline double Timer::CurrentTime() const
  {
    timeval tv;
    gettimeofday(&tv, NULL);
    return  boost::numeric_cast<double>( tv.tv_sec - time_offset ) + 
      boost::numeric_cast<double>( tv.tv_usec * 1e-6 );
  }

  //--------------------------------------------------------------------------

  inline void Timer::Print(std::ostream& os) const
  {
    os 
      << elapsed << " secs "
      << count << " times";
    if (count > 1)
    {
      os
	<< " " 
	<< (elapsed/count)
	<< " secs each";
    }
  }

} // namespace

//--------------------------------------------------------------------------
#endif //  NISAC_COMMON_TIMER
//-------------------------------------------------------------------------
