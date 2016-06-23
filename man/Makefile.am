#
# Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2009 High Performance Computing Center Stuttgart,
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2006-2016 Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2012-2013 Los Alamos National Security, Inc.  All rights reserved.
# Copyright (c) 2013-2016 Intel, Inc. All rights reserved
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

if !WW_EMBEDDED_MODE

man_MANS = \
        man/man3/ww_init.3 \
        man/man3/ww_finalize.3 \
        man/man3/ww_initialized.3 \
        man/man3/ww_abort.3 \
        man/man3/ww_put.3 \
        man/man3/ww_commit.3 \
        man/man7/ww.7 \
        man/man7/ww_constants.7

EXTRA_DIST = $(man_MANS)

man/man3/ww_init.3:
    $(top_srcdir)contrib/md2nroff.pl --source=man/ww_init.3.md;

man/man3/ww_finalize.3:
    $(top_srcdir)contrib/md2nroff.pl --source=man/ww_finalize.3.md;

man/man3/ww_initialized.3:
    $(top_srcdir)contrib/md2nroff.pl --source=man/ww_initialized.3.md;

man/man3/ww_abort.3:
    $(top_srcdir)contrib/md2nroff.pl --source=man/ww_abort.3.md;

man/man3/ww_put.3:
    $(top_srcdir)contrib/md2nroff.pl --source=man/ww_put.3.md;

man/man3/ww_commit.3:
    $(top_srcdir)contrib/md2nroff.pl --source=man/ww_commit.3.md;

man/man7/ww.7:
    $(top_srcdir)contrib/md2nroff.pl --source=man/ww.7.md;

man/man7/ww_constants.7:
    $(top_srcdir)contrib/md2nroff.pl --source=man/ww_constants.7.md;

endif # !WW_EMBEDDED_MODE
