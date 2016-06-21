/* -*- C -*-
 *
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
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 */
#ifndef WW_BFROP_BASE_H_
#define WW_BFROP_BASE_H_

#include <src/include/ww_config.h>


#ifdef HAVE_SYS_TIME_H
#include <sys/time.h> /* for struct timeval */
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "src/class/ww_pointer_array.h"
#include "src/mca/mca.h"
#include "src/mca/base/mca_base_framework.h"

#include "src/mca/bfrops/bfrops.h"


 BEGIN_C_DECLS

/*
 * MCA Framework
 */
WW_DECLSPEC extern mca_base_framework_t ww_bfrops_base_framework;
/**
 * BFROP select function
 *
 * Cycle across available components and construct the list
 * of active modules
 */
 WW_DECLSPEC ww_status_t ww_bfrop_base_select(void);

/* conversion macros */
 #define ww_htons htons
 #define ww_ntohs ntohs


 /*
  * Convert a 64 bit value to network byte order.
  */
 static inline uint64_t ww_hton64(uint64_t val) __ww_attribute_const__;
 static inline uint64_t ww_hton64(uint64_t val)
 {
 #ifdef HAVE_UNIX_BYTESWAP
     union { uint64_t ll;
             uint32_t l[2];
     } w, r;

     /* platform already in network byte order? */
     if(htonl(1) == 1L)
         return val;
     w.ll = val;
     r.l[0] = htonl(w.l[1]);
     r.l[1] = htonl(w.l[0]);
     return r.ll;
 #else
     return val;
 #endif
 }

 /*
  * Convert a 64 bit value from network to host byte order.
  */

 static inline uint64_t ww_ntoh64(uint64_t val) __ww_attribute_const__;
 static inline uint64_t ww_ntoh64(uint64_t val)
 {
 #ifdef HAVE_UNIX_BYTESWAP
     union { uint64_t ll;
             uint32_t l[2];
     } w, r;

     /* platform already in network byte order? */
     if(htonl(1) == 1L)
         return val;
     w.ll = val;
     r.l[0] = ntohl(w.l[1]);
     r.l[1] = ntohl(w.l[0]);
     return r.ll;
 #else
     return val;
 #endif
 }


/**
 * Track an active component / module
 */
struct ww_bfrops_base_active_module_t {
    ww_list_item_t super;
    ww_status_t pri;
    ww_bfrops_module_t *module;
    ww_bfrops_base_component_t *component;
};
typedef struct ww_bfrops_base_active_module_t ww_bfrops_base_active_module_t;
WW_CLASS_DECLARATION(ww_bfrops_base_active_module_t);


/* framework globals */
struct ww_bfrops_globals_t {
  ww_list_t actives;
  bool initialized;
  size_t initial_size;
  size_t threshold_size;
  ww_bfrop_buffer_type_t default_type;
};
typedef struct ww_bfrops_globals_t ww_bfrops_globals_t;

extern ww_bfrops_globals_t ww_bfrops_globals;

/*
 * The default starting chunk size
 */
#define WW_BFROP_DEFAULT_INITIAL_SIZE  128
/*
 * The default threshold size when we switch from doubling the
 * buffer size to additively increasing it
 */
#define WW_BFROP_DEFAULT_THRESHOLD_SIZE 1024

/*
 * Internal type corresponding to size_t.  Do not use this in
 * interface calls - use WW_SIZE instead.
 */
#if SIZEOF_SIZE_T == 1
#define BFROP_TYPE_SIZE_T WW_UINT8
#elif SIZEOF_SIZE_T == 2
#define BFROP_TYPE_SIZE_T WW_UINT16
#elif SIZEOF_SIZE_T == 4
#define BFROP_TYPE_SIZE_T WW_UINT32
#elif SIZEOF_SIZE_T == 8
#define BFROP_TYPE_SIZE_T WW_UINT64
#else
#error Unsupported size_t size!
#endif

/*
 * Internal type corresponding to bool.  Do not use this in interface
 * calls - use WW_BOOL instead.
 */
#if SIZEOF__BOOL == 1
#define BFROP_TYPE_BOOL WW_UINT8
#elif SIZEOF__BOOL == 2
#define BFROP_TYPE_BOOL WW_UINT16
#elif SIZEOF__BOOL == 4
#define BFROP_TYPE_BOOL WW_UINT32
#elif SIZEOF__BOOL == 8
#define BFROP_TYPE_BOOL WW_UINT64
#else
#error Unsupported bool size!
#endif

/*
 * Internal type corresponding to int and unsigned int.  Do not use
 * this in interface calls - use WW_INT / WW_UINT instead.
 */
#if SIZEOF_INT == 1
#define BFROP_TYPE_INT WW_INT8
#define BFROP_TYPE_UINT WW_UINT8
#elif SIZEOF_INT == 2
#define BFROP_TYPE_INT WW_INT16
#define BFROP_TYPE_UINT WW_UINT16
#elif SIZEOF_INT == 4
#define BFROP_TYPE_INT WW_INT32
#define BFROP_TYPE_UINT WW_UINT32
#elif SIZEOF_INT == 8
#define BFROP_TYPE_INT WW_INT64
#define BFROP_TYPE_UINT WW_UINT64
#else
#error Unsupported INT size!
#endif

/*
 * Internal type corresponding to pid_t.  Do not use this in interface
 * calls - use WW_PID instead.
 */
#if SIZEOF_PID_T == 1
#define BFROP_TYPE_PID_T WW_UINT8
#elif SIZEOF_PID_T == 2
#define BFROP_TYPE_PID_T WW_UINT16
#elif SIZEOF_PID_T == 4
#define BFROP_TYPE_PID_T WW_UINT32
#elif SIZEOF_PID_T == 8
#define BFROP_TYPE_PID_T WW_UINT64
#else
#error Unsupported pid_t size!
#endif

/* Unpack generic size macros */
#define UNPACK_SIZE_MISMATCH(unpack_type, remote_type, ret)                 \
 do {                                                                    \
    switch(remote_type) {                                               \
        case WW_UINT8:                                                    \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, uint8_t, remote_type);  \
        break;                                                          \
        case WW_INT8:                                                     \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, int8_t, remote_type);   \
        break;                                                          \
        case WW_UINT16:                                                   \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, uint16_t, remote_type); \
        break;                                                          \
        case WW_INT16:                                                    \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, int16_t, remote_type);  \
        break;                                                          \
        case WW_UINT32:                                                   \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, uint32_t, remote_type); \
        break;                                                          \
        case WW_INT32:                                                    \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, int32_t, remote_type);  \
        break;                                                          \
        case WW_UINT64:                                                   \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, uint64_t, remote_type); \
        break;                                                          \
        case WW_INT64:                                                    \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, int64_t, remote_type);  \
        break;                                                          \
        default:                                                            \
        ret = WW_ERR_NOT_FOUND;                                       \
    }                                                                   \
} while (0)

/* NOTE: do not need to deal with endianness here, as the unpacking of
   the underling sender-side type will do that for us.  Repeat: the
   data in tmpbuf[] is already in host byte order. */
#define UNPACK_SIZE_MISMATCH_FOUND(unpack_type, tmptype, tmpbfroptype)        \
   do {                                                                    \
    int32_t i;                                                          \
    tmptype *tmpbuf = (tmptype*)malloc(sizeof(tmptype) * (*num_vals));  \
    ret = ww_bfrop_unpack_buffer(pr, buffer, tmpbuf, num_vals, tmpbfroptype); \
    for (i = 0 ; i < *num_vals ; ++i) {                                 \
        ((unpack_type*) dest)[i] = (unpack_type)(tmpbuf[i]);            \
    }                                                                   \
    free(tmpbuf);                                                       \
} while (0)


/**
 * Internal struct used for holding registered bfrop functions
 */
 typedef struct {
    ww_object_t super;
    /* type identifier */
    ww_data_type_t odti_type;
    /** Debugging string name */
    char *odti_name;
    /** Pack function */
    ww_bfrop_pack_fn_t odti_pack_fn;
    /** Unpack function */
    ww_bfrop_unpack_fn_t odti_unpack_fn;
    /** copy function */
    ww_bfrop_copy_fn_t odti_copy_fn;
    /** prww_status_t function */
    ww_bfrop_print_fn_t odti_print_fn;
} ww_bfrop_type_info_t;
WW_CLASS_DECLARATION(ww_bfrop_type_info_t);

/* macro for registering data types - overwrite an existing
 * duplicate one based on type name */
#define WW_REGISTER_TYPE(n, t, p, u, c, pr, arr)                  \
 do {                                                               \
    ww_bfrop_type_info_t *_info;                                  \
    _info = WW_NEW(ww_bfrop_type_info_t);                       \
    _info->odti_name = strdup((n));                                 \
    _info->odti_type = (t);                                         \
    _info->odti_pack_fn = (ww_bfrop_pack_fn_t)(p);                \
    _info->odti_unpack_fn = (ww_bfrop_unpack_fn_t)(u);            \
    _info->odti_copy_fn = (ww_bfrop_copy_fn_t)(c) ;               \
    _info->odti_print_fn = (ww_bfrop_print_fn_t)(pr) ;            \
    ww_pointer_array_set_item((arr), (t), _info);                 \
} while (0)


/* frame-level API */
ww_status_t ww_bfrop_base_pack(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer,
                                   const void *src, ww_status_t num_vals,
                                   ww_data_type_t type);
ww_status_t ww_bfrop_base_copy(struct ww_peer_t *pr,
                                   void **dest, void *src,
                                   ww_data_type_t type);

/*
 * "Standard" pack functions
 */

ww_status_t ww_bfrop_pack_bool(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer, const void *src,
                                   int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_byte(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer, const void *src,
                                   int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_string(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, const void *src,
                                     int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_sizet(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, const void *src,
                                    int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_pid(struct ww_peer_t *pr,
                                  ww_buffer_t *buffer, const void *src,
                                  int32_t num_vals, ww_data_type_t type);

ww_status_t ww_bfrop_pack_int(struct ww_peer_t *pr,
                                  ww_buffer_t *buffer, const void *src,
                                  int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_int16(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, const void *src,
                                    int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_int32(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, const void *src,
                                    int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_datatype(struct ww_peer_t *pr,
                                       ww_buffer_t *buffer, const void *src,
                                       int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_int64(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, const void *src,
                                    int32_t num_vals, ww_data_type_t type);

ww_status_t ww_bfrop_pack_float(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, const void *src,
                                    int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_double(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, const void *src,
                                     int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_time(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer, const void *src,
                                   int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_timeval(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, const void *src,
                                      int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_time(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer, const void *src,
                                   int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_status(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, const void *src,
                                     int32_t num_vals, ww_data_type_t type);

ww_status_t ww_bfrop_pack_value(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, const void *src,
                                    int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_array(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, const void *src,
                                    int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_proc(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer, const void *src,
                                   int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_info(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer, const void *src,
                                   int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_buf(struct ww_peer_t *pr,
                                  ww_buffer_t *buffer, const void *src,
                                  int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_kval(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer, const void *src,
                                   int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_bo(struct ww_peer_t *pr,
                                 ww_buffer_t *buffer, const void *src,
                                 int32_t num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_pack_ptr(struct ww_peer_t *pr,
                                  ww_buffer_t *buffer, const void *src,
                                  int32_t num_vals, ww_data_type_t type);

/*
* "Standard" unpack functions
*/
ww_status_t ww_bfrop_base_unpack(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer,
                                     void *dst, int32_t *num_vals,
                                     ww_data_type_t type);
ww_status_t ww_bfrop_unpack_buffer(struct ww_peer_t *pr,
                                       ww_buffer_t *buffer,
                                       void *dst, int32_t *num_vals,
                                       ww_data_type_t type);

ww_status_t ww_bfrop_unpack_bool(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_byte(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_string(struct ww_peer_t *pr,
                                       ww_buffer_t *buffer, void *dest,
                                       int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_sizet(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_pid(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, void *dest,
                                    int32_t *num_vals, ww_data_type_t type);

ww_status_t ww_bfrop_unpack_int(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, void *dest,
                                    int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_int16(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_int32(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_datatype(struct ww_peer_t *pr,
                                         ww_buffer_t *buffer, void *dest,
                                         int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_int64(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type);

ww_status_t ww_bfrop_unpack_float(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_double(struct ww_peer_t *pr,
                                       ww_buffer_t *buffer, void *dest,
                                       int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_timeval(struct ww_peer_t *pr,
                                        ww_buffer_t *buffer, void *dest,
                                        int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_time(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_status(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type);

ww_status_t ww_bfrop_unpack_value(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_array(struct ww_peer_t *pr,
                                      ww_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_proc(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_info(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_buf(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, void *dest,
                                    int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_kval(struct ww_peer_t *pr,
                                     ww_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_bo(struct ww_peer_t *pr,
                                   ww_buffer_t *buffer, void *dest,
                                   int32_t *num_vals, ww_data_type_t type);
ww_status_t ww_bfrop_unpack_ptr(struct ww_peer_t *pr,
                                    ww_buffer_t *buffer, void *dest,
                                    int32_t *num_vals, ww_data_type_t type);

/*
* "Standard" copy functions
*/

ww_status_t ww_bfrop_std_copy(struct ww_peer_t *pr,
                                  void **dest, void *src, ww_data_type_t type);

ww_status_t ww_bfrop_copy_string(struct ww_peer_t *pr,
                                     char **dest, char *src, ww_data_type_t type);

ww_status_t ww_bfrop_copy_value(struct ww_peer_t *pr,
                                    ww_value_t **dest, ww_value_t *src,
                                    ww_data_type_t type);
ww_status_t ww_bfrop_copy_array(struct ww_peer_t *pr,
                                    ww_info_array_t **dest, ww_info_array_t *src,
                                    ww_data_type_t type);
ww_status_t ww_bfrop_copy_proc(struct ww_peer_t *pr,
                                   ww_proc_t **dest, ww_proc_t *src,
                                   ww_data_type_t type);
ww_status_t ww_bfrop_copy_info(struct ww_peer_t *pr,
                                   ww_info_t **dest, ww_info_t *src,
                                   ww_data_type_t type);
ww_status_t ww_bfrop_copy_buf(struct ww_peer_t *pr,
                                  ww_buffer_t **dest, ww_buffer_t *src,
                                  ww_data_type_t type);
ww_status_t ww_bfrop_copy_kval(struct ww_peer_t *pr,
                                   ww_kval_t **dest, ww_kval_t *src,
                                   ww_data_type_t type);
ww_status_t ww_bfrop_copy_bo(struct ww_peer_t *pr,
                                 ww_byte_object_t **dest, ww_byte_object_t *src,
                                 ww_data_type_t type);
/*
* "Standard" print functions
*/
ww_status_t ww_bfrop_base_print(struct ww_peer_t *pr,
                                    char **output, char *prefix,
                                    void *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_bool(struct ww_peer_t *pr,
                                    char **output, char *prefix,
                                    bool *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_byte(struct ww_peer_t *pr,
                                    char **output, char *prefix,
                                    uint8_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_string(struct ww_peer_t *pr,
                                      char **output, char *prefix,
                                      char *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_size(struct ww_peer_t *pr,
                                    char **output, char *prefix,
                                    size_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_pid(struct ww_peer_t *pr,
                                   char **output, char *prefix,
                                   pid_t *src, ww_data_type_t type);

ww_status_t ww_bfrop_print_int(struct ww_peer_t *pr,
                                   char **output, char *prefix,
                                   int *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_int8(struct ww_peer_t *pr,
                                    char **output, char *prefix,
                                    int8_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_int16(struct ww_peer_t *pr,
                                     char **output, char *prefix,
                                     int16_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_int32(struct ww_peer_t *pr,
                                     char **output, char *prefix,
                                     int32_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_int64(struct ww_peer_t *pr,
                                     char **output, char *prefix,
                                     int64_t *src, ww_data_type_t type);

ww_status_t ww_bfrop_print_uint(struct ww_peer_t *pr,
                                    char **output, char *prefix,
                                    uint *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_uint8(struct ww_peer_t *pr,
                                     char **output, char *prefix,
                                     uint8_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_uint16(struct ww_peer_t *pr,
                                      char **output, char *prefix,
                                      uint16_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_uint32(struct ww_peer_t *pr,
                                      char **output, char *prefix,
                                      uint32_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_uint64(struct ww_peer_t *pr,
                                      char **output, char *prefix,
                                      uint64_t *src, ww_data_type_t type);

ww_status_t ww_bfrop_print_float(struct ww_peer_t *pr,
                                     char **output, char *prefix,
                                     float *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_double(struct ww_peer_t *pr,
                                      char **output, char *prefix,
                                      double *src, ww_data_type_t type);

ww_status_t ww_bfrop_print_timeval(struct ww_peer_t *pr,
                                       char **output, char *prefix,
                                       struct timeval *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_time(struct ww_peer_t *pr,
                                    char **output, char *prefix,
                                    time_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_status(struct ww_peer_t *pr,
                                      char **output, char *prefix,
                                      ww_status_t *src, ww_data_type_t type);

ww_status_t ww_bfrop_print_value(struct ww_peer_t *pr,
                                     char **output, char *prefix,
                                     ww_value_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_array(struct ww_peer_t *pr,
                                     char **output, char *prefix,
                                     ww_info_array_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_proc(struct ww_peer_t *pr,
                                    char **output, char *prefix,
                                    ww_proc_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_info(struct ww_peer_t *pr,
                                    char **output, char *prefix,
                                    ww_info_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_buf(struct ww_peer_t *pr,
                                   char **output, char *prefix,
                                   ww_buffer_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_kval(struct ww_peer_t *pr,
                                    char **output, char *prefix,
                                    ww_kval_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_bo(struct ww_peer_t *pr,
                                  char **output, char *prefix,
                                  ww_byte_object_t *src, ww_data_type_t type);
ww_status_t ww_bfrop_print_ptr(struct ww_peer_t *pr,
                                   char **output, char *prefix,
                                   void *src, ww_data_type_t type);

/*
 * Common helper functions
 */

char* ww_bfrop_buffer_extend(ww_buffer_t *bptr, size_t bytes_to_add);

bool ww_bfrop_too_small(ww_buffer_t *buffer, size_t bytes_reqd);

ww_bfrop_type_info_t* ww_bfrop_find_type(ww_data_type_t type);

ww_status_t ww_bfrop_store_data_type(ww_buffer_t *buffer, ww_data_type_t type);

ww_status_t ww_bfrop_get_data_type(ww_buffer_t *buffer, ww_data_type_t *type);

ww_status_t ww_bfrop_base_copy_payload(struct ww_peer_t *pr,
                                           ww_buffer_t *dest,
                                           ww_buffer_t *src);

void ww_base_value_load(struct ww_peer_t *pr,
                          ww_value_t *v, void *data,
                          ww_data_type_t type);

ww_status_t ww_base_value_unload(struct ww_peer_t *pr,
                                     ww_value_t *kv, void **data,
                                     size_t *sz, ww_data_type_t type);

ww_status_t ww_base_value_xfer(struct ww_peer_t *pr,
                                   ww_value_t *p,
                                   ww_value_t *src);

ww_value_cmp_t ww_base_value_cmp(struct ww_peer_t *pr,
                                     ww_value_t *p,
                                     ww_value_t *p1);

 END_C_DECLS

#endif
