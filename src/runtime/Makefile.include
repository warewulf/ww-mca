# -*- makefile -*-
#
# Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2012      Los Alamos National Security, LLC.
#                         All rights reserved.
# Copyright (c) 2014-2016 Intel, Inc. All rights reserved
# Copyright (c) 2014      Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

# This makefile.am does not stand on its own - it is included from src/Makefile.am

dist_wwdata_DATA += runtime/help-ww-runtime.txt

headers += \
        runtime/ww_rte.h \
        runtime/ww_params.h \
        runtime/ww_progress_threads.h

libww_la_SOURCES += \
        runtime/ww_finalize.c \
        runtime/ww_init.c \
        runtime/ww_params.c \
        runtime/ww_progress_threads.c
