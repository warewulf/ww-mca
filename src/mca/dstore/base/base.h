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
#ifndef WW_DSTORE_BASE_H_
#define WW_DSTORE_BASE_H_

#include <src/include/ww_config.h>


#ifdef HAVE_SYS_TIME_H
#include <sys/time.h> /* for struct timeval */
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "src/mca/mca.h"
#include "src/mca/base/mca_base_framework.h"

#include "src/mca/dstore/dstore.h"


 BEGIN_C_DECLS

/*
 * MCA Framework
 */
 WW_DECLSPEC extern mca_base_framework_t ww_dstore_base_framework;
/*
 * Cycle across available components and construct the list
 * of active modules
 */
 WW_DECLSPEC ww_status_t ww_dstore_base_select(void);

/**
 * Track an active component / module
 */
 struct ww_dstore_base_active_module_t {
    ww_list_item_t super;
    ww_status_t pri;
    ww_dstore_module_t *module;
    ww_dstore_base_component_t *component;
};
typedef struct ww_dstore_base_active_module_t ww_dstore_base_active_module_t;
WW_CLASS_DECLARATION(ww_dstore_base_active_module_t);


/* framework globals */
struct ww_dstore_globals_t {
  ww_list_t actives;
  bool initialized;
};
typedef struct ww_dstore_globals_t ww_dstore_globals_t;

extern ww_dstore_globals_t ww_dstore_globals;

ww_configuration_t* ww_dstore_base_load(char *name, ww_list_t *directives);

ww_status_t ww_dstore_base_commit(ww_configuration_t *config,
                                  ww_list_t *directives);

ww_status_t ww_dstore_base_find(ww_configuration_t *config,
                                char *key, char *type,
                                ww_list_t *directives,
                                ww_list_t *results);

ww_status_t ww_dstore_base_set(ww_building_block_t *block,
                               ww_kval_t *kv, ww_list_t *directives);

ww_value_cmp_t ww_dstore_base_compare(ww_configuration_t *config1,
                                      ww_configuration_t *config2,
                                      ww_list_t *directives);

END_C_DECLS

#endif
