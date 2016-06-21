/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2015-2016 Intel, Inc.  All rights reserved.
 * Copyright (c) 2016      Mellanox Technologies, Inc.
 *                         All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>

#include <ww_types.h>
#include "src/runtime/ww_rte.h"

#include "src/class/ww_list.h"
#include "src/util/argv.h"
#include "src/util/output.h"


#include "src/mca/sec/base/base.h"


char* ww_sec_base_get_available_modules(void)
{
    ww_sec_base_active_module_t *active;
    char **tmp=NULL, *reply=NULL;

    if (!ww_sec_globals.initialized) {
        return NULL;
    }

    WW_LIST_FOREACH(active, &ww_sec_globals.actives, ww_sec_base_active_module_t) {
        ww_argv_append_nosize(&tmp, active->component->base.mca_component_name);
    }
    if (NULL != tmp) {
        reply = ww_argv_join(tmp, ',');
        ww_argv_free(tmp);
    }
    return reply;
}

ww_sec_module_t* ww_sec_base_assign_module(const char *options)
{
    ww_sec_base_active_module_t *active;
    char **tmp=NULL;
    int i;
    ww_sec_module_t *mod;

    if (!ww_sec_globals.initialized) {
        return NULL;
    }
    if (NULL != options) {
        tmp = ww_argv_split(options, ',');
    }

    WW_LIST_FOREACH(active, &ww_sec_globals.actives, ww_sec_base_active_module_t) {
        if (NULL == tmp) {
            if (NULL != (mod = active->component->assign_module())) {
                return mod;
            }
        } else {
            for (i=0; NULL != tmp[i]; i++) {
                if (0 == strcmp(tmp[i], active->component->base.mca_component_name)) {
                    if (NULL != (mod = active->component->assign_module())) {
                        ww_argv_free(tmp);
                        return mod;
                    }
                }
            }
        }
    }

    /* we only get here if nothing was found */
    if (NULL != tmp) {
        ww_argv_free(tmp);
    }
    return NULL;
}

