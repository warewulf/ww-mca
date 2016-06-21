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
#include "src/util/output.h"
#include "src/runtime/ww_rte.h"

#include "src/mca/bfrops/base/base.h"

ww_status_t ww_bfrop_base_pack(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer,
                                   const void *src, int num_vals,
                                   ww_data_type_t type)
{
    ww_bfrop_type_info_t *info;
    ww_peer_t *peer = (ww_peer_t*)pr;

    /* check for error */
    if (NULL == buffer || NULL == src ||
        NULL == peer->comm.bfrops) {
        WW_ERROR_LOG(WW_ERR_BAD_PARAM);
        return WW_ERR_BAD_PARAM;
    }

    /* ensure the buffer type is set correctly */
    buffer->type = peer->comm.type;

    /* Lookup the pack function for this type and call it */
    if (NULL == (info = (ww_bfrop_type_info_t*)ww_pointer_array_get_item(peer->comm.bfrops->types, type))) {
        WW_ERROR_LOG(WW_ERR_UNKNOWN_DATA_TYPE);
        return WW_ERR_UNKNOWN_DATA_TYPE;
    }

    return info->odti_pack_fn(pr, buffer, src, num_vals, type);
}

/* PACK FUNCTIONS FOR GENERIC SYSTEM TYPES */

/*
 * BOOL
 */
 ww_status_t ww_bfrop_pack_bool(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, const void *src,
                                    int32_t num_vals, ww_data_type_t type)
 {
    uint8_t *dst;
    int32_t i;
    bool *s = (bool*)src;
    ww_status_t ret;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_pack_bool * %d\n", num_vals);

    /* if this is a described buffer, then pack the datatype */
    if (WW_BFROP_BUFFER_FULLY_DESC == buffer->type) {
        if (WW_SUCCESS != (ret = ww_bfrop_store_data_type(buffer, BFROP_TYPE_INT))) {
            return ret;
        }
    }

    /* check to see if buffer needs extending */
    if (NULL == (dst = (uint8_t*)ww_bfrop_buffer_extend(buffer, num_vals))) {
        return WW_ERR_OUT_OF_RESOURCE;
    }

    /* store the data */
    for (i=0; i < num_vals; i++) {
        if (s[i]) {
            dst[i] = 1;
        } else {
            dst[i] = 0;
        }
    }

    /* update buffer pointers */
    buffer->pack_ptr += num_vals;
    buffer->bytes_used += num_vals;

    return WW_SUCCESS;
}

/*
 * INT
 */
 ww_status_t ww_bfrop_pack_int(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer, const void *src,
                                   int32_t num_vals, ww_data_type_t type)
 {
    ww_status_t ret;
    ww_peer_t *peer = (ww_peer_t*)pr;

    /* System types need to always be described so we can properly
       unpack them */
    if (WW_SUCCESS != (ret = ww_bfrop_store_data_type(buffer, BFROP_TYPE_INT))) {
        return ret;
    }

    /* Turn around and pack the real type */
    return peer->comm.bfrops->pack(pr, buffer, src, num_vals, BFROP_TYPE_INT);
}

/*
 * SIZE_T
 */
 ww_status_t ww_bfrop_pack_sizet(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, const void *src,
                                     int32_t num_vals, ww_data_type_t type)
 {
    int ret;
    ww_peer_t *peer = (ww_peer_t*)pr;

    /* System types need to always be described so we can properly
       unpack them. */
    if (WW_SUCCESS != (ret = ww_bfrop_store_data_type(buffer, BFROP_TYPE_SIZE_T))) {
        return ret;
    }

    return peer->comm.bfrops->pack(pr, buffer, src, num_vals, BFROP_TYPE_SIZE_T);
}

/*
 * PID_T
 */
 ww_status_t ww_bfrop_pack_pid(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer, const void *src,
                                   int32_t num_vals, ww_data_type_t type)
 {
    int ret;
    ww_peer_t *peer = (ww_peer_t*)pr;

    /* System types need to always be described so we can properly
       unpack them. */
    if (WW_SUCCESS != (ret = ww_bfrop_store_data_type(buffer, BFROP_TYPE_PID_T))) {
        return ret;
    }

    /* Turn around and pack the real type */
    return peer->comm.bfrops->pack(pr, buffer, src, num_vals, BFROP_TYPE_PID_T);
}


/* PACK FUNCTIONS FOR NON-GENERIC SYSTEM TYPES */

/*
 * BYTE, CHAR, INT8
 */
 ww_status_t ww_bfrop_pack_byte(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, const void *src,
                                    int32_t num_vals, ww_data_type_t type)
 {
    char *dst;
    ww_status_t ret;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_pack_byte * %d\n", num_vals);

    /* if this is a described buffer, then pack the datatype */
    if (WW_BFROP_BUFFER_FULLY_DESC == buffer->type) {
        if (WW_SUCCESS != (ret = ww_bfrop_store_data_type(buffer, WW_BYTE))) {
            return ret;
        }
    }

    /* check to see if buffer needs extending */
    if (NULL == (dst = ww_bfrop_buffer_extend(buffer, num_vals))) {
        return WW_ERR_OUT_OF_RESOURCE;
    }

    /* store the data */
    memcpy(dst, src, num_vals);

    /* update buffer pointers */
    buffer->pack_ptr += num_vals;
    buffer->bytes_used += num_vals;

    return WW_SUCCESS;
}

/*
 * INT16
 */
 ww_status_t ww_bfrop_pack_int16(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, const void *src,
                                     int32_t num_vals, ww_data_type_t type)
 {
    int32_t i;
    uint16_t tmp, *srctmp = (uint16_t*) src;
    char *dst;
    ww_status_t ret;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_pack_int16 * %d\n", num_vals);

    /* if this is a described buffer, then pack the datatype */
    if (WW_BFROP_BUFFER_FULLY_DESC == buffer->type) {
        if (WW_SUCCESS != (ret = ww_bfrop_store_data_type(buffer, WW_INT16))) {
            return ret;
        }
    }

    /* check to see if buffer needs extending */
    if (NULL == (dst = ww_bfrop_buffer_extend(buffer, num_vals*sizeof(tmp)))) {
        return WW_ERR_OUT_OF_RESOURCE;
    }

    for (i = 0; i < num_vals; ++i) {
        tmp = ww_htons(srctmp[i]);
        memcpy(dst, &tmp, sizeof(tmp));
        dst += sizeof(tmp);
    }
    buffer->pack_ptr += num_vals * sizeof(tmp);
    buffer->bytes_used += num_vals * sizeof(tmp);

    return WW_SUCCESS;
}

/*
 * INT32
 */
 ww_status_t ww_bfrop_pack_int32(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, const void *src,
                                     int32_t num_vals, ww_data_type_t type)
 {
    int32_t i;
    uint32_t tmp, *srctmp = (uint32_t*) src;
    char *dst;
    ww_status_t ret;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_pack_int32 * %d\n", num_vals);

    /* if this is a described buffer, then pack the datatype */
    if (WW_BFROP_BUFFER_FULLY_DESC == buffer->type) {
        if (WW_SUCCESS != (ret = ww_bfrop_store_data_type(buffer, WW_INT32))) {
            return ret;
        }
    }

    /* check to see if buffer needs extending */
    if (NULL == (dst = ww_bfrop_buffer_extend(buffer, num_vals*sizeof(tmp)))) {
        return WW_ERR_OUT_OF_RESOURCE;
    }

    for (i = 0; i < num_vals; ++i) {
        tmp = htonl(srctmp[i]);
        memcpy(dst, &tmp, sizeof(tmp));
        dst += sizeof(tmp);
    }
    buffer->pack_ptr += num_vals * sizeof(tmp);
    buffer->bytes_used += num_vals * sizeof(tmp);

    return WW_SUCCESS;
}

/*
 * INT64
 */
 ww_status_t ww_bfrop_pack_int64(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, const void *src,
                                     int32_t num_vals, ww_data_type_t type)
 {
    int32_t i;
    uint64_t tmp, tmp2;
    char *dst;
    size_t bytes_packed = num_vals * sizeof(tmp);
    ww_status_t ret;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_pack_int64 * %d\n", num_vals);

    /* if this is a described buffer, then pack the datatype */
    if (WW_BFROP_BUFFER_FULLY_DESC == buffer->type) {
        if (WW_SUCCESS != (ret = ww_bfrop_store_data_type(buffer, WW_INT64))) {
            return ret;
        }
    }

    /* check to see if buffer needs extending */
    if (NULL == (dst = ww_bfrop_buffer_extend(buffer, bytes_packed))) {
        return WW_ERR_OUT_OF_RESOURCE;
    }

    for (i = 0; i < num_vals; ++i) {
        memcpy(&tmp2, (char *)src+i*sizeof(uint64_t), sizeof(uint64_t));
        tmp = ww_hton64(tmp2);
        memcpy(dst, &tmp, sizeof(tmp));
        dst += sizeof(tmp);
    }
    buffer->pack_ptr += bytes_packed;
    buffer->bytes_used += bytes_packed;

    return WW_SUCCESS;
}

/*
 * STRING
 */
 ww_status_t ww_bfrop_pack_string(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, const void *src,
                                      int32_t num_vals, ww_data_type_t type)
 {
    int ret = WW_SUCCESS;
    int32_t i, len;
    char **ssrc = (char**) src;

    for (i = 0; i < num_vals; ++i) {
        if (NULL == ssrc[i]) {  /* got zero-length string/NULL pointer - store NULL */
            len = 0;
            if (WW_SUCCESS != (ret = ww_bfrop_pack_int32(pr, buffer, &len, 1, WW_INT32))) {
                return ret;
            }
        } else {
            len = (int32_t)strlen(ssrc[i]) + 1;
            if (WW_SUCCESS != (ret = ww_bfrop_pack_int32(pr, buffer, &len, 1, WW_INT32))) {
                return ret;
            }
            if (WW_SUCCESS != (ret =
                ww_bfrop_pack_byte(pr, buffer, ssrc[i], len, WW_BYTE))) {
                return ret;
            }
        }
    }
    return ret;
}

/* FLOAT */
ww_status_t ww_bfrop_pack_float(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, const void *src,
                                    int32_t num_vals, ww_data_type_t type)
{
    int ret = WW_SUCCESS;
    int32_t i;
    float *ssrc = (float*)src;
    char *convert;

    for (i = 0; i < num_vals; ++i) {
        asprintf(&convert, "%f", ssrc[i]);
        if (WW_SUCCESS != (ret = ww_bfrop_pack_string(pr, buffer, &convert, 1, WW_STRING))) {
            free(convert);
            return ret;
        }
        free(convert);
    }

    return WW_SUCCESS;
}

/* DOUBLE */
ww_status_t ww_bfrop_pack_double(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, const void *src,
                                     int32_t num_vals, ww_data_type_t type)
{
    int ret = WW_SUCCESS;
    int32_t i;
    double *ssrc = (double*)src;
    char *convert;

    for (i = 0; i < num_vals; ++i) {
        asprintf(&convert, "%f", ssrc[i]);
        if (WW_SUCCESS != (ret = ww_bfrop_pack_string(pr, buffer, &convert, 1, WW_STRING))) {
            free(convert);
            return ret;
        }
        free(convert);
    }

    return WW_SUCCESS;
}

/* TIMEVAL */
ww_status_t ww_bfrop_pack_timeval(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, const void *src,
                                      int32_t num_vals, ww_data_type_t type)
{
    int64_t tmp[2];
    int ret = WW_SUCCESS;
    int32_t i;
    struct timeval *ssrc = (struct timeval *)src;

    for (i = 0; i < num_vals; ++i) {
        tmp[0] = (int64_t)ssrc[i].tv_sec;
        tmp[1] = (int64_t)ssrc[i].tv_usec;
        if (WW_SUCCESS != (ret = ww_bfrop_pack_int64(pr, buffer, tmp, 2, WW_INT64))) {
            return ret;
        }
    }

    return WW_SUCCESS;
}

/* TIME */
ww_status_t ww_bfrop_pack_time(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer, const void *src,
                                   int32_t num_vals, ww_data_type_t type)
{
    int ret = WW_SUCCESS;
    int32_t i;
    time_t *ssrc = (time_t *)src;
    uint64_t ui64;

    /* time_t is a system-dependent size, so cast it
     * to uint64_t as a generic safe size
     */
     for (i = 0; i < num_vals; ++i) {
        ui64 = (uint64_t)ssrc[i];
        if (WW_SUCCESS != (ret = ww_bfrop_pack_int64(pr, buffer, &ui64, 1, WW_UINT64))) {
            return ret;
        }
    }

    return WW_SUCCESS;
}

/* STATUS */
ww_status_t ww_bfrop_pack_status(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, const void *src,
                                     int32_t num_vals, ww_data_type_t type)
{
    int ret = WW_SUCCESS;
    int32_t i;
    ww_status_t *ssrc = (ww_status_t *)src;
    int32_t status;

    for (i = 0; i < num_vals; ++i) {
        status = (int32_t)ssrc[i];
        if (WW_SUCCESS != (ret = ww_bfrop_pack_int32(pr, buffer, &status, 1, WW_INT32))) {
            return ret;
        }
    }

    return WW_SUCCESS;
}


/* PACK FUNCTIONS FOR GENERIC WW TYPES */
static int pack_val(struct ww_peer_t *pr,
                    ww_buffer_t *buffer,
                    ww_value_t *p)
{
    int ret;
    ww_peer_t *peer = (ww_peer_t*)pr;
    ww_bfrops_module_t *bmod = (ww_bfrops_module_t*)peer->comm.bfrops;

    switch (p->type) {
        case WW_BOOL:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.flag, 1, WW_BOOL))) {
            return ret;
        }
        break;
        case WW_BYTE:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.byte, 1, WW_BYTE))) {
            return ret;
        }
        break;
        case WW_STRING:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.string, 1, WW_STRING))) {
            return ret;
        }
        break;
        case WW_SIZE:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.size, 1, WW_SIZE))) {
            return ret;
        }
        break;
        case WW_PID:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.pid, 1, WW_PID))) {
            return ret;
        }
        break;
        case WW_INT:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.integer, 1, WW_INT))) {
            return ret;
        }
        break;
        case WW_INT8:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.int8, 1, WW_INT8))) {
            return ret;
        }
        break;
        case WW_INT16:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.int16, 1, WW_INT16))) {
            return ret;
        }
        break;
        case WW_INT32:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.int32, 1, WW_INT32))) {
            return ret;
        }
        break;
        case WW_INT64:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.int64, 1, WW_INT64))) {
            return ret;
        }
        break;
        case WW_UINT:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.uint, 1, WW_UINT))) {
            return ret;
        }
        break;
        case WW_UINT8:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.uint8, 1, WW_UINT8))) {
            return ret;
        }
        break;
        case WW_UINT16:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.uint16, 1, WW_UINT16))) {
            return ret;
        }
        break;
        case WW_UINT32:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.uint32, 1, WW_UINT32))) {
            return ret;
        }
        break;
        case WW_UINT64:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.uint64, 1, WW_UINT64))) {
            return ret;
        }
        break;
        case WW_FLOAT:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.fval, 1, WW_FLOAT))) {
            return ret;
        }
        break;
        case WW_DOUBLE:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.dval, 1, WW_DOUBLE))) {
            return ret;
        }
        break;
        case WW_TIMEVAL:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.tv, 1, WW_TIMEVAL))) {
            return ret;
        }
        break;
        case WW_STATUS:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.status, 1, WW_STATUS))) {
            return ret;
        }
        break;
        case WW_INFO_ARRAY:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.array, 1, WW_INFO_ARRAY))) {
            return ret;
        }
        break;
        case WW_BYTE_OBJECT:
        if (WW_SUCCESS != (ret = bmod->pack(pr, buffer, &p->data.bo, 1, WW_BYTE_OBJECT))) {
            return ret;
        }
        break;
        default:
        ww_output(0, "PACK-WW-VALUE: UNSUPPORTED TYPE %d", (int)p->type);
        return WW_ERROR;
    }
    return WW_SUCCESS;
}

/*
 * WW_VALUE
 */
 int ww_bfrop_pack_value(struct ww_peer_t *pr,
                           ww_buffer_t *buffer, const void *src,
                           int32_t num_vals, ww_data_type_t type)
 {
    ww_value_t *ptr;
    int32_t i;
    int ret;

    ptr = (ww_value_t *) src;

    for (i = 0; i < num_vals; ++i) {
        /* pack the type */
        if (WW_SUCCESS != (ret = ww_bfrop_pack_int(pr, buffer, &ptr[i].type, 1, WW_INT))) {
            return ret;
        }
        /* now pack the right field */
        if (WW_SUCCESS != (ret = pack_val(pr, buffer, &ptr[i]))) {
            return ret;
        }
    }

    return WW_SUCCESS;
}


int ww_bfrop_pack_info(struct ww_peer_t *pr,
                         ww_buffer_t *buffer, const void *src,
                         int32_t num_vals, ww_data_type_t type)
{
    ww_info_t *info;
    int32_t i;
    int ret;
    char *foo;

    info = (ww_info_t *) src;

    for (i = 0; i < num_vals; ++i) {
        /* pack key */
        foo = info[i].key;
        if (WW_SUCCESS != (ret = ww_bfrop_pack_string(pr, buffer, &foo, 1, WW_STRING))) {
            return ret;
        }
        /* pack required flag */
        if (WW_SUCCESS != (ret = ww_bfrop_pack_bool(pr, buffer, &info[i].required, 1, WW_BOOL))) {
            return ret;
        }
        /* pack the type */
        if (WW_SUCCESS != (ret = ww_bfrop_pack_int(pr, buffer, &info[i].value.type, 1, WW_INT))) {
            return ret;
        }
        /* pack value */
        if (WW_SUCCESS != (ret = pack_val(pr, buffer, &info[i].value))) {
            return ret;
        }
    }
    return WW_SUCCESS;
}

int ww_bfrop_pack_buf(struct ww_peer_t *pr,
                        ww_buffer_t *buffer, const void *src,
                        int32_t num_vals, ww_data_type_t type)
{
    ww_buffer_t **ptr;
    int32_t i;
    int ret;

    ptr = (ww_buffer_t **) src;

    for (i = 0; i < num_vals; ++i) {
        /* pack the number of bytes */
        if (WW_SUCCESS != (ret = ww_bfrop_pack_sizet(pr, buffer, &ptr[i]->bytes_used, 1, WW_SIZE))) {
            return ret;
        }
        /* pack the bytes */
        if (0 < ptr[i]->bytes_used) {
            if (WW_SUCCESS != (ret = ww_bfrop_pack_byte(pr, buffer, ptr[i]->base_ptr, ptr[i]->bytes_used, WW_BYTE))) {
                return ret;
            }
        }
    }
    return WW_SUCCESS;
}

int ww_bfrop_pack_proc(struct ww_peer_t *pr,
                         ww_buffer_t *buffer, const void *src,
                         int32_t num_vals, ww_data_type_t type)
{
    ww_proc_t *proc;
    int32_t i;
    int ret;

    proc = (ww_proc_t *) src;

    for (i = 0; i < num_vals; ++i) {
        char *ptr = proc[i].nspace;
        if (WW_SUCCESS != (ret = ww_bfrop_pack_string(pr, buffer, &ptr, 1, WW_STRING))) {
            return ret;
        }
        if (WW_SUCCESS != (ret = ww_bfrop_pack_int(pr, buffer, &proc[i].rank, 1, WW_INT))) {
            return ret;
        }
    }
    return WW_SUCCESS;
}

int ww_bfrop_pack_kval(struct ww_peer_t *pr,
                         ww_buffer_t *buffer, const void *src,
                         int32_t num_vals, ww_data_type_t type)
{
    ww_kval_t *ptr;
    int32_t i;
    int ret;

    ptr = (ww_kval_t *) src;

    for (i = 0; i < num_vals; ++i) {
        /* pack the key */
        if (WW_SUCCESS != (ret = ww_bfrop_pack_string(pr, buffer, &ptr[i].key, 1, WW_STRING))) {
            return ret;
        }
        /* pack the value */
        if (WW_SUCCESS != (ret = ww_bfrop_pack_value(pr, buffer, ptr[i].value, 1, WW_INT))) {
            return ret;
        }
    }

    return WW_SUCCESS;
}

int ww_bfrop_pack_array(struct ww_peer_t *pr,
                          ww_buffer_t *buffer, const void *src,
                          int32_t num_vals, ww_data_type_t type)
{
    ww_info_array_t *ptr;
    int32_t i;
    int ret;

    ptr = (ww_info_array_t *) src;

    for (i = 0; i < num_vals; ++i) {
        /* pack the size */
        if (WW_SUCCESS != (ret = ww_bfrop_pack_sizet(pr, buffer, &ptr[i].size, 1, WW_SIZE))) {
            return ret;
        }
        if (0 < ptr[i].size) {
            /* pack the values */
            if (WW_SUCCESS != (ret = ww_bfrop_pack_info(pr, buffer, ptr[i].array, ptr[i].size, WW_INFO))) {
                return ret;
            }
        }
    }

    return WW_SUCCESS;
}

int ww_bfrop_pack_bo(struct ww_peer_t *pr,
                       ww_buffer_t *buffer, const void *src,
                       int32_t num_vals, ww_data_type_t type)
{
    int ret;
    int i;
    ww_byte_object_t *bo;

    bo = (ww_byte_object_t*)src;
    for (i=0; i < num_vals; i++) {
        if (WW_SUCCESS != (ret = ww_bfrop_pack_sizet(pr, buffer, &bo[i].size, 1, WW_SIZE))) {
            return ret;
        }
        if (0 < bo[i].size) {
            if (WW_SUCCESS != (ret = ww_bfrop_pack_byte(pr, buffer, bo[i].bytes, bo[i].size, WW_BYTE))) {
                return ret;
            }
        }
    }
    return WW_SUCCESS;
}

int ww_bfrop_pack_ptr(struct ww_peer_t *pr,
                        ww_buffer_t *buffer, const void *src,
                        int32_t num_vals, ww_data_type_t type)
{
    uint8_t foo=1;
    /* it obviously makes no sense to pack a pointer and
     * send it somewhere else, so we just pack a sentinel */
    return ww_bfrop_pack_byte(pr, buffer, &foo, 1, WW_UINT8);
}

