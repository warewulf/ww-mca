/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2015-2016 Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>


#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "src/class/ww_pointer_array.h"
#include "src/util/argv.h"
#include "src/util/error.h"

#include "src/mca/dstore/base/base.h"

ww_configuration_t* ww_dstore_base_load(char *name, ww_list_t *directives)
{
    ww_dstore_base_active_module_t *active;
    ww_configuration_t *config;

    if (!ww_dstore_globals.initialized) {
        return NULL;
    }

    /* check the directives */
    WW_LIST_FOREACH(active, &ww_dstore_globals.actives, ww_dstore_base_active_module_t) {
        if (NULL != (config = active->module->load(name, directives))) {
            return config;
        }
    }
    return NULL;
}

ww_status_t ww_dstore_base_commit(ww_configuration_t *config,
                                  ww_list_t *directives)
{
    ww_dstore_base_active_module_t *active;
    ww_status_t rc;

    if (!ww_dstore_globals.initialized) {
        return WW_ERR_INIT;
    }

    WW_LIST_FOREACH(active, &ww_dstore_globals.actives, ww_dstore_base_active_module_t) {
    }
    return WW_SUCCESS;
}

ww_status_t ww_dstore_base_find(ww_configuration_t *config,
                                char *key, char *type,
                                ww_list_t *directives,
                                ww_list_t *results)
{
    ww_dstore_base_active_module_t *active;

    if (!ww_dstore_globals.initialized) {
        return WW_ERR_INIT;
    }

    WW_LIST_FOREACH(active, &ww_dstore_globals.actives, ww_dstore_base_active_module_t) {
    }
    return WW_SUCCESS;
}

ww_status_t ww_dstore_base_set(ww_building_block_t *block,
                               ww_kval_t *kv, ww_list_t *directives)
{
    ww_dstore_base_active_module_t *active;

    if (!ww_dstore_globals.initialized) {
        return WW_ERR_INIT;
    }

    WW_LIST_FOREACH(active, &ww_dstore_globals.actives, ww_dstore_base_active_module_t) {
    }
    return WW_SUCCESS;
}

ww_value_cmp_t ww_dstore_base_compare(ww_configuration_t *config1,
                                      ww_configuration_t *config2,
                                      ww_list_t *directives)
{
    ww_dstore_base_active_module_t *active;

    if (!ww_dstore_globals.initialized) {
        return WW_VALUE1_GREATER;
    }

    WW_LIST_FOREACH(active, &ww_dstore_globals.actives, ww_dstore_base_active_module_t) {
    }
    return WW_EQUAL;
}
