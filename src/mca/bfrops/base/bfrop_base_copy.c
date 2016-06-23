/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2014-2016 Intel, Inc. All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>


#include "src/util/argv.h"
#include "src/util/error.h"
#include "src/util/output.h"
#include "src/runtime/ww_rte.h"

#include "src/mca/bfrops/base/base.h"

ww_status_t ww_bfrop_base_copy(struct ww_peer_t *pr,
                               void **dest, void *src,
                               ww_data_type_t type)
{
    ww_bfrop_type_info_t *info;
    ww_peer_t *peer = (ww_peer_t*)pr;

    /* check for error */
    if (NULL == dest || NULL == src ||
        NULL == peer->comm.bfrops) {
        WW_ERROR_LOG(WW_ERR_BAD_PARAM);
        return WW_ERR_BAD_PARAM;
    }

    /* Lookup the copy function for this type and call it */
    if (NULL == (info = (ww_bfrop_type_info_t*)ww_pointer_array_get_item(peer->comm.bfrops->types, type))) {
        WW_ERROR_LOG(WW_ERR_UNKNOWN_DATA_TYPE);
        return WW_ERR_UNKNOWN_DATA_TYPE;
    }

    return info->odti_copy_fn(pr, dest, src, type);
}

ww_status_t ww_bfrop_base_copy_payload(struct ww_peer_t *pr,
                                           ww_buffer_t *dest,
                                           ww_buffer_t *src)
{
    size_t to_copy = 0;
    char *ptr;

    /* deal with buffer type */
    if( NULL == dest->base_ptr ){
        /* destination buffer is empty - derive src buffer type */
        dest->type = src->type;
    } else if( dest->type != src->type ){
        /* buffer types mismatch */
        WW_ERROR_LOG(WW_ERR_BAD_PARAM);
        return WW_ERR_BAD_PARAM;
    }

    to_copy = src->pack_ptr - src->unpack_ptr;
    if( NULL == (ptr = ww_bfrop_buffer_extend(dest, to_copy)) ){
        WW_ERROR_LOG(WW_ERR_OUT_OF_RESOURCE);
        return WW_ERR_OUT_OF_RESOURCE;
    }
    memcpy(ptr,src->unpack_ptr, to_copy);
    dest->bytes_used += to_copy;
    dest->pack_ptr += to_copy;
    return WW_SUCCESS;
}


/*
 * STANDARD COPY FUNCTION - WORKS FOR EVERYTHING NON-STRUCTURED
 */
 ww_status_t ww_bfrop_std_copy(struct ww_peer_t *pr,
                                   void **dest, void *src,
                                   ww_data_type_t type)
{
    size_t datasize;
    uint8_t *val = NULL;

    switch(type) {
    case WW_BOOL:
        datasize = sizeof(bool);
        break;

    case WW_INT:
    case WW_UINT:
        datasize = sizeof(int);
        break;

    case WW_SIZE:
        datasize = sizeof(size_t);
        break;

    case WW_PID:
        datasize = sizeof(pid_t);
        break;

    case WW_BYTE:
    case WW_INT8:
    case WW_UINT8:
        datasize = 1;
        break;

    case WW_INT16:
    case WW_UINT16:
        datasize = 2;
        break;

    case WW_INT32:
    case WW_UINT32:
        datasize = 4;
        break;

    case WW_INT64:
    case WW_UINT64:
        datasize = 8;
        break;

    case WW_FLOAT:
        datasize = sizeof(float);
        break;

    case WW_TIMEVAL:
        datasize = sizeof(struct timeval);
        break;

    case WW_TIME:
        datasize = sizeof(time_t);
        break;

    case WW_STATUS:
        datasize = sizeof(ww_status_t);
        break;

    case WW_POINTER:
        datasize = sizeof(char*);
        break;

    default:
        return WW_ERR_UNKNOWN_DATA_TYPE;
    }

    val = (uint8_t*)malloc(datasize);
    if (NULL == val) {
        return WW_ERR_OUT_OF_RESOURCE;
    }

    memcpy(val, src, datasize);
    *dest = val;

    return WW_SUCCESS;
}

/* COPY FUNCTIONS FOR NON-STANDARD SYSTEM TYPES */

/*
 * STRING
 */
 ww_status_t ww_bfrop_copy_string(struct ww_peer_t *pr,
                                      char **dest, char *src,
                                      ww_data_type_t type)
{
    if (NULL == src) {  /* got zero-length string/NULL pointer - store NULL */
        *dest = NULL;
    } else {
        *dest = strdup(src);
    }

    return WW_SUCCESS;
}

/* WW_VALUE */
ww_status_t ww_bfrop_copy_value(struct ww_peer_t *pr,
                                    ww_value_t **dest, ww_value_t *src,
                                    ww_data_type_t type)
{
    ww_value_t *p;

    /* create the new object */
    *dest = (ww_value_t*)malloc(sizeof(ww_value_t));
    if (NULL == *dest) {
        return WW_ERR_OUT_OF_RESOURCE;
    }
    p = *dest;

    /* copy the type */
    p->type = src->type;
    /* copy the data */
    return ww_base_value_xfer(pr, p, src);
}

ww_status_t ww_bfrop_copy_info(struct ww_peer_t *pr,
                                   ww_info_t **dest, ww_info_t *src,
                                   ww_data_type_t type)
{
    *dest = (ww_info_t*)malloc(sizeof(ww_info_t));
    (void)strncpy((*dest)->key, src->key, WW_MAX_KEYLEN);
    (*dest)->required = src->required;
    return ww_base_value_xfer(pr, &(*dest)->value, &src->value);
}

ww_status_t ww_bfrop_copy_buf(struct ww_peer_t *pr,
                                  ww_buffer_t **dest, ww_buffer_t *src,
                                  ww_data_type_t type)
{
    *dest = WW_NEW(ww_buffer_t);
    ww_bfrop_base_copy_payload(pr, *dest, src);
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_copy_kval(struct ww_peer_t *pr,
                                   ww_kval_t **dest, ww_kval_t *src,
                                   ww_data_type_t type)
{
    ww_kval_t *p;

    /* create the new object */
    *dest = WW_NEW(ww_kval_t);
    if (NULL == *dest) {
        return WW_ERR_OUT_OF_RESOURCE;
    }
    p = *dest;

    /* copy the type */
    p->value->type = src->value->type;
    /* copy the data */
    return ww_base_value_xfer(pr, p->value, src->value);
}

ww_status_t ww_bfrop_copy_array(struct ww_peer_t *pr,
                                    ww_info_array_t **dest,
                                    ww_info_array_t *src,
                                    ww_data_type_t type)
{
    ww_info_t *d1, *s1;

    *dest = (ww_info_array_t*)malloc(sizeof(ww_info_array_t));
    (*dest)->size = src->size;
    (*dest)->array = (struct ww_info_t*)malloc(src->size * sizeof(ww_info_t));
    d1 = (ww_info_t*)(*dest)->array;
    s1 = (ww_info_t*)src->array;
    memcpy(d1, s1, src->size * sizeof(ww_info_t));
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_copy_proc(struct ww_peer_t *pr,
                                   ww_proc_t **dest, ww_proc_t *src,
                                   ww_data_type_t type)
{
    *dest = (ww_proc_t*)malloc(sizeof(ww_proc_t));
    if (NULL == *dest) {
        return WW_ERR_OUT_OF_RESOURCE;
    }
    (void)strncpy((*dest)->nspace, src->nspace, WW_MAX_NSLEN);
    (*dest)->rank = src->rank;
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_copy_bo(struct ww_peer_t *pr,
                                 ww_byte_object_t **dest, ww_byte_object_t *src,
                                 ww_data_type_t type)
{
    *dest = (ww_byte_object_t*)malloc(sizeof(ww_byte_object_t));
    if (NULL == *dest) {
        return WW_ERR_OUT_OF_RESOURCE;
    }
    (*dest)->bytes = (char*)malloc(src->size);
    memcpy((*dest)->bytes, src->bytes, src->size);
    (*dest)->size = src->size;
    return WW_SUCCESS;
}

