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
 * Copyright (c) 2016      Mellanox Technologies, Inc.
 *                         All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>

#include <src/include/ww_stdint.h>

#include <stdio.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "src/util/error.h"

#include "src/runtime/ww_rte.h"
#include "src/mca/bfrops/base/base.h"

 ww_status_t ww_bfrop_base_print(struct ww_peer_t *pr,
                                     char **output, char *prefix,
                                     void *src, ww_data_type_t type)
 {
    ww_bfrop_type_info_t *info;
    ww_peer_t *peer = (ww_peer_t*)pr;

    /* check for error */
    if (NULL == output || NULL == src) {
        return WW_ERR_BAD_PARAM;
    }

    /* Lookup the print function for this type and call it */

    if(NULL == (info = (ww_bfrop_type_info_t*)ww_pointer_array_get_item(peer->comm.bfrops->types, type))) {
        return WW_ERR_UNKNOWN_DATA_TYPE;
    }

    return info->odti_print_fn(pr, output, prefix, src, type);
}

/*
 * STANDARD PRINT FUNCTIONS FOR SYSTEM TYPES
 */
 int ww_bfrop_print_bool(struct ww_peer_t *pr,
                           char **output, char *prefix,
                           bool *src, ww_data_type_t type)
 {
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_BOOL\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_BOOL\tValue: %s", prefix,
             (*src) ? "TRUE" : "FALSE");
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_byte(struct ww_peer_t *pr,
                          char **output, char *prefix,
                          uint8_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_BYTE\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_BYTE\tValue: %x", prefix, *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_string(struct ww_peer_t *pr,
                          char **output, char *prefix,
                          char *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_STRING\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_STRING\tValue: %s", prefx, src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_size(struct ww_peer_t *pr,
                          char **output, char *prefix,
                          size_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_SIZE\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_SIZE\tValue: %lu", prefx, (unsigned long) *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_pid(struct ww_peer_t *pr,
                         char **output, char *prefix,
                         pid_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_PID\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_PID\tValue: %lu", prefx, (unsigned long) *src);
    if (prefx != prefix) {
        free(prefx);
    }
    return WW_SUCCESS;
}

int ww_bfrop_print_int(struct ww_peer_t *pr,
                          char **output, char *prefix,
                          int *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_INT\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_INT\tValue: %ld", prefx, (long) *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_uint(struct ww_peer_t *pr,
                          char **output, char *prefix,
                          uint *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_UINT\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_UINT\tValue: %lu", prefx, (unsigned long) *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_uint8(struct ww_peer_t *pr,
                           char **output, char *prefix,
                           uint8_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_UINT8\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_UINT8\tValue: %u", prefx, (unsigned int) *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_uint16(struct ww_peer_t *pr,
                            char **output, char *prefix,
                            uint16_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_UINT16\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_UINT16\tValue: %u", prefx, (unsigned int) *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_uint32(struct ww_peer_t *pr,
                            char **output, char *prefix,
                            uint32_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_UINT32\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_UINT32\tValue: %u", prefx, (unsigned int) *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_int8(struct ww_peer_t *pr,
                          char **output, char *prefix,
                          int8_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_INT8\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_INT8\tValue: %d", prefx, (int) *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_int16(struct ww_peer_t *pr,
                           char **output, char *prefix,
                           int16_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_INT16\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_INT16\tValue: %d", prefx, (int) *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_int32(struct ww_peer_t *pr,
                           char **output, char *prefix,
                           int32_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_INT32\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_INT32\tValue: %d", prefx, (int) *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}
int ww_bfrop_print_uint64(struct ww_peer_t *pr,
                            char **output, char *prefix,
                            uint64_t *src,
                            ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_UINT64\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_UINT64\tValue: %lu", prefx, (unsigned long) *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_int64(struct ww_peer_t *pr,
                           char **output, char *prefix,
                           int64_t *src,
                           ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_INT64\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_INT64\tValue: %ld", prefx, (long) *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_float(struct ww_peer_t *pr,
                           char **output, char *prefix,
                           float *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_FLOAT\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_FLOAT\tValue: %f", prefx, *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_double(struct ww_peer_t *pr,
                            char **output, char *prefix,
                            double *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_DOUBLE\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_DOUBLE\tValue: %f", prefx, *src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_time(struct ww_peer_t *pr,
                          char **output, char *prefix,
                          time_t *src, ww_data_type_t type)
{
    char *prefx;
    char *t;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_TIME\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    t = ctime(src);
    t[strlen(t)-1] = '\0';  // remove trailing newline

    asprintf(output, "%sData type: WW_TIME\tValue: %s", prefx, t);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_timeval(struct ww_peer_t *pr,
                             char **output, char *prefix,
                             struct timeval *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_TIMEVAL\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_TIMEVAL\tValue: %ld.%06ld", prefx,
             (long)src->tv_sec, (long)src->tv_usec);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_status(struct ww_peer_t *pr,
                            char **output, char *prefix,
                            ww_status_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_STATUS\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_STATUS\tValue: %s", prefx, WW_Error_string(*src));
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}


/* PRINT FUNCTIONS FOR GENERIC WW TYPES */

/*
 * WW_VALUE
 */
 int ww_bfrop_print_value(struct ww_peer_t *pr,
                            char **output, char *prefix,
                            ww_value_t *src, ww_data_type_t type)
 {
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_VALUE\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    switch (src->type) {
        case WW_BYTE:
        asprintf(output, "%sWW_VALUE: Data type: WW_BYTE\tValue: %x",
                 prefx, src->data.byte);
        break;
        case WW_STRING:
        asprintf(output, "%sWW_VALUE: Data type: WW_STRING\tValue: %s",
                 prefx, src->data.string);
        break;
        case WW_SIZE:
        asprintf(output, "%sWW_VALUE: Data type: WW_SIZE\tValue: %lu",
                 prefx, (unsigned long)src->data.size);
        break;
        case WW_PID:
        asprintf(output, "%sWW_VALUE: Data type: WW_PID\tValue: %lu",
                 prefx, (unsigned long)src->data.pid);
        break;
        case WW_INT:
        asprintf(output, "%sWW_VALUE: Data type: WW_INT\tValue: %d",
                 prefx, src->data.integer);
        break;
        case WW_INT8:
        asprintf(output, "%sWW_VALUE: Data type: WW_INT8\tValue: %d",
                 prefx, (int)src->data.int8);
        break;
        case WW_INT16:
        asprintf(output, "%sWW_VALUE: Data type: WW_INT16\tValue: %d",
                 prefx, (int)src->data.int16);
        break;
        case WW_INT32:
        asprintf(output, "%sWW_VALUE: Data type: WW_INT32\tValue: %d",
                 prefx, src->data.int32);
        break;
        case WW_INT64:
        asprintf(output, "%sWW_VALUE: Data type: WW_INT64\tValue: %ld",
                 prefx, (long)src->data.int64);
        break;
        case WW_UINT:
        asprintf(output, "%sWW_VALUE: Data type: WW_UINT\tValue: %u",
                 prefx, src->data.uint);
        break;
        case WW_UINT8:
        asprintf(output, "%sWW_VALUE: Data type: WW_UINT8\tValue: %u",
                 prefx, (unsigned int)src->data.uint8);
        break;
        case WW_UINT16:
        asprintf(output, "%sWW_VALUE: Data type: WW_UINT16\tValue: %u",
                 prefx, (unsigned int)src->data.uint16);
        break;
        case WW_UINT32:
        asprintf(output, "%sWW_VALUE: Data type: WW_UINT32\tValue: %u",
                 prefx, src->data.uint32);
        break;
        case WW_UINT64:
        asprintf(output, "%sWW_VALUE: Data type: WW_UINT64\tValue: %lu",
                 prefx, (unsigned long)src->data.uint64);
        break;
        case WW_FLOAT:
        asprintf(output, "%sWW_VALUE: Data type: WW_FLOAT\tValue: %f",
                 prefx, src->data.fval);
        break;
        case WW_DOUBLE:
        asprintf(output, "%sWW_VALUE: Data type: WW_DOUBLE\tValue: %f",
                 prefx, src->data.dval);
        break;
        case WW_TIMEVAL:
        asprintf(output, "%sWW_VALUE: Data type: WW_TIMEVAL\tValue: %ld.%06ld", prefx,
                 (long)src->data.tv.tv_sec, (long)src->data.tv.tv_usec);
        break;
        case WW_STATUS:
        asprintf(output, "%sWW_VALUE: Data type: WW_STATUS\tValue: %s", prefx,
                 WW_Error_string(src->data.status));
        break;
        default:
        asprintf(output, "%sWW_VALUE: Data type: UNKNOWN\tValue: UNPRINTABLE", prefx);
        break;
    }
    if (prefx != prefix) {
        free(prefx);
    }
    return WW_SUCCESS;
}

int ww_bfrop_print_info(struct ww_peer_t *pr,
                          char **output, char *prefix,
                          ww_info_t *src, ww_data_type_t type)
{
    char *tmp;
    ww_peer_t *peer = (ww_peer_t*)pr;

    peer->comm.bfrops->print(pr, &tmp, NULL, &src->value, WW_VALUE);
    asprintf(output, "%sKEY: %s REQD: %s %s", prefix, src->key,
             src->required ? "Y" : "N", (NULL == tmp) ? "WW_VALUE: NULL" : tmp);
    if (NULL != tmp) {
        free(tmp);
    }
    return WW_SUCCESS;
}

int ww_bfrop_print_buf(struct ww_peer_t *pr,
                         char **output, char *prefix,
                         ww_buffer_t *src, ww_data_type_t type)
{
    return WW_SUCCESS;
}

int ww_bfrop_print_proc(struct ww_peer_t *pr,
                          char **output, char *prefix,
                          ww_proc_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    asprintf(output, "%sPROC: %s:%d", prefx, src->nspace, src->rank);
    return WW_SUCCESS;
}

int ww_bfrop_print_kval(struct ww_peer_t *pr,
                          char **output, char *prefix,
                          ww_kval_t *src, ww_data_type_t type)
{
    return WW_SUCCESS;
}

int ww_bfrop_print_array(struct ww_peer_t *pr,
                           char **output, char *prefix,
                           ww_info_array_t *src, ww_data_type_t type)
{
    size_t j;
    char *tmp, *tmp2, *tmp3, *pfx;
    ww_info_t *s1;
    ww_peer_t *peer = (ww_peer_t*)pr;

    asprintf(&tmp, "%sARRAY SIZE: %ld", prefix, (long)src->size);
    asprintf(&pfx, "\n%s\t",  (NULL == prefix) ? "" : prefix);
    s1 = (ww_info_t*)src->array;

    for (j=0; j < src->size; j++) {
        peer->comm.bfrops->print(pr, &tmp2, pfx, &s1[j], WW_INFO);
        asprintf(&tmp3, "%s%s", tmp, tmp2);
        free(tmp);
        free(tmp2);
        tmp = tmp3;
    }
    *output = tmp;
    return WW_SUCCESS;
}

int ww_bfrop_print_bo(struct ww_peer_t *pr,
                        char **output, char *prefix,
                        ww_byte_object_t *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    /* if src is NULL, just print data type and return */
    if (NULL == src) {
        asprintf(output, "%sData type: WW_BYTE_OBJECT\tValue: NULL pointer", prefx);
        if (prefx != prefix) {
            free(prefx);
        }
        return WW_SUCCESS;
    }

    asprintf(output, "%sData type: WW_BYTE_OBJECT\tSize: %ld", prefx, (long)src->size);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

int ww_bfrop_print_ptr(struct ww_peer_t *pr,
                         char **output, char *prefix,
                         void *src, ww_data_type_t type)
{
    char *prefx;

    /* deal with NULL prefix */
    if (NULL == prefix) asprintf(&prefx, " ");
    else prefx = prefix;

    asprintf(output, "%sData type: WW_POINTER\tAddress: %p", prefx, src);
    if (prefx != prefix) {
        free(prefx);
    }

    return WW_SUCCESS;
}

