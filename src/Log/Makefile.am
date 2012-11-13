######################################################################
## $Id: Makefile.am,v 1.1.1.1 2011/08/18 22:19:45 nsanthi Exp $
######################################################################

######################################################################

File_common = \
	Config.h \
	Logger.C \
	Logger.h \
	LoggerLayout.h \
	nullstream.h \
	LogStreambuf.C \
	LogStreambuf.h

noinst_LTLIBRARIES = libLog.la

libLog_la_CXXFLAGS = \
	$(AM_CXXFLAGS) \
	$(BOOST_CXXFLAGS)

libLog_la_SOURCES = \
	$(File_common)

######################################################################
######################################################################
