/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2007 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2008      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2010-2012 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2014-2016 Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/** @file **/

#ifndef WW_H
#define WW_H

#include "ww_config.h"
#include "ww_types.h"
#include "src/class/ww_object.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include WW_EVENT_HEADER

#include "src/util/printf.h"
#include "src/mca/bfrops/bfrops.h"
BEGIN_C_DECLS

/** version string of ww */
WW_DECLSPEC extern const char ww_version_string[];

/* object for tracking the communication-related characteristics
 * of a given peer */
typedef struct ww_peer_comm_t {
    ww_bfrop_buffer_type_t type;
    ww_bfrops_module_t *bfrops;
//    ww_sec_base_module_t *sec;
} ww_peer_comm_t;

/* define a peer object */
typedef struct ww_peer_t {
    ww_object_t super;
    uid_t uid;
    gid_t gid;
    ww_peer_comm_t comm;
} ww_peer_t;
WW_CLASS_DECLARATION(ww_peer_t);

typedef struct ww_globals_t {
    ww_event_base_t *evbase;
    ww_peer_t *mypeer;
    int debug_output;
    uid_t uid;
    gid_t gid;
} ww_globals_t;

extern ww_globals_t ww_globals;

/**
 * Initialize the WW layer, including the MCA system.
 *
 * @retval WW_SUCCESS Upon success.
 * @retval WW_ERROR Upon failure.
 *
 * \note If this function is called, ww_init_util() should *not* be
 * called.
 */
WW_DECLSPEC int ww_init(int* pargc, char*** pargv);

/**
 * Finalize the WW layer, including the MCA system.
 *
 * @retval WW_SUCCESS Upon success.
 * @retval WW_ERROR Upon failure.
 *
 * \note If this function is called, ww_finalize_util() should *not*
 * be called.
 */
WW_DECLSPEC int ww_finalize(void);

/**
 * Internal function.  Do not call.
 */
WW_DECLSPEC int ww_register_params(void);
WW_DECLSPEC int ww_deregister_params(void);

END_C_DECLS

#endif
