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
#include <src/include/pmix_config.h>

#include <pmix/pmix_common.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "src/mca/base/mca_base_var.h"
#include "src/class/pmix_list.h"
#include "src/mca/states/base/base.h"

/*
 * The following file was created by configure.  It contains extern
 * statements and the definition of an array of pointers to each
 * component's public mca_base_component_t struct.
 */

#include "src/mca/states/base/static-components.h"

/* Instantiate the global vars */
pmix_states_globals_t pmix_states_globals;
pmix_states_module_t pmix_states = {
    .pack = pmix_state_base_pack,
    .unpack = pmix_state_base_unpack,
    .copy = pmix_state_base_copy,
    .print = pmix_state_base_print,
    .value_xfer = pmix_base_value_xfer,
    .value_load = pmix_base_value_load,
    .value_unload = pmix_base_value_unload,
    .value_cmp = pmix_base_value_cmp
};

static int pmix_state_register(mca_base_register_flag_t flags)
{
    pmix_states_globals.initial_size = 0;
    mca_base_var_register("pmix", "states", "base", "initial_size",
                          "Initial size of a buffer",
                          MCA_BASE_VAR_TYPE_SIZE_T, NULL, 0, 0,
                          PMIX_INFO_LVL_2,
                          MCA_BASE_VAR_SCOPE_READONLY,
                          &pmix_states_globals.initial_size);

    pmix_states_globals.threshold_size = 0;
    mca_base_var_register("pmix", "states", "base", "threshold_size",
                          "Size at which we switch from extending a buffer by doubling to extending by a smaller value",
                          MCA_BASE_VAR_TYPE_SIZE_T, NULL, 0, 0,
                          PMIX_INFO_LVL_2,
                          MCA_BASE_VAR_SCOPE_READONLY,
                          &pmix_states_globals.threshold_size);

    pmix_states_globals.default_type = PMIX_STATE_BUFFER_NON_DESC;
    mca_base_var_register("pmix", "states", "base", "default_type",
                          "Default type for buffers",
                          MCA_BASE_VAR_TYPE_INT, NULL, 0, 0,
                          PMIX_INFO_LVL_2,
                          MCA_BASE_VAR_SCOPE_READONLY,
                          &pmix_states_globals.default_type);
    return PMIX_SUCCESS;
}

static pmix_status_t pmix_state_close(void)
{
    if (!pmix_states_globals.initialized) {
        return PMIX_SUCCESS;
    }
    pmix_states_globals.initialized = false;

    /* the components will cleanup when closed */
    PMIX_DESTRUCT(&pmix_states_globals.actives);

    return PMIX_SUCCESS;
}

static pmix_status_t pmix_state_open(mca_base_open_flag_t flags)
{
  /* initialize globals */
  pmix_states_globals.initialized = true;
  PMIX_CONSTRUCT(&pmix_states_globals.actives, pmix_list_t);
  return PMIX_SUCCESS;
}

MCA_BASE_FRAMEWORK_DECLARE(pmix, states, "PMIx Buffer Operations",
                           pmix_state_register, pmix_state_open, pmix_state_close,
                           mca_states_base_static_components, 0);

PMIX_CLASS_INSTANCE(pmix_states_base_active_module_t,
                    pmix_list_item_t,
                    NULL, NULL);

/**
 * Object constructors, destructors, and instantiations
 */
/** Value **/
static void pmix_buffer_construct (pmix_buffer_t* buffer)
{
    /** set the default buffer type */
    buffer->type = pmix_states_globals.default_type;

    /* Make everything NULL to begin with */
    buffer->base_ptr = buffer->pack_ptr = buffer->unpack_ptr = NULL;
    buffer->bytes_allocated = buffer->bytes_used = 0;
}

static void pmix_buffer_destruct (pmix_buffer_t* buffer)
{
    if (NULL != buffer->base_ptr) {
        free (buffer->base_ptr);
    }
}

PMIX_CLASS_INSTANCE(pmix_buffer_t,
                   pmix_object_t,
                   pmix_buffer_construct,
                   pmix_buffer_destruct);


static void pmix_state_type_info_construct(pmix_state_type_info_t *obj)
{
    obj->odti_name = NULL;
    obj->odti_pack_fn = NULL;
    obj->odti_unpack_fn = NULL;
    obj->odti_copy_fn = NULL;
    obj->odti_print_fn = NULL;
}

static void pmix_state_type_info_destruct(pmix_state_type_info_t *obj)
{
    if (NULL != obj->odti_name) {
        free(obj->odti_name);
    }
}

PMIX_CLASS_INSTANCE(pmix_state_type_info_t, pmix_object_t,
                   pmix_state_type_info_construct,
                   pmix_state_type_info_destruct);

static void kvcon(pmix_kval_t *k)
{
    k->key = NULL;
    k->value = NULL;
}
static void kvdes(pmix_kval_t *k)
{
    if (NULL != k->key) {
        free(k->key);
    }
    if (NULL != k->value) {
        PMIX_VALUE_RELEASE(k->value);
    }
}
PMIX_CLASS_INSTANCE(pmix_kval_t,
                   pmix_list_item_t,
                   kvcon, kvdes);

static void rcon(pmix_regex_range_t *p)
{
    p->start = 0;
    p->cnt = 0;
}
PMIX_CLASS_INSTANCE(pmix_regex_range_t,
                    pmix_list_item_t,
                    rcon, NULL);

static void rvcon(pmix_regex_value_t *p)
{
    p->prefix = NULL;
    p->suffix = NULL;
    p->num_digits = 0;
    PMIX_CONSTRUCT(&p->ranges, pmix_list_t);
}
static void rvdes(pmix_regex_value_t *p)
{
    if (NULL != p->prefix) {
        free(p->prefix);
    }
    if (NULL != p->suffix) {
        free(p->suffix);
    }
    PMIX_LIST_DESTRUCT(&p->ranges);
}
PMIX_CLASS_INSTANCE(pmix_regex_value_t,
                    pmix_list_item_t,
                    rvcon, rvdes);

