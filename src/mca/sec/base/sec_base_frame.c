/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2009 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2012-2013 Los Alamos National Security, Inc.  All rights reserved.
 * Copyright (c) 2014-2016 Intel, Inc. All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
/** @file:
 *
 */
#include <src/include/ww_config.h>

#include <ww_types.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "src/class/ww_list.h"
#include "src/mca/sec/base/base.h"

/*
 * The following file was created by configure.  It contains extern
 * statements and the definition of an array of pointers to each
 * component's public mca_base_component_t struct.
 */

#include "src/mca/sec/base/static-components.h"

/* Instantiate the global vars */
ww_sec_globals_t ww_sec_globals;

static ww_status_t ww_sec_close(void)
{
  ww_sec_base_active_module_t *active;

    if (!ww_sec_globals.initialized) {
        return WW_SUCCESS;
    }
    ww_sec_globals.initialized = false;

    WW_LIST_FOREACH(active, &ww_sec_globals.actives, ww_sec_base_active_module_t) {
      if (NULL != active->component->finalize) {
        active->component->finalize();
      }
    }
    WW_DESTRUCT(&ww_sec_globals.actives);

    return WW_SUCCESS;
}

static ww_status_t ww_sec_open(mca_base_open_flag_t flags)
{
  /* initialize globals */
  ww_sec_globals.initialized = true;
  WW_CONSTRUCT(&ww_sec_globals.actives, ww_list_t);
  return WW_SUCCESS;
}

MCA_BASE_FRAMEWORK_DECLARE(ww, sec, "Warewulf Security Operations",
                           NULL, ww_sec_open, ww_sec_close,
                           mca_sec_base_static_components, 0);

WW_CLASS_INSTANCE(ww_sec_base_active_module_t,
                    ww_list_item_t,
                    NULL, NULL);

