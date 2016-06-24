# -*- makefile -*-
#
# Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2012      Los Alamos National Security, Inc.  All rights reserved.
# Copyright (c) 2013-2016 Intel, Inc. All rights reserved
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

headers = sec_munge.h
sources = \
        sec_munge_component.c \
        sec_munge.c

# Make the output library in this directory, and name it either
# mca_<type>_<name>.la (for DSO builds) or libmca_<type>_<name>.la
# (for static builds).

if MCA_BUILD_ww_sec_munge_DSO
lib =
lib_sources =
component = mca_ww_sec_munge.la
component_sources = $(headers) $(sources)
else
lib = libmca_ww_sec_munge.la
lib_sources = $(headers) $(sources)
component =
component_sources =
endif

mcacomponentdir = $(wwlibdir)
mcacomponent_LTLIBRARIES = $(component)
mca_ww_sec_munge_la_SOURCES = $(component_sources)
mca_ww_sec_munge_la_LDFLAGS = -module -avoid-version

noinst_LTLIBRARIES = $(lib)
libmca_ww_sec_munge_la_SOURCES = $(lib_sources)
libmca_ww_sec_munge_la_LDFLAGS = -module -avoid-version
