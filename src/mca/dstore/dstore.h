/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2016      Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
/**
 * @file
 *
 * Data packing subsystem.
 */

#ifndef WW_DSTORE_H_
#define WW_DSTORE_H_

#include <src/include/ww_config.h>

#include <ww_types.h>

#include "src/mca/mca.h"

#include "dstore_types.h"

BEGIN_C_DECLS

/* Load a configuration object from the datastore into
 * the working cache. All changes made to the cached object
 * are not reflected in the datastore until a "commit" is done.
 *
 * The name param indicates which configuration is to be loaded.
 *
 * The directives param consists of an optional list of ww_kval_t
 * that provide directives influencing such as the source of the load
 * (i.e., the name of the dstore component from which the configuration
 * is to be fetched), the desired version of the configuration to be
 * loaded, etc.
 *
 * A NULL value will be returned if the requested configuration is not found
 */
typedef ww_configuration_t* (*ww_dstore_base_module_load_fn_t)(char *name, ww_list_t *directives);

/* Commit a configuration object to the datastore. The provided object
 * can either be a new one, or an update to an existing stored configuration.
 * The selected persistent datastore component may or may not retain any
 * prior versions of the configuration unless the WW_VERSIONING_REQUIRED
 * attribute is provided - in that case, an error will be returned if
 * none of the selected datastore components offer versioning support.
 */
typedef ww_status_t (*ww_dstore_base_module_commit_fn_t)(ww_configuration_t *config,
                                                         ww_list_t *directives);

/* Search and return all ww_building_block_t from the specified type
 * in the given configuration that satisfy the specified search criteria.
 * Only one key can be provided, but the key can contain multiple values
 * to check against as per the directives.
 *
 * For example, one might want to search for the "interface" key, asking
 * to return only those matches that start with "eth". In this case, one
 * would specify a key of WW_INTERFACE, and the directives list would
 * include an attribute WW_SRCH_TEMPLATE_MATCH which had a value of "eth*".
 * This would return objects for "eth0" and "eth1", but not for "inet0".
 * Providing a NULL for the directives list would return all objects
 * identified by the WW_INTERFACE key.
 *
 * Results will be added to the provide ww_list_t list. Objects on the
 * list will have their reference counts incremented, and the caller is
 * responsible for decrementing those counters by calling WW_RELEASE
 * on the objects (e.g., via the WW_LIST_RELEASE or WW_LIST_DESTRUCT macros)
 *
 * NOTE: multi-threaded operations can result in the configuration being
 * modified, including removal of the building blocks included in the
 * list of results, after completion of this function. It is therefore
 * advised that the configuration be locked throughout any find/modify/commit
 * operation.
 */
typedef ww_status_t (*ww_dstore_base_module_find_fn_t)(ww_configuration_t *config,
                                                       char *type, char *key,
                                                       ww_list_t *directives,
                                                       ww_list_t *results);

/* Get specific key-value objects from within a building_block.
*/

/* Set a value in the given building_block. The function will search the
 * building_block for the key matching that of the given ww_kval_t. If not
 * found, then the key-value pair will be added to the building_block unless
 * the WW_SET_UPDATE_ONLY attribute is given - in that case, a failure to
 * find a pre-existing matching key will result in return of an error. If
 * the matching key is found, then the operation will default to overwriting
 * the values pending any provided directives.
 *
 * Supported directives include those in the Search Directives section
 *
 * NOTE: this function includes the required thread-protection for accessing
 * and modifying the provided building block object
 */
typedef ww_status_t (*ww_dstore_base_module_set_fn_t)(ww_building_block_t *block,
                                                      ww_kval_t *kv, ww_list_t *directives);

/* Compare two configurations. The two configurations will be equal
 * if all key-value pairs within them are the same - i.e., any key
 * found in one configuration is also found in the other, and all
 * values for those keys match. Metadata will not be included
 * in the comparison unless directed as below.
 *
 * Supported directives include those in the Comparison Directives
 * section
 *
 * NOTE: if the WW_CMP_INCLUDE_METADATA attribute is not given, then
 * the last three directives will result in _only_ metadata comparisons
 *
 * NOTE: both configurations will be thread-locked during the comparison
 * operation
 */
typedef ww_value_cmp_t (*ww_dstore_base_module_cmp_fn_t)(ww_configuration_t *config1,
                                                         ww_configuration_t *config2,
                                                         ww_list_t *directives);

/**
 * Structure for a DSTORE module - the individual plugins
 * really only need to provide the load and commit APIs. All
 * other support should be common across them.
 */
struct ww_dstore_module_t {
    ww_dstore_base_module_load_fn_t     load;
    ww_dstore_base_module_commit_fn_t   commit;
};
typedef struct ww_dstore_module_t ww_dstore_module_t;


/* Structure for the public APIs - since we may have multiple
 * active dstore plugins, all operations will flow thru a set
 * of base functions. This provides a nice, clean way of
 * accessing those */
typedef struct ww_dstore_API_t {
    ww_dstore_base_module_load_fn_t     load;
    ww_dstore_base_module_commit_fn_t   commit;
    ww_dstore_base_module_find_fn_t     find;
    ww_dstore_base_module_set_fn_t      set;
    ww_dstore_base_module_cmp_fn_t      compare;
} ww_dstore_API_t;

/* a set of base functions for executing calls */
extern ww_dstore_API_t ww_dstore;

/*
 * the standard component data structure
 */
struct ww_dstore_base_component_t {
    mca_base_component_t                            base;
    mca_base_component_data_t                       data;
};
typedef struct ww_dstore_base_component_t ww_dstore_base_component_t;


/*
 * Macro for use in components that are of type dstore
 */
#define WW_DSTORE_BASE_VERSION_1_0_0 \
    WW_MCA_BASE_VERSION_1_0_0("dstore", 1, 0, 0)

END_C_DECLS

#endif /* WW_DSTORE_H */
