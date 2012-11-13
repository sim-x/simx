######################################################################
## $Id$
######################################################################

######################################################################
PYTHON_CXXFLAGS= -I/usr/include/boost -I/usr/include/python2.6
#noinst_HEADERS = \
	main.h

noinst_LTLIBRARIES = libPython.la 
libPython_la_SOURCES = \
	PyConfig.C \
	PyControl.C \
	PyEntityData.C \
	PyCreateEntity.C \
	PyInfoData.C \
	PyCreateInputInfo.C \
	PyRegister.C \
	PyEntity.C \
	PyService.C \
	PyInfo.C \
	PyEntityInput.C \
	PyServiceData.C \
	PyServiceInput.C \
	PyUtility.C \
	PyMain.C \
	PyManager.C \
	PyLogger.C \
	PyOutput.C \
	init.C \
	export.C


libPython_la_CXXFLAGS = $(PYTHON_CXXFLAGS)

######################################################################
######################################################################
