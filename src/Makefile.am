#
# Copyright (c) 2016      Intel, Inc. All rights reserved
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

# Note that the -I directory must *exactly* match what was specified
# via AC_CONFIG_MACRO_DIR in configure.ac.
ACLOCAL_AMFLAGS = -I ./config

SUBDIRS = \
	util/keyval \
	mca/base \
	$(MCA_ww_FRAMEWORKS_SUBDIRS) \
	$(MCA_ww_FRAMEWORK_COMPONENT_STATIC_SUBDIRS) \
	. \
	$(MCA_ww_FRAMEWORK_COMPONENT_DSO_SUBDIRS)

DIST_SUBDIRS = \
	mca/base \
	$(MCA_ww_FRAMEWORKS_SUBDIRS) \
	$(MCA_ww_FRAMEWORK_COMPONENT_ALL_SUBDIRS)

headers =
sources =
nodist_headers =

dist_wwdata_DATA =
lib_LTLIBRARIES = libww.la
libww_la_SOURCES = $(headers) $(sources)
libww_la_LDFLAGS = -version-info $(libww_so_version)
libww_la_LIBADD = \
	mca/base/libmca_base.la \
	$(MCA_ww_FRAMEWORK_LIBS)
libww_la_DEPENDENCIES = $(libww_la_LIBADD)

if WW_DESTRUCTOR_USE_FINI

libww_la_LDFLAGS += -Wl,-fini -Wl,ww_cleanup

endif

include class/Makefile.include
include include/Makefile.include
include mca/Makefile.include
include util/Makefile.include
include tools/Makefile.include
include runtime/Makefile.include
include sys/Makefile.include
include threads/Makefile.include

MAINTAINERCLEANFILES = Makefile.in config.h config.h.in
DISTCLEANFILES = Makefile
CLEANFILES = core.* *~
AM_CFLAGS = -Wall


EXTRA_DIST = config.h
