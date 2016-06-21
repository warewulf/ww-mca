/*
 * Copyright (c) 2004-2010 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2011 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2010-2011 Oak Ridge National Labs.  All rights reserved.
 * Copyright (c) 2011-2014 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2011-2013 Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2013-2016 Intel, Inc.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 */

#include <src/include/ww_config.h>

#include "src/mca/bfrops/base/base.h"
#include "bfrop_ww4.h"

ww_bfrops_module_t ww_bfrops_ww11_module = {
    .pack = ww_bfrop_base_pack,
    .unpack = ww_bfrop_base_unpack,
    .copy = ww_bfrop_base_copy,
    .print = ww_bfrop_base_print,
    .copy_payload = ww_bfrop_base_copy_payload,
    .value_xfer = ww_base_value_xfer,
    .value_load = ww_base_value_load,
    .value_unload = ww_base_value_unload,
    .value_cmp = ww_base_value_cmp
};
