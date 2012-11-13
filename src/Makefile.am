AM_CXXFLAGS = @MPI_CXXFLAGS@ @BOOST_CXXFLAGS@ -I$(top_builddir) -I$(top_srcdir)
AM_CFLAGS = @MPI_CFLAGS@ @BOOST_CFLAGS@ -I$(top_builddir) -I$(top_srcdir)
AM_LDFLAGS = @MPI_LDFLAGS@ @BOOST_LDFLAGS@
PYTHON_CXXFLAGS= -I/usr/include/boost -I/usr/include/python2.6

SUBDIRS = \
	Common \
	Config \
	File \
	Global \
	Log \
	Random \
	Python 


#################################################################
# the simx library
lib_LTLIBRARIES = libsimx.la 

libsimx_la_SOURCES = \
	DassfEvent.C \
	DassfLP.C \
	LP.C \
	readers.C \
	register.C \
	logger.C \
	PackedData.C \
	PackedData_PRIME.C \
	EventInfo.C \
	EventInfoManager.C \
	Service.C \
	ServiceData.C \
	ServiceManager.C \
	Input.C \
	Info.C \
	InfoData.C \
	InfoManager.C \
	InputHandler.C \
	Entity.C \
	EntityData.C \
	EntityManager.C \
	EntityFactory.C \
	output.C \
	control.C \
	ControlInfoWrapper.C \
	Messenger.C \
	Controller.C \
	simEngine.C

#################################################################
# the UIS libs parts:

uis_libs = \
	Common/libCommon.la \
	Config/libConfig.la \
	File/libFile.la \
	Global/libGlobal.la \
	Log/libLog.la \
	Random/libRandom.la \
	Python/libPython.la

.PHONY: libs
libs:     
	$(foreach f,$(uis_libs),make -C $(dir $f) $(notdir $f);)


#################################################################

libsimx_la_CXXFLAGS =  $(MPI_CXXFLAGS) $(PRIME_INCLUDES) $(PRIME_CFLAGS) $(AM_CXXFLAGS) $(PYTHON_CXXFLAGS)
libsimx_la_LIBADD = $(uis_libs)
