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
#ifndef PMIX_STATE_BASE_H_
#define PMIX_STATE_BASE_H_

#include <src/include/pmix_config.h>


#ifdef HAVE_SYS_TIME_H
#include <sys/time.h> /* for struct timeval */
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#if PMIX_HAVE_HWLOC
#include PMIX_HWLOC_HEADER
#endif

#include "src/class/pmix_pointer_array.h"
#include "src/mca/mca.h"
#include "src/mca/base/mca_base_framework.h"

#include "src/mca/states/states.h"


 BEGIN_C_DECLS

/*
 * MCA Framework
 */
PMIX_DECLSPEC extern mca_base_framework_t pmix_states_base_framework;
/**
 * STATE select function
 *
 * Cycle across available components and construct the list
 * of active modules
 */
 PMIX_DECLSPEC pmix_status_t pmix_state_base_select(void);

/**
 * Track an active component / module
 */
struct pmix_states_base_active_module_t {
    pmix_list_item_t super;
    pmix_status_t pri;
    pmix_states_module_t *module;
    pmix_states_base_component_t *component;
};
typedef struct pmix_states_base_active_module_t pmix_states_base_active_module_t;
PMIX_CLASS_DECLARATION(pmix_states_base_active_module_t);


/* framework globals */
struct pmix_states_globals_t {
  pmix_list_t actives;
  bool initialized;
  size_t initial_size;
  size_t threshold_size;
  pmix_state_buffer_type_t default_type;
};
typedef struct pmix_states_globals_t pmix_states_globals_t;

extern pmix_states_globals_t pmix_states_globals;

/*
 * The default starting chunk size
 */
#define PMIX_STATE_DEFAULT_INITIAL_SIZE  128
/*
 * The default threshold size when we switch from doubling the
 * buffer size to additively increasing it
 */
#define PMIX_STATE_DEFAULT_THRESHOLD_SIZE 1024

/*
 * Internal type corresponding to size_t.  Do not use this in
 * interface calls - use PMIX_SIZE instead.
 */
#if SIZEOF_SIZE_T == 1
#define STATE_TYPE_SIZE_T PMIX_UINT8
#elif SIZEOF_SIZE_T == 2
#define STATE_TYPE_SIZE_T PMIX_UINT16
#elif SIZEOF_SIZE_T == 4
#define STATE_TYPE_SIZE_T PMIX_UINT32
#elif SIZEOF_SIZE_T == 8
#define STATE_TYPE_SIZE_T PMIX_UINT64
#else
#error Unsupported size_t size!
#endif

/*
 * Internal type corresponding to bool.  Do not use this in interface
 * calls - use PMIX_BOOL instead.
 */
#if SIZEOF__BOOL == 1
#define STATE_TYPE_BOOL PMIX_UINT8
#elif SIZEOF__BOOL == 2
#define STATE_TYPE_BOOL PMIX_UINT16
#elif SIZEOF__BOOL == 4
#define STATE_TYPE_BOOL PMIX_UINT32
#elif SIZEOF__BOOL == 8
#define STATE_TYPE_BOOL PMIX_UINT64
#else
#error Unsupported bool size!
#endif

/*
 * Internal type corresponding to int and unsigned int.  Do not use
 * this in interface calls - use PMIX_INT / PMIX_UINT instead.
 */
#if SIZEOF_INT == 1
#define STATE_TYPE_INT PMIX_INT8
#define STATE_TYPE_UINT PMIX_UINT8
#elif SIZEOF_INT == 2
#define STATE_TYPE_INT PMIX_INT16
#define STATE_TYPE_UINT PMIX_UINT16
#elif SIZEOF_INT == 4
#define STATE_TYPE_INT PMIX_INT32
#define STATE_TYPE_UINT PMIX_UINT32
#elif SIZEOF_INT == 8
#define STATE_TYPE_INT PMIX_INT64
#define STATE_TYPE_UINT PMIX_UINT64
#else
#error Unsupported INT size!
#endif

/*
 * Internal type corresponding to pid_t.  Do not use this in interface
 * calls - use PMIX_PID instead.
 */
#if SIZEOF_PID_T == 1
#define STATE_TYPE_PID_T PMIX_UINT8
#elif SIZEOF_PID_T == 2
#define STATE_TYPE_PID_T PMIX_UINT16
#elif SIZEOF_PID_T == 4
#define STATE_TYPE_PID_T PMIX_UINT32
#elif SIZEOF_PID_T == 8
#define STATE_TYPE_PID_T PMIX_UINT64
#else
#error Unsupported pid_t size!
#endif

/* Unpack generic size macros */
#define UNPACK_SIZE_MISMATCH(unpack_type, remote_type, ret)                 \
 do {                                                                    \
    switch(remote_type) {                                               \
        case PMIX_UINT8:                                                    \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, uint8_t, remote_type);  \
        break;                                                          \
        case PMIX_INT8:                                                     \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, int8_t, remote_type);   \
        break;                                                          \
        case PMIX_UINT16:                                                   \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, uint16_t, remote_type); \
        break;                                                          \
        case PMIX_INT16:                                                    \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, int16_t, remote_type);  \
        break;                                                          \
        case PMIX_UINT32:                                                   \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, uint32_t, remote_type); \
        break;                                                          \
        case PMIX_INT32:                                                    \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, int32_t, remote_type);  \
        break;                                                          \
        case PMIX_UINT64:                                                   \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, uint64_t, remote_type); \
        break;                                                          \
        case PMIX_INT64:                                                    \
        UNPACK_SIZE_MISMATCH_FOUND(unpack_type, int64_t, remote_type);  \
        break;                                                          \
        default:                                                            \
        ret = PMIX_ERR_NOT_FOUND;                                       \
    }                                                                   \
} while (0)

/* NOTE: do not need to deal with endianness here, as the unpacking of
   the underling sender-side type will do that for us.  Repeat: the
   data in tmpbuf[] is already in host byte order. */
#define UNPACK_SIZE_MISMATCH_FOUND(unpack_type, tmptype, tmpstatetype)        \
   do {                                                                    \
    int32_t i;                                                          \
    tmptype *tmpbuf = (tmptype*)malloc(sizeof(tmptype) * (*num_vals));  \
    ret = pmix_state_unpack_buffer(pr, buffer, tmpbuf, num_vals, tmpstatetype); \
    for (i = 0 ; i < *num_vals ; ++i) {                                 \
        ((unpack_type*) dest)[i] = (unpack_type)(tmpbuf[i]);            \
    }                                                                   \
    free(tmpbuf);                                                       \
} while (0)


/**
 * Internal struct used for holding registered state functions
 */
 typedef struct {
    pmix_object_t super;
    /* type identifier */
    pmix_data_type_t odti_type;
    /** Debugging string name */
    char *odti_name;
    /** Pack function */
    pmix_state_pack_fn_t odti_pack_fn;
    /** Unpack function */
    pmix_state_unpack_fn_t odti_unpack_fn;
    /** copy function */
    pmix_state_copy_fn_t odti_copy_fn;
    /** prpmix_status_t function */
    pmix_state_print_fn_t odti_print_fn;
} pmix_state_type_info_t;
PMIX_CLASS_DECLARATION(pmix_state_type_info_t);

/* macro for registering data types - overwrite an existing
 * duplicate one based on type name */
#define PMIX_REGISTER_TYPE(n, t, p, u, c, pr, arr)                  \
 do {                                                               \
    pmix_state_type_info_t *_info;                                  \
    _info = PMIX_NEW(pmix_state_type_info_t);                       \
    _info->odti_name = strdup((n));                                 \
    _info->odti_type = (t);                                         \
    _info->odti_pack_fn = (pmix_state_pack_fn_t)(p);                \
    _info->odti_unpack_fn = (pmix_state_unpack_fn_t)(u);            \
    _info->odti_copy_fn = (pmix_state_copy_fn_t)(c) ;               \
    _info->odti_print_fn = (pmix_state_print_fn_t)(pr) ;            \
    pmix_pointer_array_set_item((arr), (t), _info);                 \
} while (0)


/* frame-level API */
pmix_status_t pmix_state_base_pack(struct pmix_peer_t *pr,
                                   pmix_buffer_t *buffer,
                                   const void *src, pmix_status_t num_vals,
                                   pmix_data_type_t type);
pmix_status_t pmix_state_base_copy(struct pmix_peer_t *pr,
                                   void **dest, void *src,
                                   pmix_data_type_t type);

/*
 * "Standard" pack functions
 */

pmix_status_t pmix_state_pack_bool(struct pmix_peer_t *pr,
                                   pmix_buffer_t *buffer, const void *src,
                                   int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_byte(struct pmix_peer_t *pr,
                                   pmix_buffer_t *buffer, const void *src,
                                   int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_string(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer, const void *src,
                                     int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_sizet(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, const void *src,
                                    int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_pid(struct pmix_peer_t *pr,
                                  pmix_buffer_t *buffer, const void *src,
                                  int32_t num_vals, pmix_data_type_t type);

pmix_status_t pmix_state_pack_int(struct pmix_peer_t *pr,
                                  pmix_buffer_t *buffer, const void *src,
                                  int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_int16(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, const void *src,
                                    int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_int32(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, const void *src,
                                    int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_datatype(struct pmix_peer_t *pr,
                                       pmix_buffer_t *buffer, const void *src,
                                       int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_int64(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, const void *src,
                                    int32_t num_vals, pmix_data_type_t type);

pmix_status_t pmix_state_pack_float(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, const void *src,
                                    int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_double(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer, const void *src,
                                     int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_time(struct pmix_peer_t *pr,
                                   pmix_buffer_t *buffer, const void *src,
                                   int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_timeval(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, const void *src,
                                      int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_time(struct pmix_peer_t *pr,
                                   pmix_buffer_t *buffer, const void *src,
                                   int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_status(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer, const void *src,
                                     int32_t num_vals, pmix_data_type_t type);

#if PMIX_HAVE_HWLOC
pmix_status_t pmix_state_pack_topo(struct pmix_peer_t *pr,
                                   pmix_buffer_t *buffer, const void *src,
                                   int32_t num_vals, pmix_data_type_t type);
#endif
pmix_status_t pmix_state_pack_value(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, const void *src,
                                    int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_array(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, const void *src,
                                    int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_proc(struct pmix_peer_t *pr,
                                   pmix_buffer_t *buffer, const void *src,
                                   int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_app(struct pmix_peer_t *pr,
                                  pmix_buffer_t *buffer, const void *src,
                                  int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_info(struct pmix_peer_t *pr,
                                   pmix_buffer_t *buffer, const void *src,
                                   int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_buf(struct pmix_peer_t *pr,
                                  pmix_buffer_t *buffer, const void *src,
                                  int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_kval(struct pmix_peer_t *pr,
                                   pmix_buffer_t *buffer, const void *src,
                                   int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_modex(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, const void *src,
                                    int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_persist(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, const void *src,
                                      int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_bo(struct pmix_peer_t *pr,
                                 pmix_buffer_t *buffer, const void *src,
                                 int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_pdata(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, const void *src,
                                    int32_t num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_pack_ptr(struct pmix_peer_t *pr,
                                  pmix_buffer_t *buffer, const void *src,
                                  int32_t num_vals, pmix_data_type_t type);

/*
* "Standard" unpack functions
*/
pmix_status_t pmix_state_base_unpack(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer,
                                     void *dst, int32_t *num_vals,
                                     pmix_data_type_t type);
pmix_status_t pmix_state_unpack_buffer(struct pmix_peer_t *pr,
                                       pmix_buffer_t *buffer,
                                       void *dst, int32_t *num_vals,
                                       pmix_data_type_t type);

pmix_status_t pmix_state_unpack_bool(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_byte(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_string(struct pmix_peer_t *pr,
                                       pmix_buffer_t *buffer, void *dest,
                                       int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_sizet(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_pid(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, void *dest,
                                    int32_t *num_vals, pmix_data_type_t type);

pmix_status_t pmix_state_unpack_int(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, void *dest,
                                    int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_int16(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_int32(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_datatype(struct pmix_peer_t *pr,
                                         pmix_buffer_t *buffer, void *dest,
                                         int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_int64(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, pmix_data_type_t type);

pmix_status_t pmix_state_unpack_float(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_double(struct pmix_peer_t *pr,
                                       pmix_buffer_t *buffer, void *dest,
                                       int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_timeval(struct pmix_peer_t *pr,
                                        pmix_buffer_t *buffer, void *dest,
                                        int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_time(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_status(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, pmix_data_type_t type);

#if PMIX_HAVE_HWLOC
pmix_status_t pmix_state_unpack_topo(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, pmix_data_type_t type);
#endif
pmix_status_t pmix_state_unpack_value(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_array(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_proc(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_app(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_info(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_buf(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, void *dest,
                                    int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_kval(struct pmix_peer_t *pr,
                                     pmix_buffer_t *buffer, void *dest,
                                     int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_modex(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_persist(struct pmix_peer_t *pr,
                                        pmix_buffer_t *buffer, void *dest,
                                        int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_bo(struct pmix_peer_t *pr,
                                   pmix_buffer_t *buffer, void *dest,
                                   int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_pdata(struct pmix_peer_t *pr,
                                      pmix_buffer_t *buffer, void *dest,
                                      int32_t *num_vals, pmix_data_type_t type);
pmix_status_t pmix_state_unpack_ptr(struct pmix_peer_t *pr,
                                    pmix_buffer_t *buffer, void *dest,
                                    int32_t *num_vals, pmix_data_type_t type);

/*
* "Standard" copy functions
*/

pmix_status_t pmix_state_std_copy(struct pmix_peer_t *pr,
                                  void **dest, void *src, pmix_data_type_t type);

pmix_status_t pmix_state_copy_string(struct pmix_peer_t *pr,
                                     char **dest, char *src, pmix_data_type_t type);

#if PMIX_HAVE_HWLOC
pmix_status_t pmix_state_copy_topo(struct pmix_peer_t *pr,
                                   hwloc_topology_t *dest,
                                   hwloc_topology_t src,
                                   pmix_data_type_t type);
#endif
pmix_status_t pmix_state_copy_value(struct pmix_peer_t *pr,
                                    pmix_value_t **dest, pmix_value_t *src,
                                    pmix_data_type_t type);
pmix_status_t pmix_state_copy_array(struct pmix_peer_t *pr,
                                    pmix_info_array_t **dest, pmix_info_array_t *src,
                                    pmix_data_type_t type);
pmix_status_t pmix_state_copy_proc(struct pmix_peer_t *pr,
                                   pmix_proc_t **dest, pmix_proc_t *src,
                                   pmix_data_type_t type);
pmix_status_t pmix_state_copy_app(struct pmix_peer_t *pr,
                                  pmix_app_t **dest, pmix_app_t *src,
                                  pmix_data_type_t type);
pmix_status_t pmix_state_copy_info(struct pmix_peer_t *pr,
                                   pmix_info_t **dest, pmix_info_t *src,
                                   pmix_data_type_t type);
pmix_status_t pmix_state_copy_buf(struct pmix_peer_t *pr,
                                  pmix_buffer_t **dest, pmix_buffer_t *src,
                                  pmix_data_type_t type);
pmix_status_t pmix_state_copy_kval(struct pmix_peer_t *pr,
                                   pmix_kval_t **dest, pmix_kval_t *src,
                                   pmix_data_type_t type);
pmix_status_t pmix_state_copy_modex(struct pmix_peer_t *pr,
                                    pmix_modex_data_t **dest, pmix_modex_data_t *src,
                                    pmix_data_type_t type);
pmix_status_t pmix_state_copy_persist(struct pmix_peer_t *pr,
                                      pmix_persistence_t **dest, pmix_persistence_t *src,
                                      pmix_data_type_t type);
pmix_status_t pmix_state_copy_bo(struct pmix_peer_t *pr,
                                 pmix_byte_object_t **dest, pmix_byte_object_t *src,
                                 pmix_data_type_t type);
pmix_status_t pmix_state_copy_pdata(struct pmix_peer_t *pr,
                                    pmix_pdata_t **dest, pmix_pdata_t *src,
                                    pmix_data_type_t type);
/*
* "Standard" print functions
*/
pmix_status_t pmix_state_base_print(struct pmix_peer_t *pr,
                                    char **output, char *prefix,
                                    void *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_bool(struct pmix_peer_t *pr,
                                    char **output, char *prefix,
                                    bool *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_byte(struct pmix_peer_t *pr,
                                    char **output, char *prefix,
                                    uint8_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_string(struct pmix_peer_t *pr,
                                      char **output, char *prefix,
                                      char *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_size(struct pmix_peer_t *pr,
                                    char **output, char *prefix,
                                    size_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_pid(struct pmix_peer_t *pr,
                                   char **output, char *prefix,
                                   pid_t *src, pmix_data_type_t type);

pmix_status_t pmix_state_print_int(struct pmix_peer_t *pr,
                                   char **output, char *prefix,
                                   int *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_int8(struct pmix_peer_t *pr,
                                    char **output, char *prefix,
                                    int8_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_int16(struct pmix_peer_t *pr,
                                     char **output, char *prefix,
                                     int16_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_int32(struct pmix_peer_t *pr,
                                     char **output, char *prefix,
                                     int32_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_int64(struct pmix_peer_t *pr,
                                     char **output, char *prefix,
                                     int64_t *src, pmix_data_type_t type);

pmix_status_t pmix_state_print_uint(struct pmix_peer_t *pr,
                                    char **output, char *prefix,
                                    uint *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_uint8(struct pmix_peer_t *pr,
                                     char **output, char *prefix,
                                     uint8_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_uint16(struct pmix_peer_t *pr,
                                      char **output, char *prefix,
                                      uint16_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_uint32(struct pmix_peer_t *pr,
                                      char **output, char *prefix,
                                      uint32_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_uint64(struct pmix_peer_t *pr,
                                      char **output, char *prefix,
                                      uint64_t *src, pmix_data_type_t type);

pmix_status_t pmix_state_print_float(struct pmix_peer_t *pr,
                                     char **output, char *prefix,
                                     float *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_double(struct pmix_peer_t *pr,
                                      char **output, char *prefix,
                                      double *src, pmix_data_type_t type);

pmix_status_t pmix_state_print_timeval(struct pmix_peer_t *pr,
                                       char **output, char *prefix,
                                       struct timeval *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_time(struct pmix_peer_t *pr,
                                    char **output, char *prefix,
                                    time_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_status(struct pmix_peer_t *pr,
                                      char **output, char *prefix,
                                      pmix_status_t *src, pmix_data_type_t type);

#if PMIX_HAVE_HWLOC
pmix_status_t pmix_state_print_topo(struct pmix_peer_t *pr,
                                    char **output, char *prefix,
                                    hwloc_topology_t src, pmix_data_type_t type);
#endif
pmix_status_t pmix_state_print_value(struct pmix_peer_t *pr,
                                     char **output, char *prefix,
                                     pmix_value_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_array(struct pmix_peer_t *pr,
                                     char **output, char *prefix,
                                     pmix_info_array_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_proc(struct pmix_peer_t *pr,
                                    char **output, char *prefix,
                                    pmix_proc_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_app(struct pmix_peer_t *pr,
                                   char **output, char *prefix,
                                   pmix_app_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_info(struct pmix_peer_t *pr,
                                    char **output, char *prefix,
                                    pmix_info_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_buf(struct pmix_peer_t *pr,
                                   char **output, char *prefix,
                                   pmix_buffer_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_kval(struct pmix_peer_t *pr,
                                    char **output, char *prefix,
                                    pmix_kval_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_modex(struct pmix_peer_t *pr,
                                     char **output, char *prefix,
                                     pmix_modex_data_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_persist(struct pmix_peer_t *pr,
                                       char **output, char *prefix,
                                       pmix_persistence_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_bo(struct pmix_peer_t *pr,
                                  char **output, char *prefix,
                                  pmix_byte_object_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_pdata(struct pmix_peer_t *pr,
                                     char **output, char *prefix,
                                     pmix_pdata_t *src, pmix_data_type_t type);
pmix_status_t pmix_state_print_ptr(struct pmix_peer_t *pr,
                                   char **output, char *prefix,
                                   void *src, pmix_data_type_t type);

/*
 * Common helper functions
 */

char* pmix_state_buffer_extend(pmix_buffer_t *bptr, size_t bytes_to_add);

bool pmix_state_too_small(pmix_buffer_t *buffer, size_t bytes_reqd);

pmix_state_type_info_t* pmix_state_find_type(pmix_data_type_t type);

pmix_status_t pmix_state_store_data_type(pmix_buffer_t *buffer, pmix_data_type_t type);

pmix_status_t pmix_state_get_data_type(pmix_buffer_t *buffer, pmix_data_type_t *type);

pmix_status_t pmix_state_base_copy_payload(struct pmix_peer_t *pr,
                                           pmix_buffer_t *dest,
                                           pmix_buffer_t *src);

void pmix_base_value_load(struct pmix_peer_t *pr,
                          pmix_value_t *v, void *data,
                          pmix_data_type_t type);

pmix_status_t pmix_base_value_unload(struct pmix_peer_t *pr,
                                     pmix_value_t *kv, void **data,
                                     size_t *sz, pmix_data_type_t type);

pmix_status_t pmix_base_value_xfer(struct pmix_peer_t *pr,
                                   pmix_value_t *p,
                                   pmix_value_t *src);

pmix_value_cmp_t pmix_base_value_cmp(struct pmix_peer_t *pr,
                                     pmix_value_t *p,
                                     pmix_value_t *p1);

 END_C_DECLS

#endif
