#
# Copyright (c) 2004-2010 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2010-2015 Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

# main library setup
noinst_LTLIBRARIES = libmca_dl.la
libmca_dl_la_SOURCES =

# local files
headers = dl.h
libmca_dl_la_SOURCES += $(headers)

# Ensure that the man pages are rebuilt if the ww_config.h file
# changes; a "good enough" way to know if configure was run again (and
# therefore the release date or version may have changed)
$(nodist_man_MANS): $(top_builddir)/ww/include/ww_config.h

# Conditionally install the header files
if WANT_INSTALL_HEADERS
wwdir = $(wwincludedir)/$(subdir)
nobase_ww_HEADERS = $(headers)
endif

include base/Makefile.am

distclean-local:
	rm -f base/static-components.h
	rm -f $(nodist_man_MANS)
