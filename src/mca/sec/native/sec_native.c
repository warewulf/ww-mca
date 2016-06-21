/*
 * Copyright (c) 2015      Intel, Inc.  All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>

#include <ww_types.h>

#include "src/runtime/ww_rte.h"
#include "src/util/argv.h"
#include "src/util/error.h"
#include "src/util/output.h"

#include <unistd.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include "src/mca/sec/sec.h"
#include "sec_native.h"

static int native_init(void);
static void native_finalize(void);
static char* create_cred(void);
static int validate_cred(ww_peer_t *peer, char *cred);

ww_sec_module_t ww_native_module = {
    "native",
    native_init,
    native_finalize,
    create_cred,
    NULL,
    validate_cred,
    NULL
};

static int native_init(void)
{
    ww_output_verbose(2, ww_globals.debug_output,
                        "sec: native init");
    return WW_SUCCESS;
}

static void native_finalize(void)
{
    ww_output_verbose(2, ww_globals.debug_output,
                        "sec: native finalize");
}

static char* create_cred(void)
{
    char *cred;

    ww_output_verbose(2, ww_globals.debug_output,
                        "sec: native create_cred");

    /* print them and return the string */
    if (0 > asprintf(&cred, "%lu:%lu", (unsigned long)ww_globals.uid,
                    (unsigned long)ww_globals.gid)) {
        return NULL;
    }

    ww_output_verbose(2, ww_globals.debug_output,
                        "sec: using credential %s", cred);

    return cred;
}

static int validate_cred(ww_peer_t *peer, char *cred)
{
    uid_t uid;
    gid_t gid;
    char **vals;

    ww_output_verbose(2, ww_globals.debug_output,
                        "sec: native validate_cred %s", cred);

    /* parse the inbound string */
    vals = ww_argv_split(cred, ':');
    if (2 != ww_argv_count(vals)) {
        ww_argv_free(vals);
        return WW_ERR_INVALID_CRED;
    }

    /* check uid */
    uid = strtoul(vals[0], NULL, 10);
    if (uid != peer->uid) {
        ww_argv_free(vals);
        return WW_ERR_INVALID_CRED;
    }

    /* check guid */
    gid = strtoul(vals[1], NULL, 10);
    if (gid != peer->gid) {
        ww_argv_free(vals);
        return WW_ERR_INVALID_CRED;
    }
    ww_argv_free(vals);

    ww_output_verbose(2, ww_globals.debug_output,
                        "sec: native credential valid");
    return WW_SUCCESS;
}

