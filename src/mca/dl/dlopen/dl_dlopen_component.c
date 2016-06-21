/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2015      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2015      Los Alamos National Security, LLC. All rights
 *                         reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>

#include "ww_types.h"
#include "src/mca/dl/dl.h"
#include "src/util/argv.h"

#include "dl_dlopen.h"


/*
 * Public string showing the sysinfo ompi_linux component version number
 */
const char *ww_dl_dlopen_component_version_string =
    "WW dl dlopen MCA component version " WW_VERSION;


/*
 * Local functions
 */
static int dlopen_component_register(void);
static int dlopen_component_open(void);
static int dlopen_component_close(void);
static int dlopen_component_query(mca_base_module_t **module, int *priority);

/*
 * Instantiate the public struct with all of our public information
 * and pointers to our public functions in it
 */

ww_dl_dlopen_component_t mca_dl_dlopen_component = {

    /* Fill in the mca_dl_base_component_t */
    .base = {

        /* First, the mca_component_t struct containing meta information
           about the component itself */
        .base_version = {
            WW_DL_BASE_VERSION_1_0_0,

            /* Component name and version */
            .mca_component_name = "dlopen",
            MCA_BASE_MAKE_VERSION(component, WW_MAJOR_VERSION, WW_MINOR_VERSION,
                                  WW_RELEASE_VERSION),

            /* Component functions */
            .mca_register_component_params = dlopen_component_register,
            .mca_open_component = dlopen_component_open,
            .mca_close_component = dlopen_component_close,
            .mca_query_component = dlopen_component_query,
        },

        .base_data = {
            /* The component is checkpoint ready */
            MCA_BASE_METADATA_PARAM_CHECKPOINT
        },

        /* The dl framework members */
        .priority = 80
    },
};


static int dlopen_component_register(void)
{
    int ret;

    mca_dl_dlopen_component.filename_suffixes_mca_storage = ".so,.dylib,.dll,.sl";
    ret =
        mca_base_component_var_register(&mca_dl_dlopen_component.base.base_version,
                                        "filename_suffixes",
                                        "Comma-delimited list of filename suffixes that the dlopen component will try",
                                        MCA_BASE_VAR_TYPE_STRING,
                                        NULL,
                                        0,
                                        MCA_BASE_VAR_FLAG_SETTABLE,
                                        WW_INFO_LVL_5,
                                        MCA_BASE_VAR_SCOPE_LOCAL,
                                        &mca_dl_dlopen_component.filename_suffixes_mca_storage);
    if (ret < 0) {
        return ret;
    }
    mca_dl_dlopen_component.filename_suffixes =
        ww_argv_split(mca_dl_dlopen_component.filename_suffixes_mca_storage,
                        ',');

    return WW_SUCCESS;
}

static int dlopen_component_open(void)
{
    return WW_SUCCESS;
}


static int dlopen_component_close(void)
{
    if (NULL != mca_dl_dlopen_component.filename_suffixes) {
        ww_argv_free(mca_dl_dlopen_component.filename_suffixes);
        mca_dl_dlopen_component.filename_suffixes = NULL;
    }

    return WW_SUCCESS;
}


static int dlopen_component_query(mca_base_module_t **module, int *priority)
{
    /* The priority value is somewhat meaningless here; by
       ww/mca/dl/configure.m4, there's at most one component
       available. */
    *priority = mca_dl_dlopen_component.base.priority;
    *module = &ww_dl_dlopen_module.super;

    return WW_SUCCESS;
}
