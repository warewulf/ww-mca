/*
 * Copyright (c) 2006-2007 Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>

#include "src/mca/installdirs/installdirs.h"
#include "src/mca/installdirs/config/install_dirs.h"

const ww_installdirs_base_component_t mca_installdirs_config_component = {
    /* First, the mca_component_t struct containing meta information
       about the component itself */
    {
        WW_INSTALLDIRS_BASE_VERSION_1_0_0,

        /* Component name and version */
        "config",
        WW_MAJOR_VERSION,
        WW_MINOR_VERSION,
        WW_RELEASE_VERSION,

        /* Component open and close functions */
        NULL,
        NULL
    },
    {
        /* This component is Checkpointable */
        MCA_BASE_METADATA_PARAM_CHECKPOINT
    },

    {
        WW_INSTALL_PREFIX,
        WW_EXEC_PREFIX,
        WW_BINDIR,
        WW_SBINDIR,
        WW_LIBEXECDIR,
        WW_DATAROOTDIR,
        WW_DATADIR,
        WW_SYSCONFDIR,
        WW_SHAREDSTATEDIR,
        WW_LOCALSTATEDIR,
        WW_LIBDIR,
        WW_INCLUDEDIR,
        WW_INFODIR,
        WW_MANDIR,
        WW_PKGDATADIR,
        WW_PKGLIBDIR,
        WW_PKGINCLUDEDIR
    }
};
