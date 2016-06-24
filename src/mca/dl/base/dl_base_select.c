/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2010 The Trustees of Indiana University.
 *                         All rights reserved.
 * Copyright (c) 2015 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2015      Los Alamos National Security, LLC. All rights
 *                         reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>

#ifdef HAVE_UNISTD_H
#include "unistd.h"
#endif

#include "ww_types.h"
#include "src/util/output.h"
#include "src/mca/mca.h"
#include "src/mca/base/base.h"
#include "src/mca/dl/dl.h"
#include "src/mca/dl/base/base.h"


int ww_dl_base_select(void)
{
    int exit_status = WW_SUCCESS;
    ww_dl_base_component_t *best_component = NULL;
    ww_dl_base_module_t *best_module = NULL;

    /*
     * Select the best component
     */
    if (WW_SUCCESS != mca_base_select("dl",
                                        ww_dl_base_framework.framework_output,
                                        &ww_dl_base_framework.framework_components,
                                        (mca_base_module_t **) &best_module,
                                        (mca_base_component_t **) &best_component, NULL) ) {
        /* This will only happen if no component was selected */
        exit_status = WW_ERROR;
        goto cleanup;
    }

    /* Save the winner */
    ww_dl_base_selected_component = best_component;
    ww_dl = best_module;

 cleanup:
    return exit_status;
}
