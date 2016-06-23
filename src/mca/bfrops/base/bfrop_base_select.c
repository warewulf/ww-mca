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
 * Copyright (c) 2016      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>
#include <ww_types.h>

#include <string.h>

#include "src/mca/mca.h"
#include "src/mca/base/base.h"

#include "src/mca/bfrops/base/base.h"

static bool selected = false;

/* Function for selecting a prioritized list of components
 * from all those that are available. */
int ww_bfrop_base_select(void)
{
    mca_base_component_list_item_t *cli = NULL;
    mca_base_component_t *component = NULL;
    mca_base_module_t *module = NULL;
    ww_bfrops_module_t *nmodule;
    ww_bfrops_base_active_module_t *newmodule, *mod;
    int rc, priority;
    bool inserted;

    if (selected) {
        /* ensure we don't do this twice */
        return WW_SUCCESS;
    }
    selected = true;

    /* Query all available components and ask if they have a module */
    WW_LIST_FOREACH(cli, &ww_bfrops_base_framework.framework_components, mca_base_component_list_item_t) {
        component = (mca_base_component_t *) cli->cli_component;

        ww_output_verbose(5, ww_bfrops_base_framework.framework_output,
                            "mca:bfrops:select: checking available component %s", component->mca_component_name);

        /* If there's no query function, skip it */
        if (NULL == component->mca_query_component) {
            ww_output_verbose(5, ww_bfrops_base_framework.framework_output,
                                "mca:bfrops:select: Skipping component [%s]. It does not implement a query function",
                                component->mca_component_name );
            continue;
        }

        /* Query the component */
        ww_output_verbose(5, ww_bfrops_base_framework.framework_output,
                            "mca:bfrops:select: Querying component [%s]",
                            component->mca_component_name);
        rc = component->mca_query_component(&module, &priority);

        /* If no module was returned, then skip component */
        if (WW_SUCCESS != rc || NULL == module) {
            ww_output_verbose(5, ww_bfrops_base_framework.framework_output,
                                "mca:bfrops:select: Skipping component [%s]. Query failed to return a module",
                                component->mca_component_name );
            continue;
        }

        /* If we got a module, keep it */
        nmodule = (ww_bfrops_module_t*) module;
        /* add to the list of selected modules */
        newmodule = WW_NEW(ww_bfrops_base_active_module_t);
        newmodule->pri = priority;
        newmodule->module = nmodule;
        newmodule->component = (ww_bfrops_base_component_t*)cli->cli_component;

        /* maintain priority order */
        inserted = false;
        WW_LIST_FOREACH(mod, &ww_bfrops_globals.actives, ww_bfrops_base_active_module_t) {
            if (priority > mod->pri) {
                ww_list_insert_pos(&ww_bfrops_globals.actives,
                                     (ww_list_item_t*)mod, &newmodule->super);
                inserted = true;
                break;
            }
        }
        if (!inserted) {
            /* must be lowest priority - add to end */
            ww_list_append(&ww_bfrops_globals.actives, &newmodule->super);
        }
    }

    if (4 < ww_output_get_verbosity(ww_bfrops_base_framework.framework_output)) {
        ww_output(0, "Final Bfrop priorities");
        /* show the prioritized list */
        WW_LIST_FOREACH(mod, &ww_bfrops_globals.actives, ww_bfrops_base_active_module_t) {
            ww_output(0, "\tBfrop: %s Priority: %d", mod->component->base.mca_component_name, mod->pri);
        }
    }

    return WW_SUCCESS;;
}
