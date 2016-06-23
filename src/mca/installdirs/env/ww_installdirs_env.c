/*
 * Copyright (c) 2006-2007 Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2007      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>

#include <stdlib.h>
#include <string.h>

#include "ww_types.h"
#include "src/mca/installdirs/installdirs.h"

static int installdirs_env_open(void);


ww_installdirs_base_component_t mca_installdirs_env_component = {
    /* First, the mca_component_t struct containing meta information
       about the component itself */
    {
        WW_INSTALLDIRS_BASE_VERSION_1_0_0,

        /* Component name and version */
        "env",
        WW_MAJOR_VERSION,
        WW_MINOR_VERSION,
        WW_RELEASE_VERSION,

        /* Component open and close functions */
        installdirs_env_open,
        NULL
    },
    {
        /* This component is checkpointable */
        MCA_BASE_METADATA_PARAM_CHECKPOINT
    },

    /* Next the ww_install_dirs_t install_dirs_data information */
    {
        NULL,
    },
};


#define SET_FIELD(field, envname)                                         \
    do {                                                                  \
        char *tmp = getenv(envname);                                      \
         if (NULL != tmp && 0 == strlen(tmp)) {                           \
             tmp = NULL;                                                  \
         }                                                                \
         mca_installdirs_env_component.install_dirs_data.field = tmp;     \
    } while (0)


static int
installdirs_env_open(void)
{
    SET_FIELD(prefix, "WW_INSTALL_PREFIX");
    SET_FIELD(exec_prefix, "WW_EXEC_PREFIX");
    SET_FIELD(bindir, "WW_BINDIR");
    SET_FIELD(sbindir, "WW_SBINDIR");
    SET_FIELD(libexecdir, "WW_LIBEXECDIR");
    SET_FIELD(datarootdir, "WW_DATAROOTDIR");
    SET_FIELD(datadir, "WW_DATADIR");
    SET_FIELD(sysconfdir, "WW_SYSCONFDIR");
    SET_FIELD(sharedstatedir, "WW_SHAREDSTATEDIR");
    SET_FIELD(localstatedir, "WW_LOCALSTATEDIR");
    SET_FIELD(libdir, "WW_LIBDIR");
    SET_FIELD(includedir, "WW_INCLUDEDIR");
    SET_FIELD(infodir, "WW_INFODIR");
    SET_FIELD(mandir, "WW_MANDIR");
    SET_FIELD(wwdatadir, "WW_PKGDATADIR");
    SET_FIELD(wwlibdir, "WW_PKGLIBDIR");
    SET_FIELD(wwincludedir, "WW_PKGINCLUDEDIR");

    return WW_SUCCESS;
}
