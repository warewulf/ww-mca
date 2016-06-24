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

SUBDIRS = config src lib etc bin

headers =
sources =
nodist_headers =

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure compile config.* install-sh missing
DISTCLEANFILES =
CLEANFILES =
EXTRA_DIST = warewulf.spec LICENSE
dist_wwdata_DATA = contrib/ww-valgrind.supp
wwdir = $(wwincludedir)/$(subdir)

nobase_ww_HEADERS = $(headers)

# Fix for make distcheck
DISTCHECK_CONFIGURE_FLAGS = --with-perllibdir=$$dc_install_base/perllibs

TESTVERBOSE=0

test: all
	perl -I$(srcdir)/lib -MTest::Harness -e '$$Test::Harness::verbose='$(TESTVERBOSE)'; runtests(@ARGV);' t/*.t

test-verbose:
	$(MAKE) test TESTVERBOSE=1

nroff:
	(cd man; $(MAKE) nroff)

dist-hook:
	env LS_COLORS= sh "$(top_srcdir)/config/distscript.sh" "$(top_srcdir)" "$(distdir)" "$(WW_VERSION)" "$(WW_REPO_REV)"

