/*
 * Copyright (c) 2006-2012 Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2007      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2010      Sandia National Laboratories. All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 */

#include <src/include/ww_config.h>

#include "ww_types.h"
#include "src/mca/mca.h"
#include "src/mca/installdirs/installdirs.h"
#include "src/mca/installdirs/base/base.h"
#include "src/mca/installdirs/base/static-components.h"

ww_install_dirs_t ww_install_dirs = {0};

#define CONDITIONAL_COPY(target, origin, field)                 \
    do {                                                        \
        if (origin.field != NULL && target.field == NULL) {     \
            target.field = origin.field;                        \
        }                                                       \
    } while (0)

static int
ww_installdirs_base_open(mca_base_open_flag_t flags)
{
    mca_base_component_list_item_t *component_item;
    int ret;

    ret = mca_base_framework_components_open (&ww_installdirs_base_framework,
                                              flags);
    if (WW_SUCCESS != ret) {
        return ret;
    }

    WW_LIST_FOREACH(component_item, &ww_installdirs_base_framework.framework_components, mca_base_component_list_item_t) {
        const ww_installdirs_base_component_t *component =
            (const ww_installdirs_base_component_t *) component_item->cli_component;

        /* copy over the data, if something isn't already there */
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         prefix);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         exec_prefix);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         bindir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         sbindir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         libexecdir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         datarootdir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         datadir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         sysconfdir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         sharedstatedir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         localstatedir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         libdir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         includedir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         infodir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         mandir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         wwdatadir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         wwlibdir);
        CONDITIONAL_COPY(ww_install_dirs, component->install_dirs_data,
                         wwincludedir);
    }

    /* expand out all the fields */
    ww_install_dirs.prefix =
        ww_install_dirs_expand_setup(ww_install_dirs.prefix);
    ww_install_dirs.exec_prefix =
        ww_install_dirs_expand_setup(ww_install_dirs.exec_prefix);
    ww_install_dirs.bindir =
        ww_install_dirs_expand_setup(ww_install_dirs.bindir);
    ww_install_dirs.sbindir =
        ww_install_dirs_expand_setup(ww_install_dirs.sbindir);
    ww_install_dirs.libexecdir =
        ww_install_dirs_expand_setup(ww_install_dirs.libexecdir);
    ww_install_dirs.datarootdir =
        ww_install_dirs_expand_setup(ww_install_dirs.datarootdir);
    ww_install_dirs.datadir =
        ww_install_dirs_expand_setup(ww_install_dirs.datadir);
    ww_install_dirs.sysconfdir =
        ww_install_dirs_expand_setup(ww_install_dirs.sysconfdir);
    ww_install_dirs.sharedstatedir =
        ww_install_dirs_expand_setup(ww_install_dirs.sharedstatedir);
    ww_install_dirs.localstatedir =
        ww_install_dirs_expand_setup(ww_install_dirs.localstatedir);
    ww_install_dirs.libdir =
        ww_install_dirs_expand_setup(ww_install_dirs.libdir);
    ww_install_dirs.includedir =
        ww_install_dirs_expand_setup(ww_install_dirs.includedir);
    ww_install_dirs.infodir =
        ww_install_dirs_expand_setup(ww_install_dirs.infodir);
    ww_install_dirs.mandir =
        ww_install_dirs_expand_setup(ww_install_dirs.mandir);
    ww_install_dirs.wwdatadir =
        ww_install_dirs_expand_setup(ww_install_dirs.wwdatadir);
    ww_install_dirs.wwlibdir =
        ww_install_dirs_expand_setup(ww_install_dirs.wwlibdir);
    ww_install_dirs.wwincludedir =
        ww_install_dirs_expand_setup(ww_install_dirs.wwincludedir);

#if 0
    fprintf(stderr, "prefix:         %s\n", ww_install_dirs.prefix);
    fprintf(stderr, "exec_prefix:    %s\n", ww_install_dirs.exec_prefix);
    fprintf(stderr, "bindir:         %s\n", ww_install_dirs.bindir);
    fprintf(stderr, "sbindir:        %s\n", ww_install_dirs.sbindir);
    fprintf(stderr, "libexecdir:     %s\n", ww_install_dirs.libexecdir);
    fprintf(stderr, "datarootdir:    %s\n", ww_install_dirs.datarootdir);
    fprintf(stderr, "datadir:        %s\n", ww_install_dirs.datadir);
    fprintf(stderr, "sysconfdir:     %s\n", ww_install_dirs.sysconfdir);
    fprintf(stderr, "sharedstatedir: %s\n", ww_install_dirs.sharedstatedir);
    fprintf(stderr, "localstatedir:  %s\n", ww_install_dirs.localstatedir);
    fprintf(stderr, "libdir:         %s\n", ww_install_dirs.libdir);
    fprintf(stderr, "includedir:     %s\n", ww_install_dirs.includedir);
    fprintf(stderr, "infodir:        %s\n", ww_install_dirs.infodir);
    fprintf(stderr, "mandir:         %s\n", ww_install_dirs.mandir);
    fprintf(stderr, "pkgdatadir:     %s\n", ww_install_dirs.pkgdatadir);
    fprintf(stderr, "pkglibdir:      %s\n", ww_install_dirs.pkglibdir);
    fprintf(stderr, "pkgincludedir:  %s\n", ww_install_dirs.pkgincludedir);
#endif

    /* NTH: Is it ok not to close the components? If not we can add a flag
       to mca_base_framework_components_close to indicate not to deregister
       variable groups */
    return WW_SUCCESS;
}


static int
ww_installdirs_base_close(void)
{
    free(ww_install_dirs.prefix);
    free(ww_install_dirs.exec_prefix);
    free(ww_install_dirs.bindir);
    free(ww_install_dirs.sbindir);
    free(ww_install_dirs.libexecdir);
    free(ww_install_dirs.datarootdir);
    free(ww_install_dirs.datadir);
    free(ww_install_dirs.sysconfdir);
    free(ww_install_dirs.sharedstatedir);
    free(ww_install_dirs.localstatedir);
    free(ww_install_dirs.libdir);
    free(ww_install_dirs.includedir);
    free(ww_install_dirs.infodir);
    free(ww_install_dirs.mandir);
    free(ww_install_dirs.wwdatadir);
    free(ww_install_dirs.wwlibdir);
    free(ww_install_dirs.wwincludedir);
    memset (&ww_install_dirs, 0, sizeof (ww_install_dirs));

    return mca_base_framework_components_close (&ww_installdirs_base_framework, NULL);
}

/* Declare the installdirs framework */
MCA_BASE_FRAMEWORK_DECLARE(ww, installdirs, NULL, NULL, ww_installdirs_base_open,
                           ww_installdirs_base_close, mca_installdirs_base_static_components,
                           MCA_BASE_FRAMEWORK_FLAG_NOREGISTER | MCA_BASE_FRAMEWORK_FLAG_NO_DSO);
