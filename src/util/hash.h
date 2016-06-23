/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2012      Los Alamos National Security, Inc. All rights reserved.
 * Copyright (c) 2014-2015 Intel, Inc. All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef WW_HASH_H
#define WW_HASH_H

#include <src/include/ww_config.h>


#include "src/mca/bfrops/bfrops.h"
#include "src/class/ww_hash_table.h"

BEGIN_C_DECLS

/* store a value in the given hash table for the specified
 * rank index.*/
ww_status_t ww_hash_store(ww_hash_table_t *table,
                              int rank, ww_kval_t *kv);

/* Fetch the value for a specified key and rank from within
 * the given hash_table */
ww_status_t ww_hash_fetch(ww_hash_table_t *table, int rank,
                              const char *key, ww_value_t **kvs);

/* Fetch the value for a specified key from within
 * the given hash_table
 * It gets the next portion of data from table, where matching key.
 * To get the first data from table, function is called with key parameter as string.
 * Remaining data from table are obtained by calling function with a null pointer for the key parameter.*/
ww_status_t ww_hash_fetch_by_key(ww_hash_table_t *table, const char *key,
                                     int *rank, ww_value_t **kvs, void **last);

/* remove the specified key-value from the given hash_table.
 * A NULL key will result in removal of all data for the
 * given rank. A rank of WW_RANK_WILDCARD indicates that
 * the specified key  is to be removed from the data for all
 * ranks in the table. Combining key=NULL with rank=WW_RANK_WILDCARD
 * will therefore result in removal of all data from the
 * table */
ww_status_t ww_hash_remove_data(ww_hash_table_t *table,
                                    int rank, const char *key);

END_C_DECLS

#endif /* WW_HASH_H */
