/*
 * Copyright (c) 2004-2010 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2015      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef WW_DL_BASE_H
#define WW_DL_BASE_H

#include <src/include/ww_config.h>
#include "src/mca/dl/dl.h"
#include "src/util/ww_environ.h"

#include "src/mca/base/base.h"


BEGIN_C_DECLS

/**
 * Globals
 */
WW_DECLSPEC extern mca_base_framework_t ww_dl_base_framework;
WW_DECLSPEC extern ww_dl_base_component_t
*ww_dl_base_selected_component;
WW_DECLSPEC extern ww_dl_base_module_t *ww_dl;


/**
 * Initialize the DL MCA framework
 *
 * @retval WW_SUCCESS Upon success
 * @retval WW_ERROR   Upon failures
 *
 * This function is invoked during ww_init();
 */
WW_DECLSPEC int ww_dl_base_open(mca_base_open_flag_t flags);

/**
 * Select an available component.
 *
 * @retval WW_SUCCESS Upon Success
 * @retval WW_NOT_FOUND If no component can be selected
 * @retval WW_ERROR Upon other failure
 *
 */
WW_DECLSPEC int ww_dl_base_select(void);

/**
 * Finalize the DL MCA framework
 *
 * @retval WW_SUCCESS Upon success
 * @retval WW_ERROR   Upon failures
 *
 * This function is invoked during ww_finalize();
 */
WW_DECLSPEC int ww_dl_base_close(void);

/**
 * Open a DSO
 *
 * (see ww_dl_base_module_open_ft_t in ww/mca/dl/dl.h for
 * documentation of this function)
 */
WW_DECLSPEC int ww_dl_open(const char *fname,
                               bool use_ext, bool private_namespace,
                               ww_dl_handle_t **handle, char **err_msg);

/**
 * Lookup a symbol in a DSO
 *
 * (see ww_dl_base_module_lookup_ft_t in ww/mca/dl/dl.h for
 * documentation of this function)
 */
WW_DECLSPEC int ww_dl_lookup(ww_dl_handle_t *handle,
                                 const char *symbol,
                                 void **ptr, char **err_msg);

/**
 * Close a DSO
 *
 * (see ww_dl_base_module_close_ft_t in ww/mca/dl/dl.h for
 * documentation of this function)
 */
WW_DECLSPEC int ww_dl_close(ww_dl_handle_t *handle);

/**
 * Iterate over files in a path
 *
 * (see ww_dl_base_module_foreachfile_ft_t in ww/mca/dl/dl.h for
 * documentation of this function)
 */
WW_DECLSPEC int ww_dl_foreachfile(const char *search_path,
                                      int (*cb_func)(const char *filename,
                                                     void *context),
                                      void *context);

END_C_DECLS

#endif /* WW_DL_BASE_H */
