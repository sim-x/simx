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

//--------------------------------------------------------------------------
// File:    LoggerLayout.h
// Module:  Log
// Author:  Paula Stretz
// Created: July 11, 2002
// Description: 
//		Layout class specific to LANL for formatting log messages.
//		A subclass of log4cpp::Layout.
//
// @@
//
//--------------------------------------------------------------------------
#ifndef NISAC_LOG_LOGGER_LAYOUT_H
#define NISAC_LOG_LOGGER_LAYOUT_H
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>

namespace Log {

// Layout class specific to LANL for formatting
// log messages.  Prints the following format:
// Priority Month Day Time ThreadName - Message
class LoggerLayout : public log4cpp::Layout
{
	public:

	// Constructor
	LoggerLayout();
	~LoggerLayout();
	virtual std::string format(const log4cpp::LoggingEvent& event);
	void setSeparator(std::string sep) {fSeparator = sep;}
	private:
	std::string fSeparator;
	const int fCategoryNameWidth;
	const int fThreadNameWidth;
	const int fPriorityWidth;
};

inline
LoggerLayout::LoggerLayout() :
fSeparator(" "),
fCategoryNameWidth(12),
fThreadNameWidth(3),
fPriorityWidth(5)
{
}

inline
LoggerLayout::~LoggerLayout()
{
}

inline 
std::string 
LoggerLayout::format(const log4cpp::LoggingEvent& event)
{
	// get message level
	std::string pr = log4cpp::Priority::getPriorityName((int) event.priority);

	// Format date
	// # seconds since 1970
	time_t seconds = event.timeStamp.getSeconds();
  int microsec = event.timeStamp.getMicroSeconds();
	std::string timestr(ctime(&seconds));
	std::string day, month, tod, dow;
	std::stringstream ist(timestr.c_str());
	ist >> dow >> month >> day >> tod;

	// Construct prefix string
  std::stringstream ost;
	ost.setf(std::ios::left); 
	ost << month << " " << day << fSeparator << tod << "."
      << std::setw(6) << std::setfill('0') << std::right << microsec << " "
      << std::setfill(' ')
      << std::setw(fThreadNameWidth) << event.ndc << " "
      << std::setw(fCategoryNameWidth) << event.categoryName << " "
      << std::setw(fPriorityWidth) << pr << ": ";

  ost << event.message;
	return ost.str();
}
} // namespace
#endif
