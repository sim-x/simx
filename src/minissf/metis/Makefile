SSFDIR = ../..
include $(SSFDIR)/Makefile.include
INCLUDES = -I.

METIS_SRCFILES = \
	balance.c \
	bucketsort.c \
	ccgraph.c \
	coarsen.c \
	compress.c \
	debug.c \
	defs.h \
	estmem.c \
	fm.c \
	fortran.c \
	frename.c \
	graph.c \
	initpart.c \
	kmetis.c \
	kvmetis.c \
	kwayfm.c \
	kwayrefine.c \
	kwayvolfm.c \
	kwayvolrefine.c \
	macros.h \
	match.c \
	mbalance2.c \
	mbalance.c \
	mcoarsen.c \
	memory.c \
	mesh.c \
	meshpart.c \
	metis.h \
	mfm2.c \
	mfm.c \
	mincover.c \
	minitpart2.c \
	minitpart.c \
	mkmetis.c \
	mkwayfmh.c \
	mkwayrefine.c \
	mmatch.c \
	mmd.c \
	mpmetis.c \
	mrefine2.c \
	mrefine.c \
	mutil.c \
	myqsort.c \
	ometis.c \
	parmetis.c \
	pmetis.c \
	pqueue.c \
	proto.h \
	refine.c \
	rename.h \
	separator.c \
	sfm.c \
	srefine.c \
	stat.c \
	struct.h \
	subdomains.c \
	timing.c \
	util.c \
	myrng.c

METIS_CFILES = $(filter %.c,$(METIS_SRCFILES))
METIS_HFILES = $(filter %.h,$(METIS_SRCFILES))
METIS_OBJFILES = $(METIS_CFILES:.c=.o)
METIS_BUILTLIB = libmetis.a
METIS_CLEANUP = $(METIS_OBJFILES) $(METIS_BUILTLIB)

metis:	$(METIS_BUILTLIB)

clean:
	$(RM) $(METIS_CLEANUP)

%.o:	%.c $(METIS_HFILES)
	$(CC) -c $(INCLUDES) $(CFLAGS) -o $@ $<

$(METIS_BUILTLIB):	$(METIS_OBJFILES)
	$(RM) $(METIS_BUILTLIB)
	$(AR) $(ARFLAGS) $(METIS_BUILTLIB) $(METIS_OBJFILES)
	$(RANLIB) $(METIS_BUILTLIB)
