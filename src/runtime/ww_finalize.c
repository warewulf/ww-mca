/*
 * Copyright (c) 2004-2010 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2008-2015 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2010-2015 Los Alamos National Security, LLC.
 *                         All rights reserved.
 * Copyright (c) 2013-2016 Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/** @file **/

#include <src/include/ww_config.h>

#include "src/class/ww_object.h"
#include "src/util/output.h"
#include "src/util/keyval_parse.h"
#include "src/util/show_help.h"
#include "src/mca/base/base.h"
#include "src/mca/installdirs/base/base.h"
#include "src/mca/bfrops/base/base.h"
#include "src/mca/sec/base/base.h"
#include WW_EVENT_HEADER

#include "src/runtime/ww_rte.h"
#include "src/runtime/ww_progress_threads.h"

extern int ww_initialized;
extern bool ww_init_called;

static void __ww_attribute_destructor__ ww_cleanup (void)
{
    if (!ww_initialized) {
        /* nothing to do */
        return;
    }

    /* finalize the class/object system */
    ww_class_finalize();
}

int ww_finalize(void)
{
    if( --ww_initialized != 0 ) {
        if( ww_initialized < 0 ) {
            return WW_ERROR;
        }
        return WW_SUCCESS;
    }

    /* close the security framework */
    (void) mca_base_framework_close(&ww_sec_base_framework);

    /* Clear out all the registered MCA params */
    ww_deregister_params();
    mca_base_var_finalize();

    /* keyval lex-based parser */
    ww_util_keyval_parse_finalize();

    (void) mca_base_framework_close(&ww_installdirs_base_framework);

    /* finalize the show_help system */
    ww_show_help_finalize();

    /* finalize the output system.  This has to come *after* the
       malloc code, as the malloc code needs to call into this, but
       the malloc code turning off doesn't affect ww_output that
       much */
    ww_output_finalize();

    /* close the bfrops */
    (void) mca_base_framework_close(&ww_bfrops_base_framework);

    /* stop the progress thread */
    (void) ww_progress_thread_finalize(NULL);

    #if WW_NO_LIB_DESTRUCTOR
        ww_cleanup();
    #endif

    return WW_SUCCESS;
}
