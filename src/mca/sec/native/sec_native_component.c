/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2008 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2015      Los Alamos National Security, LLC. All rights
 *                         reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 * These symbols are in a file by themselves to provide nice linker
 * semantics.  Since linkers generally pull in symbols by object
 * files, keeping these symbols as the only symbols in this file
 * prevents utility programs such as "ompi_info" from having to import
 * entire components just to query their version and parameters.
 */

#include <src/include/ww_config.h>
#include "ww_types.h"


#include "src/mca/sec/sec.h"
#include "sec_native.h"

static ww_status_t component_open(void);
static ww_status_t component_close(void);
static ww_status_t component_query(mca_base_module_t **module, int *priority);

/*
 * Instantiate the public struct with all of our public information
 * and pointers to our public functions in it
 */
ww_sec_base_component_t mca_ww_sec_component = {
    .base = {
        WW_SEC_BASE_VERSION_1_0_0,

        /* Component name and version */
        .mca_component_name = "native",
        MCA_BASE_MAKE_VERSION(component, WW_MAJOR_VERSION, WW_MINOR_VERSION,
                              WW_RELEASE_VERSION),

        /* Component open and close functions */
        .mca_open_component = component_open,
        .mca_close_component = component_close,
        .mca_query_component = component_query,
    },
    .data = {
        /* The component is checkpoint ready */
        MCA_BASE_METADATA_PARAM_CHECKPOINT
    },
};


static int component_open(void)
{
    return WW_SUCCESS;
}


static int component_query(mca_base_module_t **module, int *priority)
{
    *priority = 10;
    *module = (mca_base_module_t *)&ww_native_module;
    return WW_SUCCESS;
}


static int component_close(void)
{
    return WW_SUCCESS;
}

