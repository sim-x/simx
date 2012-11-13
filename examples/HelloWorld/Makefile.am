
LOCAL_CXXFLAGS = 
PYTHON_CXXFLAGS= -I/usr/include/boost -I/usr/include/python2.6
PYTHON_LDFLAGS = -lboost_python -lpython2.6 -lmpich -lmpl -lboost_serialization -lboost_iostreams

hellosim_files = \
	HelloWorld.C \
	Person.C \
	HelloHandlerPerson.C

##########################################################################
### HelloWorld
##########################################################################

bin_PROGRAMS = HelloWorld

noinst_LTLIBRARIES = libHelloWorld.la

libHelloWorld_la_SOURCES = register.C \
                           HelloWorld.C \
                           Person.C \
                           HelloHandlerPerson.C \
	                   $(top_builddir)/SimCore/Python/init_MPI.C


libHelloWorld_la_CXXFLAGS = $(PRIME_INCLUDES) $(PRIME_CFLAGS) $(AM_CXXFLAGS) $(PYTHON_CXXFLAGS)

HelloWorld_LDFLAGS = $(PRIME_LDFLAGS) \
	$(PYTHON_LDFLAGS) \
                     $(AM_LDFLAGS) \
		     $(MPI_LDFLAGS)

HelloWorld_LDADD = $(top_builddir)/HelloWorld/libHelloWorld.la \
                   $(top_builddir)/SimCore/libSimCore.la \
                   $(PRIME_LDADD) \
		   $(MPI_LIBS)

HelloWorld_SOURCES = HelloWorld.C \
			$(top_builddir)/SimCore/Global/main_MPI.C

HelloWorld_CXXFLAGS = $(PRIME_INCLUDES) $(PRIME_CFLAGS) $(PYTHON_CXXFLAGS) \
		      $(MPI_CXXFLAGS) \
                      $(AM_CXXFLAGS)
