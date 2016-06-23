/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
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
 * Copyright (c) 2007-2012 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2007      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
 * Copyright (c) 2010-2015 Los Alamos National Security, LLC.
 *                         All rights reserved.
 * Copyright (c) 2013-2016 Intel, Inc. All rights reserved
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/** @file **/

#include <src/include/ww_config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "src/util/output.h"
#include "src/util/show_help.h"
#include "src/mca/base/base.h"
#include "src/mca/base/mca_base_var.h"
#include "src/mca/installdirs/base/base.h"
#include "src/mca/bfrops/base/base.h"
#include "src/mca/sec/base/base.h"

#include "ww_types.h"
#include "src/util/error.h"
#include "src/util/keyval_parse.h"

#include "src/runtime/ww_rte.h"
#include "src/runtime/ww_progress_threads.h"

#if WW_CC_USE_PRAGMA_IDENT
#pragma ident WW_IDENT_STRING
#elif WW_CC_USE_IDENT
#ident WW_IDENT_STRING
#endif
const char ww_version_string[] = WW_IDENT_STRING;

int ww_initialized = 0;
bool ww_init_called = false;
ww_globals_t ww_globals;

int ww_init(int* pargc, char*** pargv)
{
    int ret;
    char *error = NULL;

    if( ++ww_initialized != 1 ) {
        if( ww_initialized < 1 ) {
            return WW_ERROR;
        }
        return WW_SUCCESS;
    }

    #if WW_NO_LIB_DESTRUCTOR
        if (ww_init_called) {
            /* can't use show_help here */
            fprintf (stderr, "ww_init_util: attempted to initialize after finalize without compiler "
                     "support for either __attribute__(destructor) or linker support for -fini -- process "
                     "will likely abort\n");
            return WW_ERR_NOT_SUPPORTED;
        }
    #endif

        ww_init_called = true;

    /* initialize the output system */
    ww_output_init();

    /* initialize install dirs code */
    if (WW_SUCCESS != (ret = mca_base_framework_open(&ww_installdirs_base_framework, 0))) {
        fprintf(stderr, "ww_installdirs_base_open() failed -- process will likely abort (%s:%d, returned %d instead of WW_SUCCESS)\n",
                __FILE__, __LINE__, ret);
        return ret;
    }

    /* initialize the help system */
    ww_show_help_init();

    /* keyval lex-based parser */
    if (WW_SUCCESS != (ret = ww_util_keyval_parse_init())) {
        error = "ww_util_keyval_parse_init";
        goto return_error;
    }

    /* Setup the parameter system */
    if (WW_SUCCESS != (ret = mca_base_var_init())) {
        error = "mca_base_var_init";
        goto return_error;
    }

    /* read any param files that were provided */
    if (WW_SUCCESS != (ret = mca_base_var_cache_files(false))) {
        error = "failed to cache files";
        goto return_error;
    }


    /* register params for ww */
    if (WW_SUCCESS != (ret = ww_register_params())) {
        error = "ww_register_params";
        goto return_error;
    }

    /* initialize the mca */
    if (WW_SUCCESS != (ret = mca_base_open())) {
        error = "mca_base_open";
        goto return_error;
    }

    /* initialize the security framework */
    if( WW_SUCCESS != (ret = mca_base_framework_open(&ww_sec_base_framework, 0)) ) {
        error = "ww_sec_base_open";
        goto return_error;
    }
    if( WW_SUCCESS != (ret = ww_sec_base_select()) ) {
        error = "ww_sec_base_select";
        goto return_error;
    }

    /* initialize the bfrops framework */
    if( WW_SUCCESS != (ret = mca_base_framework_open(&ww_bfrops_base_framework, 0)) ) {
        error = "ww_bfrops_base_open";
        goto return_error;
    }
    if( WW_SUCCESS != (ret = ww_bfrop_base_select()) ) {
        error = "ww_bfrops_base_select";
        goto return_error;
    }

    /* create an event base and progress thread for us */
    if (NULL == (ww_globals.evbase = ww_progress_thread_init(NULL))) {
        error = "progress thread";
        ret = WW_ERROR;
        goto return_error;
    }

    return WW_SUCCESS;

 return_error:
    ww_show_help( "help-ww-runtime.txt",
                    "ww_init:startup:internal-failure", true,
                    error, ret );
    return ret;
}
