/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2007-2008 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2015-2016 Intel, Inc.  All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/**
 * @file
 *
 * This interface is for security support. PMIx doesn't need much in
 * this regard, but we do need a mechanism for authenticating connections.
 *
 * Only *one* plugin will be active in a client, but multiple plugins may
 * be active in a server. Thus, this is a multi-select framework.
 *
 * Available plugins may be defined at runtime via the typical MCA parameter
 * syntax.
 */

#ifndef WW_SEC_H
#define WW_SEC_H

#include <src/include/ww_config.h>

#include "src/runtime/ww_rte.h"
#include "src/mca/mca.h"

BEGIN_C_DECLS

/******    MODULE DEFINITION    ******/

/**
 * Initialize the module. Returns an error if the module cannot
 * run, success if it can and wants to be used.
 */
typedef int (*ww_sec_base_module_init_fn_t)(void);

/**
 * Finalize the module. Tear down any allocated storage, disconnect
 * from any system support (e.g., LDAP server)
 */
typedef void (*ww_sec_base_module_fini_fn_t)(void);

/****    CLIENT-SIDE FUNCTIONS    ****/
/**
 * Create and return a string representation of a credential for this
 * client
 */
typedef char* (*ww_sec_base_module_create_cred_fn_t)(void);

/**
 * Perform the client-side handshake. Note that it is not required
 * (and indeed, would be rare) for a protocol to use both the
 * credential and handshake interfaces. It is acceptable, therefore,
 * for one of them to be NULL */
typedef ww_status_t (*ww_sec_base_module_client_hndshk_fn_t)(int sd);


/****    SERVER-SIDE FUNCTIONS    ****/
/**
 * Validate a client's credential
 */
typedef ww_status_t (*ww_sec_base_module_validate_cred_fn_t)(ww_peer_t *peer, char *cred);

/**
 * Perform the server-side handshake. Note that it is not required
 * (and indeed, would be rare) for a protocol to use both the
 * credential and handshake interfaces. It is acceptable, therefore,
 * for one of them to be NULL */
typedef ww_status_t (*ww_sec_base_module_server_hndshk_fn_t)(ww_peer_t *peer);

/**
 * Base structure for a SEC module
 */
typedef struct {
    char *name;
    /* init/finalize */
    ww_sec_base_module_init_fn_t           init;
    ww_sec_base_module_fini_fn_t           finalize;
    /** Client-side */
    ww_sec_base_module_create_cred_fn_t    create_cred;
    ww_sec_base_module_client_hndshk_fn_t  client_handshake;
    /** Server-side */
    ww_sec_base_module_validate_cred_fn_t  validate_cred;
    ww_sec_base_module_server_hndshk_fn_t  server_handshake;
} ww_sec_module_t;


/****    COMPONENT STRUCTURE DEFINITION    ****/

/* define a component-level API for initializing the component */
typedef ww_status_t (*ww_sec_base_component_init_fn_t)(void);

/* define a component-level API for finalizing the component */
typedef void (*ww_sec_base_component_finalize_fn_t)(void);

/* define a component-level API for getting a module */
typedef ww_sec_module_t* (*ww_sec_base_component_assign_module_fn_t)(void);

/*
 * the standard component data structure
 */
struct ww_sec_base_component_t {
    mca_base_component_t                            base;
    mca_base_component_data_t                       data;
    int                                             priority;
    ww_sec_base_component_init_fn_t               init;
    ww_sec_base_component_finalize_fn_t           finalize;
    ww_sec_base_component_assign_module_fn_t      assign_module;
};
typedef struct ww_sec_base_component_t ww_sec_base_component_t;


/*
 * Macro for use in components that are of type sec
 */
#define WW_SEC_BASE_VERSION_1_0_0 \
    WW_MCA_BASE_VERSION_1_0_0("sec", 1, 0, 0)

END_C_DECLS

#endif
