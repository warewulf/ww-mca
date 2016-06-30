/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2016      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
/** @file:
 *
 */
#include <src/include/ww_config.h>

#include <ww_types.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "src/mca/base/mca_base_var.h"
#include "src/class/ww_list.h"
#include "src/util/argv.h"

#include "src/mca/dstore/base/base.h"

/*
 * The following file was created by configure.  It contains extern
 * statements and the definition of an array of pointers to each
 * component's public mca_base_component_t struct.
 */

#include "src/mca/dstore/base/static-components.h"

/* Instantiate the global vars */
ww_dstore_globals_t ww_dstore_globals;
ww_dstore_API_t ww_dstore = {
    .load = ww_dstore_base_load,
    .commit = ww_dstore_base_commit,
    .find = ww_dstore_base_find,
    .set = ww_dstore_base_set,
    .compare = ww_dstore_base_compare
};

static ww_status_t ww_dstore_close(void)
{
    ww_dstore_base_active_module_t *active;

    if (!ww_dstore_globals.initialized) {
        return WW_SUCCESS;
    }
    ww_dstore_globals.initialized = false;

    WW_LIST_FOREACH(active, &ww_dstore_globals.actives, ww_dstore_base_active_module_t) {
        if (NULL != active->component->base.mca_close_component) {
            active->component->base.mca_close_component();
        }
    }
    WW_LIST_DESTRUCT(&ww_dstore_globals.actives);

    return WW_SUCCESS;
}

static ww_status_t ww_dstore_open(mca_base_open_flag_t flags)
{
  /* initialize globals */
  ww_dstore_globals.initialized = true;
  WW_CONSTRUCT(&ww_dstore_globals.actives, ww_list_t);
  return WW_SUCCESS;
}

MCA_BASE_FRAMEWORK_DECLARE(ww, dstore, "Warewulf Datastore Operations",
                           NULL, ww_dstore_open, ww_dstore_close,
                           mca_dstore_base_static_components, 0);

/**
 * Object constructors, destructors, and instantiations
 */
static void kvcon(ww_kval_t *k)
{
    k->key = NULL;
    k->values = NULL;
}
static void kvdes(ww_kval_t *k)
{
    if (NULL != k->key) {
        free(k->key);
    }
    if (NULL != k->values) {
        ww_argv_free(k->values);
    }
}
WW_CLASS_INSTANCE(ww_kval_t,
                  ww_list_item_t,
                  kvcon, kvdes);

WW_CLASS_INSTANCE(ww_dstore_base_active_module_t,
                  ww_list_item_t,
                  NULL, NULL);
