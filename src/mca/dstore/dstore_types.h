/* -*- C -*-
 *
 * Copyright (c) 2016      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
/**
 * @file
 *
 * Buffer management types.
 */

#ifndef WW_DSTORE_TYPES_H_
#define WW_DSTORE_TYPES_H_

#include <src/include/ww_config.h>

#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "src/class/ww_object.h"
#include "src/class/ww_list.h"
#include "src/class/ww_pointer_array.h"
#include "src/threads/threads.h"

BEGIN_C_DECLS

/* define the results values for comparisons so we can change them in only one place */
typedef enum {
    WW_EQUAL = 0,
    WW_VALUE1_GREATER,
    WW_VALUE2_GREATER
} ww_value_cmp_t;

/* object for storing data in hash tables */
typedef struct {
    ww_list_item_t super;
    char *key;
    char **values;
} ww_kval_t;
WW_CLASS_DECLARATION(ww_kval_t);

/* Warewulf-maintained configuration metadata
 * NOTE: times are stored as strings since the clock of
 * a machine reading the data may not match the clock of
 * the machine that last stored it */
typedef struct ww_metadata_t {
    size_t ww_version_cnt;      // counts number of times config has been modified since its creation
    char *atime;                // last time this config was accessed
    char *mtime;                // last time this config was modified
    char *ctime;                // when this config was created (version 0)
} ww_metadata_t;

/* Datastore-maintained configuration metadata
 * NOTE: times are stored as strings since the clock of
 * a machine reading the data may not match the clock of
 * the machine that last stored it */
typedef struct ww_dsmeta_t {
    ww_list_item_t super;
    char *name;             // string identifier of this dstore (typically the plugin name)
    char *version;          // version string assigned by the dstore upon the last commit
    char *atime;            // last access time recorded by the dstore, if available
    char *mtime;            // last modifcation time recorded by the dstore, if available
    char *ctime;            // when this config was initially committed to this dstore
} ww_dsmeta_t;
WW_CLASS_DECLARATION(ww_dsmeta_t);

/* define a high-level object for storing types of building
 * blocks. The configuration is assumed to be defined
 * in basic building blocks - e.g., "nodes" and "vnfs". We
 * define an object for holding all the instances for a
 * given type of building block
 */
typedef struct ww_type_object_t {
    ww_object_t super;
    char *type;
    ww_pointer_array_t blocks;
    size_t nblocks;
} ww_type_object_t;
WW_CLASS_DECLARATION(ww_type_object_t);

/* define a middle-level object for storing the key-value
 * descriptions of a building block. For example, an object
 * might store the information describing a single node, with
 * the object subsequently stored in the "blocks" array of
 * the "nodes" type object
 */
typedef struct ww_building_block_t {
    ww_object_t super;
    ww_mutex_t mutex;
    ww_condition_t cond;
    char *uuid;
    ww_pointer_array_t keyvals;
    size_t nkvals;
} ww_building_block_t;
WW_CLASS_DECLARATION(ww_building_block_t);

/* define the master object that contains the entire configuration */
typedef struct ww_configuration_t {
    ww_object_t super;
    char *name;
    bool modified;                  // flags that this config has been modified since last commit
    ww_list_t dstores;              // list of ww_dsmeta_t from all dstores containing this config
    ww_metadata_t ww_metadata;      // cache of current state of this config
    ww_list_t attributes;           // list of ww_kval_t attributes describing this configuration
    ww_pointer_array_t types;       // array of ww_type_object_t
} ww_configuration_t;
WW_CLASS_DECLARATION(ww_configuration_t);

/* define an object for returning results from a find request
 * We need this to allow us to return the building blocks on
 * a list, which is a convenient way for caller's to consume
 * the data. Unfortunately, objects can only be on one list
 * at a time, so to support multi-threaded operations we
 * have to provide a wrapper to hold the building block.
 */
typedef struct ww_result_t {
    ww_list_item_t super;
    ww_building_block_t *block;
} ww_result_t;
WW_CLASS_DECLARATION(ww_result_t);


/****    Warewulf Defined Directives    ****/

/* General behaviors */
#define WW_VERSIONING_REQUIRED      "ww.vsn.reqd"           // dstore versioning support is required
#define WW_PREFERRED_DSTORE         "ww.pref"               // name of the preferred dstore, if available
#define WW_REQUIRED_DSTORE          "ww.reqd"               // name of the mandatory, required dstore - operation
                                                            //     must fail if not available
#define WW_ALL_DSTORES              "ww.all.dstore"         // replicate this configuration into all available dstores

/* Search directives */
#define WW_SRCH_EXACT_MATCH         "ww.srch.xct"           // only return values that exactly match the given criteria
#define WW_SRCH_PARTIAL_MATCH       "ww.srch.part"          // return values that match at least one given criteria
#define WW_SRCH_TEMPLATE_MATCH      "ww.srch.tmplt"         // use the provided value as a template and return values
                                                            //     that match the given regex

/* Set directives */
#define WW_SET_OVERWRITE_ALLOWED    "ww.overwrite"          // okay to overwrite an existing value
#define WW_SET_OVERWRITE_ERROR      "ww.overerr"            // return an error if operation would overwrite an existing value
#define WW_SET_APPEND_DATA          "ww.append"             // append given data to any existing data for this key
#define WW_SET_PREPEND_DATA         "ww.prepend"            // prepend given data to any existing data for this key
#define WW_SET_UPDATE_ONLY          "ww.update"             // update an existing value - return an error if not pre-existing

/* Comparison directives */
#define WW_CMP_INCLUDE_METADATA     "ww.cmp.incmeta"        // include metadata in the overall comparison
#define WW_CMP_METADATA_ONLY        "ww.cmp.metaonly"       // only compare the Warewulf-maintained metadata
#define WW_CMP_DSMETA_ONLY          "ww.cmp.distonly"       // only compare the datastore metadata
#define WW_CMP_METADATA             "ww.cmp.bothmeta"       // compare both sets of metadata

END_C_DECLS

#endif /* WW_DSTORE_TYPES_H */
