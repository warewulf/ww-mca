/* -*- C -*-
 *
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2012      Los Alamos National Security, Inc.  All rights reserved.
 * Copyright (c) 2014-2016 Intel, Inc. All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 */
#ifndef WW_SEC_BASE_H_
#define WW_SEC_BASE_H_

#include <src/include/ww_config.h>


#ifdef HAVE_SYS_TIME_H
#include <sys/time.h> /* for struct timeval */
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "src/class/ww_pointer_array.h"
#include "src/mca/mca.h"
#include "src/mca/base/mca_base_framework.h"

#include "src/mca/sec/sec.h"


 BEGIN_C_DECLS

/*
 * MCA Framework
 */
WW_DECLSPEC extern mca_base_framework_t ww_sec_base_framework;
/**
 * BFROP select function
 *
 * Cycle across available components and construct the list
 * of active modules
 */
WW_DECLSPEC ww_status_t ww_sec_base_select(void);

/* Select a sec module for a given peer */
WW_DECLSPEC ww_sec_module_t* ww_sec_base_assign_module(const char *options);

/* get a list of available options - caller must free results
 * when done */
WW_DECLSPEC char* ww_sec_base_get_available_modules(void);

/**
 * Track an active component / module
 */
struct ww_sec_base_active_module_t {
    ww_list_item_t super;
    int pri;
    ww_sec_module_t *module;
    ww_sec_base_component_t *component;
};
typedef struct ww_sec_base_active_module_t ww_sec_base_active_module_t;
WW_CLASS_DECLARATION(ww_sec_base_active_module_t);


/* framework globals */
struct ww_sec_globals_t {
  ww_list_t actives;
  bool initialized;
};
typedef struct ww_sec_globals_t ww_sec_globals_t;

extern ww_sec_globals_t ww_sec_globals;

 END_C_DECLS

#endif
