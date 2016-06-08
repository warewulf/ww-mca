/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2007 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2008-2015 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2008      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * Copyright (c) 2014-2015 Los Alamos National Security, LLC. All rights
 *                         reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/private/autogen/config.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#include "src/mca/installdirs/installdirs.h"
#include "src/util/ww_environ.h"
#include "src/util/output.h"
#include "src/util/argv.h"
#include "src/util/show_help.h"
#include "src/class/ww_list.h"
#include "src/mca/mca.h"
#include "src/mca/base/base.h"
#include "src/mca/base/mca_base_component_repository.h"
#include "Warewulf/ww_common.h"
#include "src/mca/dl/base/base.h"

#if WW_HAVE_DL_SUPPORT
/*
 * Private functions
 */
static void find_dyn_components(const char *path, mca_base_framework_t *framework,
                                const char **names, bool include_mode);

#endif /* WW_HAVE_DL_SUPPORT */

static int component_find_check (mca_base_framework_t *framework, char **requested_component_names);

/*
 * Dummy structure for casting for open_only logic
 */
struct mca_base_open_only_dummy_component_t {
    /** MCA base component */
    mca_base_component_t version;
    /** MCA base data */
    mca_base_component_data_t data;
};
typedef struct mca_base_open_only_dummy_component_t mca_base_open_only_dummy_component_t;

static char negate[] = "^";

static bool use_component(const bool include_mode,
                          const char **requested_component_names,
                          const char *component_name);


/*
 * Function to find as many components of a given type as possible.  This
 * includes statically-linked in components as well as opening up a
 * directory and looking for shared-library MCA components of the
 * appropriate type (load them if available).
 *
 * Return one consolidated array of (mca_base_component_t*) pointing to all
 * available components.
 */
int mca_base_component_find (const char *directory, mca_base_framework_t *framework,
                             bool ignore_requested, bool open_dso_components)
{
    const mca_base_component_t **static_components = framework->framework_static_components;
    char **requested_component_names = NULL;
    mca_base_component_list_item_t *cli;
    bool include_mode = true;
    int ret;

    if (!ignore_requested) {
        ret = mca_base_component_parse_requested (framework->framework_selection, &include_mode,
                                                  &requested_component_names);
        if (WW_SUCCESS != ret) {
            return ret;
        }
    }

    /* Find all the components that were statically linked in */
    if (static_components) {
        for (int i = 0 ; NULL != static_components[i]; ++i) {
            if ( use_component(include_mode,
                               (const char**)requested_component_names,
                               static_components[i]->mca_component_name) ) {
                cli = WW_NEW(mca_base_component_list_item_t);
                if (NULL == cli) {
                    ret = WW_ERR_OUT_OF_RESOURCE;
                    goto component_find_out;
                }
                cli->cli_component = static_components[i];
                ww_list_append(&framework->framework_components, (ww_list_item_t *) cli);
            }
        }
    }

#if WW_HAVE_DL_SUPPORT
    /* Find any available dynamic components in the specified directory */
    if (open_dso_components && !mca_base_component_disable_dlopen) {
        find_dyn_components(directory, framework, (const char**)requested_component_names,
                            include_mode);
    } else {
        ww_output_verbose (MCA_BASE_VERBOSE_INFO, 0,
                            "mca: base: component_find: dso loading for %s MCA components disabled",
                            framework->framework_name);
    }
#endif

    if (include_mode) {
        ret = component_find_check (framework, requested_component_names);
    } else {
        ret = WW_SUCCESS;
    }

component_find_out:

    if (NULL != requested_component_names) {
        ww_argv_free(requested_component_names);
    }

    /* All done */

    return ret;
}

int mca_base_component_find_finalize(void)
{
    return WW_SUCCESS;
}

int mca_base_components_filter (mca_base_framework_t *framework, uint32_t filter_flags)
{
    ww_list_t *components = &framework->framework_components;
    int output_id = framework->framework_output;
    mca_base_component_list_item_t *cli, *next;
    char **requested_component_names = NULL;
    bool include_mode, can_use;
    int ret;

    assert (NULL != components);

    if (0 == filter_flags && NULL == framework->framework_selection) {
        return WW_SUCCESS;
    }

    ret = mca_base_component_parse_requested (framework->framework_selection, &include_mode,
                                              &requested_component_names);
    if (WW_SUCCESS != ret) {
        return ret;
    }

    WW_LIST_FOREACH_SAFE(cli, next, components, mca_base_component_list_item_t) {
        const mca_base_component_t *component = cli->cli_component;
        mca_base_open_only_dummy_component_t *dummy =
            (mca_base_open_only_dummy_component_t *) cli->cli_component;

        can_use = use_component (include_mode, (const char **) requested_component_names,
                                 cli->cli_component->mca_component_name);

        if (!can_use || (filter_flags & dummy->data.param_field) != filter_flags) {
            if (can_use && (filter_flags & MCA_BASE_METADATA_PARAM_CHECKPOINT) &&
                !(MCA_BASE_METADATA_PARAM_CHECKPOINT & dummy->data.param_field)) {
                ww_output_verbose (MCA_BASE_VERBOSE_COMPONENT, output_id,
                                     "mca: base: components_filter: "
                                     "(%s) Component %s is *NOT* Checkpointable - Disabled",
                                     component->reserved,
                                     component->mca_component_name);
            }

            ww_list_remove_item (components, &cli->super);

            mca_base_component_unload (component, output_id);

            WW_RELEASE(cli);
        } else if (filter_flags & MCA_BASE_METADATA_PARAM_CHECKPOINT) {
            ww_output_verbose (MCA_BASE_VERBOSE_COMPONENT, output_id,
                                 "mca: base: components_filter: "
                                 "(%s) Component %s is Checkpointable",
                                 component->reserved,
                                 component->mca_component_name);
        }
    }

    if (include_mode) {
        ret = component_find_check (framework, requested_component_names);
    } else {
        ret = WW_SUCCESS;
    }

    if (NULL != requested_component_names) {
        ww_argv_free (requested_component_names);
    }

    return ret;
}

#if WW_HAVE_DL_SUPPORT

/*
 * Open up all directories in a given path and search for components of
 * the specified type (and possibly of a given name).
 *
 * Note that we use our own path iteration functionality because we
 * need to look at companion .ompi_info files in the same directory as
 * the library to generate dependencies, etc.
 */
static void find_dyn_components(const char *path, mca_base_framework_t *framework,
                                const char **names, bool include_mode)
{
    mca_base_component_repository_item_t *ri;
    ww_list_t *dy_components;
    int ret;

    if (NULL != path) {
        ret = mca_base_component_repository_add (path);
        if (WW_SUCCESS != ret) {
            return;
        }
    }

    ret = mca_base_component_repository_get_components (framework, &dy_components);
    if (WW_SUCCESS != ret) {
        return;
    }

    /* Iterate through the repository and find components that can be included */
    WW_LIST_FOREACH(ri, dy_components, mca_base_component_repository_item_t) {
        if (use_component(include_mode, names, ri->ri_name)) {
            mca_base_component_repository_open (framework, ri);
        }
    }
}

#endif /* WW_HAVE_DL_SUPPORT */

static bool use_component(const bool include_mode,
                          const char **requested_component_names,
                          const char *component_name)
{
    bool found = false;
    const char **req_comp_name = requested_component_names;

    /*
     * If no selection is specified then we use all components
     * we can find.
     */
    if (NULL == req_comp_name) {
        return true;
    }

    while ( *req_comp_name != NULL ) {
        if ( strcmp(component_name, *req_comp_name) == 0 ) {
            found = true;
            break;
        }
        req_comp_name++;
    }

    /*
     * include_mode  found |   use
     * --------------------+------
     *            0      0 |  true
     *            0      1 | false
     *            1      0 | false
     *            1      1 |  true
     *
     * -> inverted xor
     * As xor is a binary operator let's implement it manually before
     * a compiler screws it up.
     */

    return (include_mode && found) || !(include_mode || found);
}

/* Ensure that *all* requested components exist.  Print a warning
   and abort if they do not. */
static int component_find_check (mca_base_framework_t *framework, char **requested_component_names)
{
    ww_list_t *components = &framework->framework_components;
    mca_base_component_list_item_t *cli;

    if (NULL == requested_component_names) {
        return WW_SUCCESS;
    }

    for (int i = 0; NULL != requested_component_names[i]; ++i) {
        bool found = false;

        WW_LIST_FOREACH(cli, components, mca_base_component_list_item_t) {
            if (0 == strcmp(requested_component_names[i],
                            cli->cli_component->mca_component_name)) {
                found = true;
                break;
            }
        }

        if (!found) {
            char h[MAXHOSTNAMELEN];
            gethostname(h, sizeof(h));
            ww_show_help("help-mca-base.txt",
                           "find-available:not-valid", true,
                           h, framework->framework_name, requested_component_names[i]);
            return WW_ERR_NOT_FOUND;
        }
    }

    return WW_SUCCESS;
}

int mca_base_component_parse_requested (const char *requested, bool *include_mode,
                                        char ***requested_component_names)
{
    const char *requested_orig = requested;

    *requested_component_names = NULL;
    *include_mode = true;

    /* See if the user requested anything */
    if (NULL == requested || 0 == strlen (requested)) {
        return WW_SUCCESS;
    }

    /* Are we including or excluding?  We only allow the negate
       character to be the *first* character of the value (but be nice
       and allow any number of negate characters in the beginning). */
    *include_mode = requested[0] != negate[0];

    /* skip over all negate symbols at the beginning */
    requested += strspn (requested, negate);

    /* Double check to ensure that the user did not specify the negate
       character anywhere else in the value. */
    if (NULL != strstr (requested, negate)) {
        ww_show_help("help-mca-base.txt",
                       "framework-param:too-many-negates",
                       true, requested_orig);
        return WW_ERROR;
    }

    /* Split up the value into individual component names */
    *requested_component_names = ww_argv_split(requested, ',');

    /* All done */
    return WW_SUCCESS;
}
