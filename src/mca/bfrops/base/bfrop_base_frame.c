/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2009 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2012-2013 Los Alamos National Security, Inc.  All rights reserved.
 * Copyright (c) 2014-2016 Intel, Inc. All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
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
#include "src/mca/bfrops/base/base.h"

/*
 * The following file was created by configure.  It contains extern
 * statements and the definition of an array of pointers to each
 * component's public mca_base_component_t struct.
 */

#include "src/mca/bfrops/base/static-components.h"

/* Instantiate the global vars */
ww_bfrops_globals_t ww_bfrops_globals;
ww_bfrops_module_t ww_bfrops = {
    .pack = ww_bfrop_base_pack,
    .unpack = ww_bfrop_base_unpack,
    .copy = ww_bfrop_base_copy,
    .print = ww_bfrop_base_print,
    .value_xfer = ww_base_value_xfer,
    .value_load = ww_base_value_load,
    .value_unload = ww_base_value_unload,
    .value_cmp = ww_base_value_cmp
};

static int ww_bfrop_register(mca_base_register_flag_t flags)
{
    ww_bfrops_globals.initial_size = 0;
    mca_base_var_register("ww", "bfrops", "base", "initial_size",
                          "Initial size of a buffer",
                          MCA_BASE_VAR_TYPE_SIZE_T, NULL, 0, 0,
                          WW_INFO_LVL_2,
                          MCA_BASE_VAR_SCOPE_READONLY,
                          &ww_bfrops_globals.initial_size);

    ww_bfrops_globals.threshold_size = 0;
    mca_base_var_register("ww", "bfrops", "base", "threshold_size",
                          "Size at which we switch from extending a buffer by doubling to extending by a smaller value",
                          MCA_BASE_VAR_TYPE_SIZE_T, NULL, 0, 0,
                          WW_INFO_LVL_2,
                          MCA_BASE_VAR_SCOPE_READONLY,
                          &ww_bfrops_globals.threshold_size);

    ww_bfrops_globals.default_type = WW_BFROP_BUFFER_NON_DESC;
    mca_base_var_register("ww", "bfrops", "base", "default_type",
                          "Default type for buffers",
                          MCA_BASE_VAR_TYPE_INT, NULL, 0, 0,
                          WW_INFO_LVL_2,
                          MCA_BASE_VAR_SCOPE_READONLY,
                          &ww_bfrops_globals.default_type);
    return WW_SUCCESS;
}

static ww_status_t ww_bfrop_close(void)
{
    if (!ww_bfrops_globals.initialized) {
        return WW_SUCCESS;
    }
    ww_bfrops_globals.initialized = false;

    /* the components will cleanup when closed */
    WW_DESTRUCT(&ww_bfrops_globals.actives);

    return WW_SUCCESS;
}

static ww_status_t ww_bfrop_open(mca_base_open_flag_t flags)
{
  /* initialize globals */
  ww_bfrops_globals.initialized = true;
  WW_CONSTRUCT(&ww_bfrops_globals.actives, ww_list_t);
  return WW_SUCCESS;
}

MCA_BASE_FRAMEWORK_DECLARE(ww, bfrops, "Warewulf Buffer Operations",
                           ww_bfrop_register, ww_bfrop_open, ww_bfrop_close,
                           mca_bfrops_base_static_components, 0);

WW_CLASS_INSTANCE(ww_bfrops_base_active_module_t,
                    ww_list_item_t,
                    NULL, NULL);

/**
 * Object constructors, destructors, and instantiations
 */
/** Value **/
static void ww_buffer_construct (ww_buffer_t* buffer)
{
    /** set the default buffer type */
    buffer->type = ww_bfrops_globals.default_type;

    /* Make everything NULL to begin with */
    buffer->base_ptr = buffer->pack_ptr = buffer->unpack_ptr = NULL;
    buffer->bytes_allocated = buffer->bytes_used = 0;
}

static void ww_buffer_destruct (ww_buffer_t* buffer)
{
    if (NULL != buffer->base_ptr) {
        free (buffer->base_ptr);
    }
}

WW_CLASS_INSTANCE(ww_buffer_t,
                   ww_object_t,
                   ww_buffer_construct,
                   ww_buffer_destruct);


static void ww_bfrop_type_info_construct(ww_bfrop_type_info_t *obj)
{
    obj->odti_name = NULL;
    obj->odti_pack_fn = NULL;
    obj->odti_unpack_fn = NULL;
    obj->odti_copy_fn = NULL;
    obj->odti_print_fn = NULL;
}

static void ww_bfrop_type_info_destruct(ww_bfrop_type_info_t *obj)
{
    if (NULL != obj->odti_name) {
        free(obj->odti_name);
    }
}

WW_CLASS_INSTANCE(ww_bfrop_type_info_t, ww_object_t,
                   ww_bfrop_type_info_construct,
                   ww_bfrop_type_info_destruct);

static void kvcon(ww_kval_t *k)
{
    k->key = NULL;
    k->value = NULL;
}
static void kvdes(ww_kval_t *k)
{
    if (NULL != k->key) {
        free(k->key);
    }
    if (NULL != k->value) {
        WW_VALUE_RELEASE(k->value);
    }
}
WW_CLASS_INSTANCE(ww_kval_t,
                   ww_list_item_t,
                   kvcon, kvdes);

static void rcon(ww_regex_range_t *p)
{
    p->start = 0;
    p->cnt = 0;
}
WW_CLASS_INSTANCE(ww_regex_range_t,
                    ww_list_item_t,
                    rcon, NULL);

static void rvcon(ww_regex_value_t *p)
{
    p->prefix = NULL;
    p->suffix = NULL;
    p->num_digits = 0;
    WW_CONSTRUCT(&p->ranges, ww_list_t);
}
static void rvdes(ww_regex_value_t *p)
{
    if (NULL != p->prefix) {
        free(p->prefix);
    }
    if (NULL != p->suffix) {
        free(p->suffix);
    }
    WW_LIST_DESTRUCT(&p->ranges);
}
WW_CLASS_INSTANCE(ww_regex_value_t,
                    ww_list_item_t,
                    rvcon, rvdes);

