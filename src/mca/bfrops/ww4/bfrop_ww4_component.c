/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2008 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennbfropsee and The University
 *                         of Tennbfropsee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2015      Los Alamos National Security, LLC. All rights
 *                         reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 * These symbols are in a file by themselves to provide nice linker
 * semantics.  Since linkers generally pull in symbols by object
 * files, keeping these symbols as the only symbols in this file
 * prevents utility programs such as "ompi_info" from having to import
 * entire components just to query their version and parameters.
 */

#include <src/include/ww_config.h>
#include <ww_types.h>

#include "src/mca/bfrops/base/base.h"
#include "src/mca/bfrops/ww4/bfrop_ww4.h"

extern ww_bfrops_module_t ww_bfrops_ww11_module;

static ww_status_t component_open(void);
static ww_status_t component_query(mca_base_module_t **module, int *priority);
static ww_status_t component_close(void);
static ww_status_t component_register_type(const char *name, ww_data_type_t type,
                                             ww_bfrop_pack_fn_t pack,
                                             ww_bfrop_unpack_fn_t unpack,
                                             ww_bfrop_copy_fn_t copy,
                                             ww_bfrop_print_fn_t print);
static ww_bfrops_module_t* assign_module(const char *version);

/*
 * Instantiate the public struct with all of our public information
 * and pointers to our public functions in it
 */
ww_bfrops_base_component_t mca_bfrops_ww11_component = {
    .base = {
        WW_BFROPS_BASE_VERSION_1_0_0,

        /* Component name and version */
        .mca_component_name = "ww4",
        MCA_BASE_MAKE_VERSION(component, WW_MAJOR_VERSION, WW_MINOR_VERSION,
                              WW_RELEASE_VERSION),

        /* Component open and close functions */
        .mca_open_component = component_open,
        .mca_close_component = component_close,
        .mca_query_component = component_query,
    },
    .priority = 5,
    .register_type = component_register_type,
    .assign_module = assign_module
};


ww_status_t component_open(void)
{
    /* setup the types array */
    WW_CONSTRUCT(&mca_bfrops_ww11_component.types, ww_pointer_array_t);

    /* Register all the supported types */
    WW_REGISTER_TYPE("WW_BOOL", WW_BOOL,
                       ww_bfrop_pack_bool,
                       ww_bfrop_unpack_bool,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_bool,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_BYTE", WW_BYTE,
                       ww_bfrop_pack_byte,
                       ww_bfrop_unpack_byte,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_byte,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_STRING", WW_STRING,
                       ww_bfrop_pack_string,
                       ww_bfrop_unpack_string,
                       ww_bfrop_copy_string,
                       ww_bfrop_print_string,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_SIZE", WW_SIZE,
                       ww_bfrop_pack_sizet,
                       ww_bfrop_unpack_sizet,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_size,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_PID", WW_PID,
                       ww_bfrop_pack_pid,
                       ww_bfrop_unpack_pid,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_pid,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_INT", WW_INT,
                       ww_bfrop_pack_int,
                       ww_bfrop_unpack_int,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_int,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_INT8", WW_INT8,
                       ww_bfrop_pack_byte,
                       ww_bfrop_unpack_byte,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_int8,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_INT16", WW_INT16,
                       ww_bfrop_pack_int16,
                       ww_bfrop_unpack_int16,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_int16,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_INT32", WW_INT32,
                       ww_bfrop_pack_int32,
                       ww_bfrop_unpack_int32,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_int32,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_INT64", WW_INT64,
                       ww_bfrop_pack_int64,
                       ww_bfrop_unpack_int64,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_int64,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_UINT", WW_UINT,
                       ww_bfrop_pack_int,
                       ww_bfrop_unpack_int,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_uint,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_UINT8", WW_UINT8,
                       ww_bfrop_pack_byte,
                       ww_bfrop_unpack_byte,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_uint8,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_UINT16", WW_UINT16,
                       ww_bfrop_pack_int16,
                       ww_bfrop_unpack_int16,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_uint16,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_UINT32", WW_UINT32,
                       ww_bfrop_pack_int32,
                       ww_bfrop_unpack_int32,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_uint32,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_UINT64", WW_UINT64,
                       ww_bfrop_pack_int64,
                       ww_bfrop_unpack_int64,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_uint64,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_FLOAT", WW_FLOAT,
                       ww_bfrop_pack_float,
                       ww_bfrop_unpack_float,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_float,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_DOUBLE", WW_DOUBLE,
                       ww_bfrop_pack_double,
                       ww_bfrop_unpack_double,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_double,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_TIMEVAL", WW_TIMEVAL,
                       ww_bfrop_pack_timeval,
                       ww_bfrop_unpack_timeval,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_timeval,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_TIME", WW_TIME,
                       ww_bfrop_pack_time,
                       ww_bfrop_unpack_time,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_time,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_STATUS", WW_STATUS,
                       ww_bfrop_pack_status,
                       ww_bfrop_unpack_status,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_status,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_VALUE", WW_VALUE,
                       ww_bfrop_pack_value,
                       ww_bfrop_unpack_value,
                       ww_bfrop_copy_value,
                       ww_bfrop_print_value,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_INFO_ARRAY", WW_INFO_ARRAY,
                       ww_bfrop_pack_array,
                       ww_bfrop_unpack_array,
                       ww_bfrop_copy_array,
                       ww_bfrop_print_array,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_PROC", WW_PROC,
                       ww_bfrop_pack_proc,
                       ww_bfrop_unpack_proc,
                       ww_bfrop_copy_proc,
                       ww_bfrop_print_proc,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_INFO", WW_INFO,
                       ww_bfrop_pack_info,
                       ww_bfrop_unpack_info,
                       ww_bfrop_copy_info,
                       ww_bfrop_print_info,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_BUFFER", WW_BUFFER,
                       ww_bfrop_pack_buf,
                       ww_bfrop_unpack_buf,
                       ww_bfrop_copy_buf,
                       ww_bfrop_print_buf,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_BYTE_OBJECT", WW_BYTE_OBJECT,
                       ww_bfrop_pack_bo,
                       ww_bfrop_unpack_bo,
                       ww_bfrop_copy_bo,
                       ww_bfrop_print_bo,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_KVAL", WW_KVAL,
                       ww_bfrop_pack_kval,
                       ww_bfrop_unpack_kval,
                       ww_bfrop_copy_kval,
                       ww_bfrop_print_kval,
                       &mca_bfrops_ww11_component.types);

    WW_REGISTER_TYPE("WW_POINTER", WW_POINTER,
                       ww_bfrop_pack_ptr,
                       ww_bfrop_unpack_ptr,
                       ww_bfrop_std_copy,
                       ww_bfrop_print_ptr,
                       &mca_bfrops_ww11_component.types);

    return WW_SUCCESS;
}


ww_status_t component_query(mca_base_module_t **module, int *priority)
{

    *priority = mca_bfrops_ww11_component.priority;
    *module = (mca_base_module_t *)&ww_bfrops_ww11_module;
    return WW_SUCCESS;
}


ww_status_t component_close(void)
{
    return WW_SUCCESS;
}

static ww_status_t component_register_type(const char *name, ww_data_type_t type,
                                             ww_bfrop_pack_fn_t pack,
                                             ww_bfrop_unpack_fn_t unpack,
                                             ww_bfrop_copy_fn_t copy,
                                             ww_bfrop_print_fn_t print)
{
    return WW_SUCCESS;
}

static ww_bfrops_module_t* assign_module(const char *version)
{
    return NULL;
}
