/* -*- C -*-
 *
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
 * Copyright (c) 2007-2011 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2012-2013 Los Alamos National Security, Inc. All rights reserved.
 * Copyright (c) 2014-2016 Intel, Inc. All rights reserved.
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

#ifndef WW_BFROP_TYPES_H_
#define WW_BFROP_TYPES_H_

#include <src/include/ww_config.h>


#include "src/class/ww_object.h"
#include "src/class/ww_pointer_array.h"
#include "src/class/ww_list.h"
#include <ww_types.h>

BEGIN_C_DECLS

/* define the results values for comparisons so we can change them in only one place */
typedef enum {
    WW_EQUAL = 0,
    WW_VALUE1_GREATER,
    WW_VALUE2_GREATER
} ww_value_cmp_t;

/**
 * buffer type
 */
enum ww_bfrop_buffer_type_t {
    WW_BFROP_BUFFER_NON_DESC   = 0x00,
    WW_BFROP_BUFFER_FULLY_DESC = 0x01
};

typedef enum ww_bfrop_buffer_type_t ww_bfrop_buffer_type_t;

#define WW_BFROP_BUFFER_TYPE_HTON(h);
#define WW_BFROP_BUFFER_TYPE_NTOH(h);

/* internally used object for transferring data
 * to/from the server and for storing in the
 * hash tables */
typedef struct {
    ww_list_item_t super;
    char *key;
    ww_value_t *value;
} ww_kval_t;
WW_CLASS_DECLARATION(ww_kval_t);


/**
 * Structure for holding a buffer */
typedef struct {
    /** First member must be the object's parent */
    ww_object_t parent;
    /** type of buffer */
    ww_bfrop_buffer_type_t type;
    /** Start of my memory */
    char *base_ptr;
    /** Where the next data will be packed to (within the allocated
        memory starting at base_ptr) */
    char *pack_ptr;
    /** Where the next data will be unpacked from (within the
        allocated memory starting as base_ptr) */
    char *unpack_ptr;

    /** Number of bytes allocated (starting at base_ptr) */
    size_t bytes_allocated;
    /** Number of bytes used by the buffer (i.e., amount of data --
        including overhead -- packed in the buffer) */
    size_t bytes_used;
} ww_buffer_t;
WW_CLASS_DECLARATION(ww_buffer_t);

/* provide a macro specifically to construct a
 * new buffer object - requires passing a pointer
 * to the target buffer object, and a pointer to the
 * peer this buffer will be sent to */
#define WW_BUFFER_CONSTRUCT(a, b)                 \
    do {                                            \
        WW_OBJ_CONSTRUCT((a), ww_buffer_t);     \
        (a)->type = (b)->comm.type;                 \
    } while(0);

/* do the same for getting a newly malloc'd buffer -
 * requires passing a pointer to the target buffer object,
 * and a pointer to the peer this buffer will be sent to */
#define WW_BUFFER_NEW(a, b)                       \
    do {                                            \
        (a) = WW_OBJ_NEW(ww_buffer_t);          \
        (a)->type = (b)->comm.type;                 \
    } while(0);

/* these classes are required by the regex code shared
 * between the client and server implementations - it
 * is put here so that both can access these objects */
typedef struct {
    ww_list_item_t super;
    int start;
    int cnt;
} ww_regex_range_t;
WW_CLASS_DECLARATION(ww_regex_range_t);

typedef struct {
    /* list object */
    ww_list_item_t super;
    char *prefix;
    char *suffix;
    int num_digits;
    ww_list_t ranges;
} ww_regex_value_t;
WW_CLASS_DECLARATION(ww_regex_value_t);

#define WW_LOAD_BUFFER(b, d, s)                       \
    do {                                                \
        (b)->base_ptr = (char*)(d);                     \
        (b)->bytes_used = (s);                          \
        (b)->bytes_allocated = (s);                     \
        (b)->pack_ptr = ((char*)(b)->base_ptr) + (s);   \
        (b)->unpack_ptr = (b)->base_ptr;                \
        (d) = NULL;                                     \
        (s) = 0;                                        \
    } while (0)

#define WW_UNLOAD_BUFFER(b, d, s)             \
    do {                                        \
        (d) = (char*)(b)->unpack_ptr;           \
        (s) = (b)->bytes_used;                  \
        (b)->base_ptr = NULL;                   \
        (b)->bytes_used = 0;                    \
        (b)->bytes_allocated = 0;               \
        (b)->pack_ptr = NULL;                   \
        (b)->unpack_ptr = NULL;                 \
    } while (0)


END_C_DECLS

#endif /* WW_BFROP_TYPES_H */
