/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2004-2011 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2011-2014 Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2014-2016 Intel, Inc. All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * Copyright (c) 2016      Mellanox Technologies, Inc.
 *                         All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 */

#include <src/include/ww_config.h>

#include <src/include/ww_stdint.h>
#include <src/include/hash_string.h>

#include <string.h>

#include "src/runtime/ww_rte.h"
#include "src/class/ww_hash_table.h"
#include "src/class/ww_pointer_array.h"
#include "src/mca/bfrops/bfrops.h"
#include "src/util/error.h"
#include "src/util/output.h"

#include "src/util/hash.h"

/**
 * Data for a particular ww process
 * The name association is maintained in the
 * proc_data hash table.
 */
typedef struct {
    /** Structure can be put on lists (including in hash tables) */
    ww_list_item_t super;
    /* List of ww_kval_t structures containing all data
       received from this process */
    ww_list_t data;
} ww_proc_data_t;
static void pdcon(ww_proc_data_t *p)
{
    WW_CONSTRUCT(&p->data, ww_list_t);
}
static void pddes(ww_proc_data_t *p)
{
    WW_LIST_DESTRUCT(&p->data);
}
static WW_CLASS_INSTANCE(ww_proc_data_t,
                           ww_list_item_t,
                           pdcon, pddes);

static ww_kval_t* lookup_keyval(ww_list_t *data,
                                  const char *key);
static ww_proc_data_t* lookup_proc(ww_hash_table_t *jtable,
                                     uint64_t id, bool create);

int ww_hash_store(ww_hash_table_t *table,
                    int rank, ww_kval_t *kin)
{
    ww_proc_data_t *proc_data;
    uint64_t id;
    ww_kval_t *hv;

    ww_output_verbose(10, ww_globals.debug_output,
                        "HASH:STORE rank %d key %s",
                        rank, kin->key);

    id = (uint64_t)rank;

    /* lookup the proc data object for this proc - create
     * it if we don't already have it */
    if (NULL == (proc_data = lookup_proc(table, id, true))) {
        return WW_ERR_OUT_OF_RESOURCE;
    }

    /* see if we already have this key-value */
    hv = lookup_keyval(&proc_data->data, kin->key);
    if (NULL != hv) {
        /* yes we do - so remove the current value
         * and replace it */
        ww_list_remove_item(&proc_data->data, &hv->super);
        WW_RELEASE(hv);
    }
    WW_RETAIN(kin);
    ww_list_append(&proc_data->data, &kin->super);

    return WW_SUCCESS;
}

ww_status_t ww_hash_fetch(ww_hash_table_t *table, int rank,
                              const char *key, ww_value_t **kvs)
{
    ww_status_t rc = WW_SUCCESS;
    ww_proc_data_t *proc_data;
    ww_kval_t *hv;
    uint64_t id;
    char *node;

    ww_output_verbose(10, ww_globals.debug_output,
                        "HASH:FETCH rank %d key %s",
                        rank, (NULL == key) ? "NULL" : key);

    id = (uint64_t)rank;

    /* - WW_RANK_UNDEF should return following statuses
     * WW_ERR_PROC_ENTRY_NOT_FOUND | WW_SUCCESS
     * - specified rank can return following statuses
     * WW_ERR_PROC_ENTRY_NOT_FOUND | WW_ERR_NOT_FOUND | WW_SUCCESS
     * special logic is basing on these statuses on a client and a server */
    if (WW_RANK_UNDEF == rank) {
        rc = ww_hash_table_get_first_key_uint64(table, &id,
                (void**)&proc_data, (void**)&node);
        if (WW_SUCCESS != rc) {
            ww_output_verbose(10, ww_globals.debug_output,
                                "HASH:FETCH proc data for rank %d not found",
                                rank);
            return WW_ERR_PROC_ENTRY_NOT_FOUND;
        }
    }

    while (WW_SUCCESS == rc) {
        proc_data = lookup_proc(table, id, false);
        if (NULL == proc_data) {
            ww_output_verbose(10, ww_globals.debug_output,
                                "HASH:FETCH proc data for rank %d not found",
                                rank);
            return WW_ERR_PROC_ENTRY_NOT_FOUND;
        }

        /* if the key is NULL, then the user wants -all- data
         * put by the specified rank */
        if (NULL == key) {
            /* we will return the data as an array of ww_info_t
             * in the kvs ww_value_t */

        } else {
            /* find the value from within this proc_data object */
            hv = lookup_keyval(&proc_data->data, key);
            if (NULL != hv) {
                /* create the copy */
                if (WW_SUCCESS != (rc = ww_bfrops.copy(ww_globals.mypeer, (void**)kvs, hv->value, WW_VALUE))) {
                    WW_ERROR_LOG(rc);
                    return rc;
                }
                break;
            } else if (WW_RANK_UNDEF != rank) {
                ww_output_verbose(10, ww_globals.debug_output,
                                    "HASH:FETCH data for key %s not found", key);
                return WW_ERR_NOT_FOUND;
            }
        }

        rc = ww_hash_table_get_next_key_uint64(table, &id,
                (void**)&proc_data, node, (void**)&node);
        if (WW_SUCCESS != rc) {
            ww_output_verbose(10, ww_globals.debug_output,
                                "HASH:FETCH data for key %s not found", key);
            return WW_ERR_PROC_ENTRY_NOT_FOUND;
        }
    }

    return rc;
}

ww_status_t ww_hash_fetch_by_key(ww_hash_table_t *table, const char *key,
                                     int *rank, ww_value_t **kvs, void **last)
{
    ww_status_t rc = WW_SUCCESS;
    ww_proc_data_t *proc_data;
    ww_kval_t *hv;
    uint64_t id;
    char *node;
    static const char *key_r = NULL;

    if (key == NULL && (node = *last) == NULL) {
        return WW_ERR_PROC_ENTRY_NOT_FOUND;
    }

    if (key == NULL && key_r == NULL) {
        return WW_ERR_PROC_ENTRY_NOT_FOUND;
    }

    if (key) {
        rc = ww_hash_table_get_first_key_uint64(table, &id,
                (void**)&proc_data, (void**)&node);
        key_r = key;
    } else {
        rc = ww_hash_table_get_next_key_uint64(table, &id,
                (void**)&proc_data, node, (void**)&node);
    }

    ww_output_verbose(10, ww_globals.debug_output,
                        "HASH:FETCH BY KEY rank %d key %s",
                        (int)id, key_r);

    if (WW_SUCCESS != rc) {
        ww_output_verbose(10, ww_globals.debug_output,
                            "HASH:FETCH proc data for key %s not found",
                            key_r);
        return WW_ERR_PROC_ENTRY_NOT_FOUND;
    }

    /* find the value from within this proc_data object */
    hv = lookup_keyval(&proc_data->data, key_r);
    if (hv) {
        /* create the copy */
        if (WW_SUCCESS != (rc = ww_bfrops.copy(ww_globals.mypeer, (void**)kvs, hv->value, WW_VALUE))) {
            WW_ERROR_LOG(rc);
            return rc;
        }
    } else {
        return WW_ERR_NOT_FOUND;
    }

    *rank = (int)id;
    *last = node;

    return WW_SUCCESS;
}

int ww_hash_remove_data(ww_hash_table_t *table,
                          int rank, const char *key)
{
    ww_status_t rc = WW_SUCCESS;
    ww_proc_data_t *proc_data;
    ww_kval_t *kv;
    uint64_t id;
    char *node;

    id = (uint64_t)rank;

    /* if the rank is wildcard, we want to apply this to
     * all rank entries */
    if (WW_RANK_UNDEF == rank) {
        rc = ww_hash_table_get_first_key_uint64(table, &id,
                (void**)&proc_data, (void**)&node);
        while (WW_SUCCESS == rc) {
            if (NULL != proc_data) {
                if (NULL == key) {
                    WW_RELEASE(proc_data);
                } else {
                    WW_LIST_FOREACH(kv, &proc_data->data, ww_kval_t) {
                        if (0 == strcmp(key, kv->key)) {
                            ww_list_remove_item(&proc_data->data, &kv->super);
                            WW_RELEASE(kv);
                            break;
                        }
                    }
                }
            }
            rc = ww_hash_table_get_next_key_uint64(table, &id,
                    (void**)&proc_data, node, (void**)&node);
        }
    }

    /* lookup the specified proc */
    if (NULL == (proc_data = lookup_proc(table, id, false))) {
        /* no data for this proc */
        return WW_SUCCESS;
    }

    /* if key is NULL, remove all data for this proc */
    if (NULL == key) {
        while (NULL != (kv = (ww_kval_t*)ww_list_remove_first(&proc_data->data))) {
            WW_RELEASE(kv);
        }
        /* remove the proc_data object itself from the jtable */
        ww_hash_table_remove_value_uint64(table, id);
        /* cleanup */
        WW_RELEASE(proc_data);
        return WW_SUCCESS;
    }

    /* remove this item */
    WW_LIST_FOREACH(kv, &proc_data->data, ww_kval_t) {
        if (0 == strcmp(key, kv->key)) {
            ww_list_remove_item(&proc_data->data, &kv->super);
            WW_RELEASE(kv);
            break;
        }
    }

    return WW_SUCCESS;
}

/**
 * Find data for a given key in a given ww_list_t.
 */
static ww_kval_t* lookup_keyval(ww_list_t *data,
                                  const char *key)
{
    ww_kval_t *kv;

    WW_LIST_FOREACH(kv, data, ww_kval_t) {
        if (0 == strcmp(key, kv->key)) {
            return kv;
        }
    }
    return NULL;
}


/**
 * Find proc_data_t container associated with given
 * ww_identifier_t.
 */
static ww_proc_data_t* lookup_proc(ww_hash_table_t *jtable,
                                     uint64_t id, bool create)
{
    ww_proc_data_t *proc_data = NULL;

    ww_hash_table_get_value_uint64(jtable, id, (void**)&proc_data);
    if (NULL == proc_data && create) {
        /* The proc clearly exists, so create a data structure for it */
        proc_data = WW_NEW(ww_proc_data_t);
        if (NULL == proc_data) {
            ww_output(0, "ww:client:hash:lookup_ww_proc: unable to allocate proc_data_t\n");
            return NULL;
        }
        ww_hash_table_set_value_uint64(jtable, id, proc_data);
    }

    return proc_data;
}
