/*
 * Copyright (c) 2004-2010 The Trustees of Indiana University.
 *                         All rights reserved.
 * Copyright (c) 2015      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/**
 * This file is a simple set of wrappers around the selected OPAL DL
 * component (it's a compile-time framework with, at most, a single
 * component; see dl.h for details).
 */

#include <src/include/ww_config.h>

#include "ww_types.h"

#include "src/mca/dl/base/base.h"


int ww_dl_open(const char *fname,
                 bool use_ext, bool private_namespace,
                 ww_dl_handle_t **handle, char **err_msg)
{
    *handle = NULL;

    if (NULL != ww_dl && NULL != ww_dl->open) {
        return ww_dl->open(fname, use_ext, private_namespace,
                             handle, err_msg);
    }

    return WW_ERR_NOT_SUPPORTED;
}

int ww_dl_lookup(ww_dl_handle_t *handle,
                   const char *symbol,
                   void **ptr, char **err_msg)
{
    if (NULL != ww_dl && NULL != ww_dl->lookup) {
        return ww_dl->lookup(handle, symbol, ptr, err_msg);
    }

    return WW_ERR_NOT_SUPPORTED;
}

int ww_dl_close(ww_dl_handle_t *handle)
{
    if (NULL != ww_dl && NULL != ww_dl->close) {
        return ww_dl->close(handle);
    }

    return WW_ERR_NOT_SUPPORTED;
}

int ww_dl_foreachfile(const char *search_path,
                        int (*cb_func)(const char *filename, void *context),
                        void *context)
{
    if (NULL != ww_dl && NULL != ww_dl->foreachfile) {
        return ww_dl->foreachfile(search_path, cb_func, context);
    }

    return WW_ERR_NOT_SUPPORTED;
}
