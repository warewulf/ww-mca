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
 * Copyright (c) 2012      Los Alamos National Security, Inc.  All rights reserved.
 * Copyright (c) 2014-2016 Intel, Inc. All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * Copyright (c) 2016      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>

#include <ww_types.h>

#include "src/util/argv.h"
#include "src/util/error.h"
#include "src/util/output.h"
#include "src/runtime/ww_rte.h"
#include "src/mca/bfrops/bfrop_types.h"
#include "src/mca/bfrops/base/base.h"

ww_status_t ww_bfrop_base_unpack(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer,
                                     void *dst, int32_t *num_vals,
                                     ww_data_type_t type)
{
    ww_status_t rc, ret;
    int32_t local_num, n=1;
    ww_data_type_t local_type;
    ww_peer_t *peer = (ww_peer_t*)pr;

    /* check for error */
    if (NULL == buffer || NULL == dst ||
        NULL == num_vals || NULL == peer->comm.bfrops) {
        return WW_ERR_BAD_PARAM;
    }
    /* ensure the buffer type is set correctly */
    buffer->type = peer->comm.type;

    /* if user provides a zero for num_vals, then there is no storage allocated
     * so return an appropriate error
     */
     if (0 == *num_vals) {
        ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack: inadequate space ( %p, %p, %lu, %d )\n",
                            (void*)buffer, dst, (long unsigned int)*num_vals, (int)type);
        return WW_ERR_UNPACK_INADEQUATE_SPACE;
    }

    /** Unpack the declared number of values
     * REMINDER: it is possible that the buffer is corrupted and that
     * the BFROP will *think* there is a proper int32_t variable at the
     * beginning of the unpack region - but that the value is bogus (e.g., just
     * a byte field in a string array that so happens to have a value that
     * matches the int32_t data type flag). Therefore, this error check is
     * NOT completely safe. This is true for ALL unpack functions, not just
     * int32_t as used here.
     */
     if (WW_BFROP_BUFFER_FULLY_DESC == buffer->type) {
        if (WW_SUCCESS != (rc = ww_bfrop_get_data_type(buffer, &local_type))) {
            *num_vals = 0;
            /* don't error log here as the user may be unpacking past
             * the end of the buffer, which isn't necessarily an error */
            return rc;
        }
        if (WW_INT32 != local_type) { /* if the length wasn't first, then error */
            *num_vals = 0;
            return WW_ERR_UNPACK_FAILURE;
        }
    }

    n=1;
    if (WW_SUCCESS != (rc = ww_bfrop_unpack_int32(pr, buffer, &local_num, &n, WW_INT32))) {
        *num_vals = 0;
            /* don't error log here as the user may be unpacking past
             * the end of the buffer, which isn't necessarily an error */
        return rc;
    }

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack: found %d values for %d provided storage",
                        local_num, *num_vals);

    /** if the storage provided is inadequate, set things up
     * to unpack as much as we can and to return an error code
     * indicating that everything was not unpacked - the buffer
     * is left in a state where it can not be further unpacked.
     */
     if (local_num > *num_vals) {
        local_num = *num_vals;
        ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack: inadequate space ( %p, %p, %lu, %d )\n",
                            (void*)buffer, dst, (long unsigned int)*num_vals, (int)type);
        ret = WW_ERR_UNPACK_INADEQUATE_SPACE;
    } else {  /** enough or more than enough storage */
        *num_vals = local_num;  /** let the user know how many we actually unpacked */
        ret = WW_SUCCESS;
    }

    /** Unpack the value(s) */
    if (WW_SUCCESS != (rc = ww_bfrop_unpack_buffer(pr, buffer, dst, &local_num, type))) {
        *num_vals = 0;
        ret = rc;
    }

    return ret;
}

ww_status_t ww_bfrop_unpack_buffer(struct ww_peer_t *pr,
                                       ww_buffer_t *buffer,
                                       void *dst, int32_t *num_vals,
                                       ww_data_type_t type)
{
    ww_status_t rc;
    ww_data_type_t local_type;
    ww_bfrop_type_info_t *info;
    ww_peer_t *peer = (ww_peer_t*)pr;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack_buffer( %p, %p, %lu, %d )\n",
                        (void*)buffer, dst, (long unsigned int)*num_vals, (int)type);

    /** Unpack the declared data type */
    if (WW_BFROP_BUFFER_FULLY_DESC == buffer->type) {
        if (WW_SUCCESS != (rc = ww_bfrop_get_data_type(buffer, &local_type))) {
            return rc;
        }
        /* if the data types don't match, then return an error */
        if (type != local_type) {
            ww_output(0, "WW bfrop:unpack: got type %d when expecting type %d", local_type, type);
            return WW_ERR_PACK_MISMATCH;
        }
    }

    /* Lookup the unpack function for this type and call it */

    if (NULL == (info = (ww_bfrop_type_info_t*)ww_pointer_array_get_item(peer->comm.bfrops->types, type))) {
        return WW_ERR_UNPACK_FAILURE;
    }

    return info->odti_unpack_fn(pr, buffer, dst, num_vals, type);
}


/* UNPACK GENERIC SYSTEM TYPES */

/*
 * BOOL
 */
 ww_status_t ww_bfrop_unpack_bool(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type)
 {
    int32_t i;
    uint8_t *src;
    bool *dst;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack_bool * %d\n", (int)*num_vals);
    /* check to see if there's enough data in buffer */
    if (ww_bfrop_too_small(buffer, *num_vals)) {
        return WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER;
    }

    /* unpack the data */
    src = (uint8_t*)buffer->unpack_ptr;
    dst = (bool*)dest;

    for (i=0; i < *num_vals; i++) {
        if (src[i]) {
            dst[i] = true;
        } else {
            dst[i] = false;
        }
    }

    /* update buffer pointer */
    buffer->unpack_ptr += *num_vals;

    return WW_SUCCESS;
}

/*
 * INT
 */
 ww_status_t ww_bfrop_unpack_int(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type)
 {
    ww_status_t ret;
    ww_data_type_t remote_type;

    if (WW_SUCCESS != (ret = ww_bfrop_get_data_type(buffer, &remote_type))) {
        return ret;
    }

    if (remote_type == BFROP_TYPE_INT) {
        /* fast path it if the sizes are the same */
        /* Turn around and unpack the real type */
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, dest, num_vals, BFROP_TYPE_INT))) {
        }
    } else {
        /* slow path - types are different sizes */
        UNPACK_SIZE_MISMATCH(int, remote_type, ret);
    }

    return ret;
}

/*
 * SIZE_T
 */
 ww_status_t ww_bfrop_unpack_sizet(struct ww_peer_t *pr,
                                       ww_buffer_t *buffer, void *dest,
                                       int32_t *num_vals, ww_data_type_t type)
 {
    ww_status_t ret;
    ww_data_type_t remote_type;

    if (WW_SUCCESS != (ret = ww_bfrop_get_data_type(buffer, &remote_type))) {
        return ret;
    }

    if (remote_type == BFROP_TYPE_SIZE_T) {
        /* fast path it if the sizes are the same */
        /* Turn around and unpack the real type */
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, dest, num_vals, BFROP_TYPE_SIZE_T))) {
        }
    } else {
        /* slow path - types are different sizes */
        UNPACK_SIZE_MISMATCH(size_t, remote_type, ret);
    }

    return ret;
}

/*
 * PID_T
 */
 ww_status_t ww_bfrop_unpack_pid(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type)
 {
    ww_status_t ret;
    ww_data_type_t remote_type;

    if (WW_SUCCESS != (ret = ww_bfrop_get_data_type(buffer, &remote_type))) {
        return ret;
    }

    if (remote_type == BFROP_TYPE_PID_T) {
        /* fast path it if the sizes are the same */
        /* Turn around and unpack the real type */
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, dest, num_vals, BFROP_TYPE_PID_T))) {
        }
    } else {
        /* slow path - types are different sizes */
        UNPACK_SIZE_MISMATCH(pid_t, remote_type, ret);
    }

    return ret;
}


/* UNPACK FUNCTIONS FOR NON-GENERIC SYSTEM TYPES */

/*
 * BYTE, CHAR, INT8
 */
 ww_status_t ww_bfrop_unpack_byte(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type)
 {
    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack_byte * %d\n", (int)*num_vals);
    /* check to see if there's enough data in buffer */
    if (ww_bfrop_too_small(buffer, *num_vals)) {
        return WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER;
    }

    /* unpack the data */
    memcpy(dest, buffer->unpack_ptr, *num_vals);

    /* update buffer pointer */
    buffer->unpack_ptr += *num_vals;

    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_int16(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type)
{
    int32_t i;
    uint16_t tmp, *desttmp = (uint16_t*) dest;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack_int16 * %d\n", (int)*num_vals);
    /* check to see if there's enough data in buffer */
    if (ww_bfrop_too_small(buffer, (*num_vals)*sizeof(tmp))) {
        return WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER;
    }

    /* unpack the data */
    for (i = 0; i < (*num_vals); ++i) {
        memcpy( &(tmp), buffer->unpack_ptr, sizeof(tmp) );
        tmp = ww_ntohs(tmp);
        memcpy(&desttmp[i], &tmp, sizeof(tmp));
        buffer->unpack_ptr += sizeof(tmp);
    }

    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_int32(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type)
{
    int32_t i;
    uint32_t tmp, *desttmp = (uint32_t*) dest;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack_int32 * %d\n", (int)*num_vals);
    /* check to see if there's enough data in buffer */
    if (ww_bfrop_too_small(buffer, (*num_vals)*sizeof(tmp))) {
        return WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER;
    }

    /* unpack the data */
    for (i = 0; i < (*num_vals); ++i) {
        memcpy( &(tmp), buffer->unpack_ptr, sizeof(tmp) );
        tmp = ntohl(tmp);
        memcpy(&desttmp[i], &tmp, sizeof(tmp));
        buffer->unpack_ptr += sizeof(tmp);
    }

    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_datatype(struct ww_peer_t *pr,
                                         ww_buffer_t *buffer, void *dest,
                                         int32_t *num_vals, ww_data_type_t type)
{
    return ww_bfrop_unpack_int16(pr, buffer, dest, num_vals, type);
}

ww_status_t ww_bfrop_unpack_int64(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type)
{
    int32_t i;
    uint64_t tmp, *desttmp = (uint64_t*) dest;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack_int64 * %d\n", (int)*num_vals);
    /* check to see if there's enough data in buffer */
    if (ww_bfrop_too_small(buffer, (*num_vals)*sizeof(tmp))) {
        return WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER;
    }

    /* unpack the data */
    for (i = 0; i < (*num_vals); ++i) {
        memcpy( &(tmp), buffer->unpack_ptr, sizeof(tmp) );
        tmp = ww_ntoh64(tmp);
        memcpy(&desttmp[i], &tmp, sizeof(tmp));
        buffer->unpack_ptr += sizeof(tmp);
    }

    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_string(struct ww_peer_t *pr,
                                       ww_buffer_t *buffer, void *dest,
                                       int32_t *num_vals, ww_data_type_t type)
{
    ww_status_t ret;
    int32_t i, len, n=1;
    char **sdest = (char**) dest;

    for (i = 0; i < (*num_vals); ++i) {
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_int32(pr, buffer, &len, &n, WW_INT32))) {
            return ret;
        }
        if (0 ==  len) {   /* zero-length string - unpack the NULL */
        sdest[i] = NULL;
    } else {
        sdest[i] = (char*)malloc(len);
        if (NULL == sdest[i]) {
            return WW_ERR_OUT_OF_RESOURCE;
        }
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_byte(pr, buffer, sdest[i], &len, WW_BYTE))) {
            return ret;
        }
    }
}

return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_float(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type)
{
    int32_t i, n;
    float *desttmp = (float*) dest, tmp;
    ww_status_t ret;
    char *convert;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack_float * %d\n", (int)*num_vals);
    /* check to see if there's enough data in buffer */
    if (ww_bfrop_too_small(buffer, (*num_vals)*sizeof(float))) {
        return WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER;
    }

    /* unpack the data */
    for (i = 0; i < (*num_vals); ++i) {
        n=1;
        convert = NULL;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_string(pr, buffer, &convert, &n, WW_STRING))) {
            return ret;
        }
        if (NULL != convert) {
            tmp = strtof(convert, NULL);
            memcpy(&desttmp[i], &tmp, sizeof(tmp));
            free(convert);
        }
    }
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_double(struct ww_peer_t *pr,
                                       ww_buffer_t *buffer, void *dest,
                                       int32_t *num_vals, ww_data_type_t type)
{
    int32_t i, n;
    double *desttmp = (double*) dest, tmp;
    ww_status_t ret;
    char *convert;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack_double * %d\n", (int)*num_vals);
    /* check to see if there's enough data in buffer */
    if (ww_bfrop_too_small(buffer, (*num_vals)*sizeof(double))) {
        return WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER;
    }

    /* unpack the data */
    for (i = 0; i < (*num_vals); ++i) {
        n=1;
        convert = NULL;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_string(pr, buffer, &convert, &n, WW_STRING))) {
            return ret;
        }
        if (NULL != convert) {
            tmp = strtod(convert, NULL);
            memcpy(&desttmp[i], &tmp, sizeof(tmp));
            free(convert);
        }
    }
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_timeval(struct ww_peer_t *pr,
                                        ww_buffer_t *buffer, void *dest,
                                        int32_t *num_vals, ww_data_type_t type)
{
    int32_t i, n;
    int64_t tmp[2];
    struct timeval *desttmp = (struct timeval *) dest, tt;
    ww_status_t ret;

    ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack_timeval * %d\n", (int)*num_vals);
    /* check to see if there's enough data in buffer */
    if (ww_bfrop_too_small(buffer, (*num_vals)*sizeof(struct timeval))) {
        return WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER;
    }

    /* unpack the data */
    for (i = 0; i < (*num_vals); ++i) {
        n=2;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_int64(pr, buffer, tmp, &n, WW_INT64))) {
            return ret;
        }
        tt.tv_sec = tmp[0];
        tt.tv_usec = tmp[1];
        memcpy(&desttmp[i], &tt, sizeof(tt));
    }
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_time(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type)
{
    int32_t i, n;
    time_t *desttmp = (time_t *) dest, tmp;
    ww_status_t ret;
    uint64_t ui64;

    /* time_t is a system-dependent size, so cast it
     * to uint64_t as a generic safe size
     */

     ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack_time * %d\n", (int)*num_vals);
    /* check to see if there's enough data in buffer */
     if (ww_bfrop_too_small(buffer, (*num_vals)*(sizeof(uint64_t)))) {
        return WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER;
    }

    /* unpack the data */
    for (i = 0; i < (*num_vals); ++i) {
        n=1;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_int64(pr, buffer, &ui64, &n, WW_UINT64))) {
            return ret;
        }
        tmp = (time_t)ui64;
        memcpy(&desttmp[i], &tmp, sizeof(tmp));
    }
    return WW_SUCCESS;
}


ww_status_t ww_bfrop_unpack_status(struct ww_peer_t *pr,
                                       ww_buffer_t *buffer, void *dest,
                                       int32_t *num_vals, ww_data_type_t type)
{
     ww_output_verbose(20, ww_globals.debug_output, "ww_bfrop_unpack_status * %d\n", (int)*num_vals);
    /* check to see if there's enough data in buffer */
    if (ww_bfrop_too_small(buffer, (*num_vals)*(sizeof(ww_status_t)))) {
        return WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER;
    }

    /* unpack the data */
    return ww_bfrop_unpack_int32(pr, buffer, dest, num_vals, WW_INT32);
}


/* UNPACK FUNCTIONS FOR GENERIC WW TYPES */

/*
 * WW_VALUE
 */
 static ww_status_t unpack_val(struct ww_peer_t *pr,
                                 ww_buffer_t *buffer,
                                 ww_value_t *val)
 {
    int m;
    ww_status_t ret;

    m = 1;
    switch (val->type) {
        case WW_BOOL:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.flag, &m, WW_BOOL))) {
            return ret;
        }
        break;
        case WW_BYTE:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.byte, &m, WW_BYTE))) {
            return ret;
        }
        break;
        case WW_STRING:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.string, &m, WW_STRING))) {
            return ret;
        }
        break;
        case WW_SIZE:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.size, &m, WW_SIZE))) {
            return ret;
        }
        break;
        case WW_PID:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.pid, &m, WW_PID))) {
            return ret;
        }
        break;
        case WW_INT:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.integer, &m, WW_INT))) {
            return ret;
        }
        break;
        case WW_INT8:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.int8, &m, WW_INT8))) {
            return ret;
        }
        break;
        case WW_INT16:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.int16, &m, WW_INT16))) {
            return ret;
        }
        break;
        case WW_INT32:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.int32, &m, WW_INT32))) {
            return ret;
        }
        break;
        case WW_INT64:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.int64, &m, WW_INT64))) {
            return ret;
        }
        break;
        case WW_UINT:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.uint, &m, WW_UINT))) {
            return ret;
        }
        break;
        case WW_UINT8:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.uint8, &m, WW_UINT8))) {
            return ret;
        }
        break;
        case WW_UINT16:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.uint16, &m, WW_UINT16))) {
            return ret;
        }
        break;
        case WW_UINT32:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.uint32, &m, WW_UINT32))) {
            return ret;
        }
        break;
        case WW_UINT64:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.uint64, &m, WW_UINT64))) {
            return ret;
        }
        break;
        case WW_FLOAT:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.fval, &m, WW_FLOAT))) {
            return ret;
        }
        break;
        case WW_DOUBLE:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.dval, &m, WW_DOUBLE))) {
            return ret;
        }
        break;
        case WW_TIMEVAL:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.tv, &m, WW_TIMEVAL))) {
            return ret;
        }
        break;
        case WW_STATUS:
         if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.status, &m, WW_STATUS))) {
             return ret;
         }
         break;
        case WW_INFO_ARRAY:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.array, &m, WW_INFO_ARRAY))) {
            return ret;
        }
        break;
        case WW_BYTE_OBJECT:
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_buffer(pr, buffer, &val->data.bo, &m, WW_BYTE_OBJECT))) {
            return ret;
        }
        break;
        default:
        ww_output(0, "UNPACK-WW-VALUE: UNSUPPORTED TYPE");
        return WW_ERROR;
    }

    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_value(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type)
{
    ww_value_t *ptr;
    int32_t i, m, n;
    ww_status_t ret;

    ptr = (ww_value_t *) dest;
    n = *num_vals;

    for (i = 0; i < n; ++i) {
        /* unpack the type */
        m=1;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_int(pr, buffer, &ptr[i].type, &m, WW_INT))) {
            return ret;
        }
        /* unpack value */
        if (WW_SUCCESS != (ret = unpack_val(pr, buffer, &ptr[i])) ) {
            return ret;
        }
    }
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_info(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type)
{
    ww_info_t *ptr;
    int32_t i, n, m;
    ww_status_t ret;
    char *tmp;

    ww_output_verbose(20, ww_globals.debug_output,
                        "ww_bfrop_unpack: %d info", *num_vals);

    ptr = (ww_info_t *) dest;
    n = *num_vals;

    for (i = 0; i < n; ++i) {
        memset(ptr[i].key, 0, sizeof(ptr[i].key));
        memset(&ptr[i].value, 0, sizeof(ww_value_t));
        /* unpack key */
        m=1;
        tmp = NULL;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_string(pr, buffer, &tmp, &m, WW_STRING))) {
            return ret;
        }
        if (NULL == tmp) {
            return WW_ERROR;
        }
        (void)strncpy(ptr[i].key, tmp, WW_MAX_KEYLEN);
        free(tmp);
        /* unpack the required flag */
        m=1;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_bool(pr, buffer, &ptr[i].required, &m, WW_BOOL))) {
            return ret;
        }
        /* unpack value - since the value structure is statically-defined
         * instead of a pointer in this struct, we directly unpack it to
         * avoid the malloc */
         m=1;
         if (WW_SUCCESS != (ret = ww_bfrop_unpack_int(pr, buffer, &ptr[i].value.type, &m, WW_INT))) {
            return ret;
        }
        ww_output_verbose(20, ww_globals.debug_output,
                            "ww_bfrop_unpack: info type %d", ptr[i].value.type);
        m=1;
        if (WW_SUCCESS != (ret = unpack_val(pr, buffer, &ptr[i].value))) {
            return ret;
        }
    }
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_buf(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, void *dest,
                                    int32_t *num_vals, ww_data_type_t type)
{
    ww_buffer_t **ptr;
    int32_t i, n, m;
    ww_status_t ret;
    size_t nbytes;

    ptr = (ww_buffer_t **) dest;
    n = *num_vals;

    for (i = 0; i < n; ++i) {
        /* allocate the new object */
        ptr[i] = WW_NEW(ww_buffer_t);
        if (NULL == ptr[i]) {
            return WW_ERR_OUT_OF_RESOURCE;
        }
        /* unpack the number of bytes */
        m=1;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_sizet(pr, buffer, &nbytes, &m, WW_SIZE))) {
            return ret;
        }
        m = nbytes;
        /* setup the buffer's data region */
        if (0 < nbytes) {
            ptr[i]->base_ptr = (char*)malloc(nbytes);
            /* unpack the bytes */
            if (WW_SUCCESS != (ret = ww_bfrop_unpack_byte(pr, buffer, ptr[i]->base_ptr, &m, WW_BYTE))) {
                return ret;
            }
        }
        ptr[i]->pack_ptr = ptr[i]->base_ptr + m;
        ptr[i]->unpack_ptr = ptr[i]->base_ptr;
        ptr[i]->bytes_allocated = nbytes;
        ptr[i]->bytes_used = m;
    }
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_proc(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type)
{
    ww_proc_t *ptr;
    int32_t i, n, m;
    ww_status_t ret;
    char *tmp;

    ww_output_verbose(20, ww_globals.debug_output,
                        "ww_bfrop_unpack: %d procs", *num_vals);

    ptr = (ww_proc_t *) dest;
    n = *num_vals;

    for (i = 0; i < n; ++i) {
        ww_output_verbose(20, ww_globals.debug_output,
                            "ww_bfrop_unpack: init proc[%d]", i);
        memset(&ptr[i], 0, sizeof(ww_proc_t));
        /* unpack nspace */
        m=1;
        tmp = NULL;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_string(pr, buffer, &tmp, &m, WW_STRING))) {
            return ret;
        }
        if (NULL == tmp) {
            return WW_ERROR;
        }
        (void)strncpy(ptr[i].nspace, tmp, WW_MAX_NSLEN);
        free(tmp);
        /* unpack the rank */
        m=1;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_int(pr, buffer, &ptr[i].rank, &m, WW_INT))) {
            return ret;
        }
    }
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_kval(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type)
{
    ww_kval_t *ptr;
    int32_t i, n, m;
    ww_status_t ret;

    ww_output_verbose(20, ww_globals.debug_output,
                        "ww_bfrop_unpack: %d kvals", *num_vals);

    ptr = (ww_kval_t*) dest;
    n = *num_vals;

    for (i = 0; i < n; ++i) {
        WW_CONSTRUCT(&ptr[i], ww_kval_t);
        /* unpack the key */
        m = 1;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_string(pr, buffer, &ptr[i].key, &m, WW_STRING))) {
            WW_ERROR_LOG(ret);
            return ret;
        }
        /* allocate the space */
        ptr[i].value = (ww_value_t*)malloc(sizeof(ww_value_t));
        /* unpack the value */
        m = 1;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_value(pr, buffer, ptr[i].value, &m, WW_VALUE))) {
            WW_ERROR_LOG(ret);
            return ret;
        }
    }
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_array(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type)
{
    ww_info_array_t *ptr;
    int32_t i, n, m;
    ww_status_t ret;

    ww_output_verbose(20, ww_globals.debug_output,
                        "ww_bfrop_unpack: %d info arrays", *num_vals);

    ptr = (ww_info_array_t*) dest;
    n = *num_vals;

    for (i = 0; i < n; ++i) {
        ww_output_verbose(20, ww_globals.debug_output,
                            "ww_bfrop_unpack: init array[%d]", i);
        memset(&ptr[i], 0, sizeof(ww_info_array_t));
        /* unpack the size of this array */
        m=1;
        if (WW_SUCCESS != (ret = ww_bfrop_unpack_sizet(pr, buffer, &ptr[i].size, &m, WW_SIZE))) {
            return ret;
        }
        if (0 < ptr[i].size) {
            ptr[i].array = (struct ww_info_t*)malloc(ptr[i].size * sizeof(ww_info_t));
            m=ptr[i].size;
            if (WW_SUCCESS != (ret = ww_bfrop_unpack_value(pr, buffer, ptr[i].array, &m, WW_INFO))) {
                return ret;
            }
        }
    }
    return WW_SUCCESS;
}

ww_status_t ww_bfrop_unpack_ptr(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, void *dest,
                                    int32_t *num_vals, ww_data_type_t type)
{
    uint8_t foo=1;
    int32_t cnt=1;

    /* it obviously makes no sense to pack a pointer and
     * send it somewhere else, so we just unpack the sentinel */
    return ww_bfrop_unpack_byte(pr, buffer, &foo, &cnt, WW_UINT8);
}

