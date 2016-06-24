/*
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
 * Copyright (c) 2015-2016 Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>


#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "src/class/ww_pointer_array.h"
#include "src/util/argv.h"
#include "src/util/error.h"

#include "src/mca/bfrops/base/base.h"

char* ww_bfrop_base_get_available_modules(void)
{
    ww_bfrops_base_active_module_t *active;
    char **tmp=NULL, *reply=NULL;

    if (!ww_bfrops_globals.initialized) {
        return NULL;
    }

    WW_LIST_FOREACH(active, &ww_bfrops_globals.actives, ww_bfrops_base_active_module_t) {
        ww_argv_append_nosize(&tmp, active->component->base.mca_component_name);
    }
    if (NULL != tmp) {
        reply = ww_argv_join(tmp, ',');
        ww_argv_free(tmp);
    }
    return reply;
}

ww_bfrops_module_t* ww_bfrop_base_assign_module(const char *version)
{
    ww_bfrops_base_active_module_t *active;
    ww_bfrops_module_t *mod;

    if (!ww_bfrops_globals.initialized) {
        return NULL;
    }

    WW_LIST_FOREACH(active, &ww_bfrops_globals.actives, ww_bfrops_base_active_module_t) {
        if (NULL != (mod = active->component->assign_module(version))) {
            return mod;
        }
    }

    /* we only get here if nothing was found */
    return NULL;
}

ww_status_t ww_bfrop_base_register_datatype(const char *name, ww_data_type_t type,
                                                ww_bfrop_pack_fn_t pack,
                                                ww_bfrop_unpack_fn_t unpack,
                                                ww_bfrop_copy_fn_t copy,
                                                ww_bfrop_print_fn_t print)
{
    ww_bfrops_base_active_module_t *active;
    ww_status_t rc=WW_SUCCESS;

    if (!ww_bfrops_globals.initialized) {
        return WW_ERR_INIT;
    }

    WW_LIST_FOREACH(active, &ww_bfrops_globals.actives, ww_bfrops_base_active_module_t) {
        if (WW_SUCCESS != (rc = active->component->register_type(name, type,
                                                                   pack, unpack,
                                                                   copy, print))) {
            return rc;
        }
    }

    return rc;
}

void ww_value_load(ww_value_t *v, void *data,
                     ww_data_type_t type)
{
    ww_base_value_load(NULL, v, data, type);
}

void ww_base_value_load(struct ww_peer_t *pr,
                          ww_value_t *v, void *data,
                          ww_data_type_t type)
{
    v->type = type;
    if (NULL == data) {
        /* just set the fields to zero */
        memset(&v->data, 0, sizeof(v->data));
    } else {
        switch(type) {
        case WW_UNDEF:
            break;
        case WW_BOOL:
            memcpy(&(v->data.flag), data, 1);
            break;
        case WW_BYTE:
            memcpy(&(v->data.byte), data, 1);
            break;
        case WW_STRING:
            v->data.string = strdup(data);
            break;
        case WW_SIZE:
            memcpy(&(v->data.size), data, sizeof(size_t));
            break;
        case WW_PID:
            memcpy(&(v->data.pid), data, sizeof(pid_t));
            break;
        case WW_INT:
            memcpy(&(v->data.integer), data, sizeof(int));
            break;
        case WW_INT8:
            memcpy(&(v->data.int8), data, 1);
            break;
        case WW_INT16:
            memcpy(&(v->data.int16), data, 2);
            break;
        case WW_INT32:
            memcpy(&(v->data.int32), data, 4);
            break;
        case WW_INT64:
            memcpy(&(v->data.int64), data, 8);
            break;
        case WW_UINT:
            memcpy(&(v->data.uint), data, sizeof(int));
            break;
        case WW_UINT8:
            memcpy(&(v->data.uint8), data, 1);
            break;
        case WW_UINT16:
            memcpy(&(v->data.uint16), data, 2);
            break;
        case WW_UINT32:
            memcpy(&(v->data.uint32), data, 4);
            break;
        case WW_UINT64:
            memcpy(&(v->data.uint64), data, 8);
            break;
        case WW_FLOAT:
            memcpy(&(v->data.fval), data, sizeof(float));
            break;
        case WW_DOUBLE:
            memcpy(&(v->data.dval), data, sizeof(double));
            break;
        case WW_TIMEVAL:
            memcpy(&(v->data.tv), data, sizeof(struct timeval));
            break;
        case WW_BYTE_OBJECT:
            v->data.bo.bytes = data;
            memcpy(&(v->data.bo.size), data, sizeof(size_t));
            break;
        case WW_TIME:
        case WW_VALUE:
        case WW_INFO_ARRAY:
        case WW_INFO:
        case WW_BUFFER:
        case WW_KVAL:
        case WW_PROC:
            /* silence warnings */
            break;
        }
    }
    return;
}

ww_status_t ww_base_value_unload(struct ww_peer_t *pr,
                                     ww_value_t *kv, void **data,
                                     size_t *sz, ww_data_type_t type)
{
    ww_status_t rc;

    rc = WW_SUCCESS;
    if (type != kv->type) {
        rc = WW_ERR_TYPE_MISMATCH;
    } else if (NULL == data ||
               (NULL == *data && WW_STRING != type && WW_BYTE_OBJECT != type)) {
        rc = WW_ERR_BAD_PARAM;
    } else {
        switch(type) {
        case WW_UNDEF:
            rc = WW_ERR_UNKNOWN_DATA_TYPE;
            break;
        case WW_BOOL:
            memcpy(*data, &(kv->data.flag), 1);
            *sz = 1;
            break;
        case WW_BYTE:
            memcpy(*data, &(kv->data.byte), 1);
            *sz = 1;
            break;
        case WW_STRING:
            if (NULL != kv->data.string) {
                *data = strdup(kv->data.string);
                *sz = strlen(kv->data.string);
            }
            break;
        case WW_SIZE:
            memcpy(*data, &(kv->data.size), sizeof(size_t));
            *sz = sizeof(size_t);
            break;
        case WW_PID:
            memcpy(*data, &(kv->data.pid), sizeof(pid_t));
            *sz = sizeof(pid_t);
            break;
        case WW_INT:
            memcpy(*data, &(kv->data.integer), sizeof(int));
            *sz = sizeof(int);
            break;
        case WW_INT8:
            memcpy(*data, &(kv->data.int8), 1);
            *sz = 1;
            break;
        case WW_INT16:
            memcpy(*data, &(kv->data.int16), 2);
            *sz = 2;
            break;
        case WW_INT32:
            memcpy(*data, &(kv->data.int32), 4);
            *sz = 4;
            break;
        case WW_INT64:
            memcpy(*data, &(kv->data.int64), 8);
            *sz = 8;
            break;
        case WW_UINT:
            memcpy(*data, &(kv->data.uint), sizeof(int));
            *sz = sizeof(int);
            break;
        case WW_UINT8:
            memcpy(*data, &(kv->data.uint8), 1);
            *sz = 1;
            break;
        case WW_UINT16:
            memcpy(*data, &(kv->data.uint16), 2);
            *sz = 2;
            break;
        case WW_UINT32:
            memcpy(*data, &(kv->data.uint32), 4);
            *sz = 4;
            break;
        case WW_UINT64:
            memcpy(*data, &(kv->data.uint64), 8);
            *sz = 8;
            break;
        case WW_FLOAT:
            memcpy(*data, &(kv->data.fval), sizeof(float));
            *sz = sizeof(float);
            break;
        case WW_DOUBLE:
            memcpy(*data, &(kv->data.dval), sizeof(double));
            *sz = sizeof(double);
            break;
        case WW_TIMEVAL:
            memcpy(*data, &(kv->data.tv), sizeof(struct timeval));
            *sz = sizeof(struct timeval);
            break;
        case WW_BYTE_OBJECT:
            if (NULL != kv->data.bo.bytes && 0 < kv->data.bo.size) {
                *data = kv->data.bo.bytes;
                *sz = kv->data.bo.size;
            } else {
                *data = NULL;
                *sz = 0;
            }
            break;
        case WW_TIME:
        case WW_VALUE:
        case WW_INFO_ARRAY:
        case WW_INFO:
        case WW_BUFFER:
        case WW_KVAL:
        case WW_PROC:
            /* silence warnings */
            rc = WW_ERROR;
            break;
        }
    }
    return rc;
}

/* compare function for ww_value_t */
ww_value_cmp_t ww_base_value_cmp(struct ww_peer_t *pr,
                                     ww_value_t *p,
                                     ww_value_t *p1)
{
    ww_value_cmp_t rc = WW_VALUE1_GREATER;
    switch (p->type) {
        case WW_BOOL:
            if (p->data.flag == p1->data.flag) {
                rc = WW_EQUAL;
            }
            break;
        case WW_BYTE:
            if (p->data.byte == p1->data.byte) {
                rc = WW_EQUAL;
            }
            break;
        case WW_SIZE:
            if (p->data.size == p1->data.size) {
                rc = WW_EQUAL;
            }
            break;
        case WW_INT:
            if (p->data.integer == p1->data.integer) {
                rc = WW_EQUAL;
            }
            break;
        case WW_INT8:
            if (p->data.int8 == p1->data.int8) {
                rc = WW_EQUAL;
            }
            break;
        case WW_INT16:
            if (p->data.int16 == p1->data.int16) {
                rc = WW_EQUAL;
            }
            break;
        case WW_INT32:
            if (p->data.int32 == p1->data.int32) {
                rc = WW_EQUAL;
            }
            break;
        case WW_INT64:
            if (p->data.int64 == p1->data.int64) {
                rc = WW_EQUAL;
            }
            break;
        case WW_UINT:
            if (p->data.uint == p1->data.uint) {
                rc = WW_EQUAL;
            }
            break;
        case WW_UINT8:
            if (p->data.uint8 == p1->data.int8) {
                rc = WW_EQUAL;
            }
            break;
        case WW_UINT16:
            if (p->data.uint16 == p1->data.uint16) {
                rc = WW_EQUAL;
            }
            break;
        case WW_UINT32:
            if (p->data.uint32 == p1->data.uint32) {
                rc = WW_EQUAL;
            }
            break;
        case WW_UINT64:
            if (p->data.uint64 == p1->data.uint64) {
                rc = WW_EQUAL;
            }
            break;
        case WW_STRING:
            if (0 == strcmp(p->data.string, p1->data.string)) {
                rc = WW_EQUAL;
            }
            break;
        case WW_STATUS:
            if (p->data.status == p1->data.status) {
                rc = WW_EQUAL;
            }
            break;
        default:
            ww_output(0, "COMPARE-WW-VALUE: UNSUPPORTED TYPE %d", (int)p->type);
    }
    return rc;
}

/* Xfer FUNCTIONS FOR GENERIC WW TYPES */
ww_status_t ww_base_value_xfer(struct ww_peer_t *pr,
                                   ww_value_t *dest,
                                   ww_value_t *src)
{
    ww_info_t *p1, *s1;

    /* copy the right field */
    dest->type = src->type;
    switch (src->type) {
    case WW_BOOL:
        dest->data.flag = src->data.flag;
        break;
    case WW_BYTE:
        dest->data.byte = src->data.byte;
        break;
    case WW_STRING:
        if (NULL != src->data.string) {
            dest->data.string = strdup(src->data.string);
        } else {
            dest->data.string = NULL;
        }
        break;
    case WW_SIZE:
        dest->data.size = src->data.size;
        break;
    case WW_PID:
        dest->data.pid = src->data.pid;
        break;
    case WW_INT:
        /* to avoid alignment issues */
        memcpy(&dest->data.integer, &src->data.integer, sizeof(int));
        break;
    case WW_INT8:
        dest->data.int8 = src->data.int8;
        break;
    case WW_INT16:
        /* to avoid alignment issues */
        memcpy(&dest->data.int16, &src->data.int16, 2);
        break;
    case WW_INT32:
        /* to avoid alignment issues */
        memcpy(&dest->data.int32, &src->data.int32, 4);
        break;
    case WW_INT64:
        /* to avoid alignment issues */
        memcpy(&dest->data.int64, &src->data.int64, 8);
        break;
    case WW_UINT:
        /* to avoid alignment issues */
        memcpy(&dest->data.uint, &src->data.uint, sizeof(unsigned int));
        break;
    case WW_UINT8:
        dest->data.uint8 = src->data.uint8;
        break;
    case WW_UINT16:
        /* to avoid alignment issues */
        memcpy(&dest->data.uint16, &src->data.uint16, 2);
        break;
    case WW_UINT32:
        /* to avoid alignment issues */
        memcpy(&dest->data.uint32, &src->data.uint32, 4);
        break;
    case WW_UINT64:
        /* to avoid alignment issues */
        memcpy(&dest->data.uint64, &src->data.uint64, 8);
        break;
    case WW_FLOAT:
        dest->data.fval = src->data.fval;
        break;
    case WW_DOUBLE:
        dest->data.dval = src->data.dval;
        break;
    case WW_TIMEVAL:
        dest->data.tv.tv_sec = src->data.tv.tv_sec;
        dest->data.tv.tv_usec = src->data.tv.tv_usec;
        break;
    case WW_STATUS:
        memcpy(&dest->data.status, &src->data.status, sizeof(ww_status_t));
        break;
    case WW_INFO_ARRAY:
        dest->data.array.size = src->data.array.size;
        if (0 < src->data.array.size) {
            dest->data.array.array = (struct ww_info_t*)malloc(src->data.array.size * sizeof(ww_info_t));
            p1 = (ww_info_t*)dest->data.array.array;
            s1 = (ww_info_t*)src->data.array.array;
            memcpy(p1, s1, src->data.array.size * sizeof(ww_info_t));
        }
        break;
    case WW_BYTE_OBJECT:
        if (NULL != src->data.bo.bytes && 0 < src->data.bo.size) {
            dest->data.bo.bytes = malloc(src->data.bo.size);
            memcpy(dest->data.bo.bytes, src->data.bo.bytes, src->data.bo.size);
            dest->data.bo.size = src->data.bo.size;
        } else {
            dest->data.bo.bytes = NULL;
            dest->data.bo.size = 0;
        }
        break;
    case WW_POINTER:
        memcpy(&dest->data.ptr, &src->data.ptr, sizeof(void*));
        break;
    default:
        ww_output(0, "COPY-WW-VALUE: UNSUPPORTED TYPE %d", (int)src->type);
        return WW_ERR_NOT_SUPPORTED;
    }
    return WW_SUCCESS;
}


/**
 * Internal function that resizes (expands) an inuse buffer if
 * necessary.
 */
char* ww_bfrop_buffer_extend(ww_buffer_t *buffer, size_t bytes_to_add)
{
    size_t required, to_alloc;
    size_t pack_offset, unpack_offset;

    /* Check to see if we have enough space already */

    if ((buffer->bytes_allocated - buffer->bytes_used) >= bytes_to_add) {
        return buffer->pack_ptr;
    }

    required = buffer->bytes_used + bytes_to_add;
    if (required >= ww_bfrops_globals.threshold_size) {
        to_alloc = ((required + ww_bfrops_globals.threshold_size - 1)
                    / ww_bfrops_globals.threshold_size) * ww_bfrops_globals.threshold_size;
    } else {
        to_alloc = buffer->bytes_allocated;
        if (0 == to_alloc) {
            to_alloc = ww_bfrops_globals.initial_size;
        }
        while (to_alloc < required) {
            to_alloc <<= 1;
        }
    }

    if (NULL != buffer->base_ptr) {
        pack_offset = ((char*) buffer->pack_ptr) - ((char*) buffer->base_ptr);
        unpack_offset = ((char*) buffer->unpack_ptr) -
            ((char*) buffer->base_ptr);
        buffer->base_ptr = (char*)realloc(buffer->base_ptr, to_alloc);
        memset(buffer->base_ptr + pack_offset, 0, to_alloc - buffer->bytes_allocated);
    } else {
        pack_offset = 0;
        unpack_offset = 0;
        buffer->bytes_used = 0;
        buffer->base_ptr = (char*)malloc(to_alloc);
        memset(buffer->base_ptr, 0, to_alloc);
    }

    if (NULL == buffer->base_ptr) {
        return NULL;
    }
    buffer->pack_ptr = ((char*) buffer->base_ptr) + pack_offset;
    buffer->unpack_ptr = ((char*) buffer->base_ptr) + unpack_offset;
    buffer->bytes_allocated = to_alloc;

    /* All done */

    return buffer->pack_ptr;
}

/*
 * Internal function that checks to see if the specified number of bytes
 * remain in the buffer for unpacking
 */
bool ww_bfrop_too_small(ww_buffer_t *buffer, size_t bytes_reqd)
{
    size_t bytes_remaining_packed;

    if (buffer->pack_ptr < buffer->unpack_ptr) {
        return true;
    }

    bytes_remaining_packed = buffer->pack_ptr - buffer->unpack_ptr;

    if (bytes_remaining_packed < bytes_reqd) {
        /* don't error log this - it could be that someone is trying to
         * simply read until the buffer is empty
         */
        return true;
    }

    return false;
}

int ww_bfrop_store_data_type(ww_buffer_t *buffer, ww_data_type_t type)
{
    uint16_t tmp;
    char *dst;

    /* check to see if buffer needs extending */
     if (NULL == (dst = ww_bfrop_buffer_extend(buffer, sizeof(tmp)))) {
        return WW_ERR_OUT_OF_RESOURCE;
    }

    tmp = ww_htons(type);
    memcpy(dst, &tmp, sizeof(tmp));
    buffer->pack_ptr += sizeof(tmp);
    buffer->bytes_used += sizeof(tmp);

    return WW_SUCCESS;
}

int ww_bfrop_get_data_type(ww_buffer_t *buffer, ww_data_type_t *type)
{
    uint16_t tmp;

    /* check to see if there's enough data in buffer */
    if (ww_bfrop_too_small(buffer, sizeof(tmp))) {
        return WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER;
    }

    /* unpack the data */
    memcpy(&tmp, buffer->unpack_ptr, sizeof(tmp));
    tmp = ww_ntohs(tmp);
    memcpy(type, &tmp, sizeof(tmp));
    buffer->unpack_ptr += sizeof(tmp);

    return WW_SUCCESS;
}
