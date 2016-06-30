/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2010      Cisco Systems, Inc. All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "ww_config.h"

#include "src/threads/threads.h"
#include "include/ww_types.h"

bool ww_debug_threads = false;

static void ww_thread_construct(ww_thread_t *t);

WW_CLASS_INSTANCE(ww_thread_t,
                   ww_object_t,
                   ww_thread_construct, NULL);


/*
 * Constructor
 */
static void ww_thread_construct(ww_thread_t *t)
{
    t->t_run = 0;
    t->t_handle = (pthread_t) -1;
}

int ww_thread_start(ww_thread_t *t)
{
    int rc;

    if (WW_ENABLE_DEBUG) {
        if (NULL == t->t_run || t->t_handle != (pthread_t) -1) {
            return WW_ERR_BAD_PARAM;
        }
    }

    rc = pthread_create(&t->t_handle, NULL, (void*(*)(void*)) t->t_run, t);

    return (rc == 0) ? WW_SUCCESS : WW_ERROR;
}


int ww_thread_join(ww_thread_t *t, void **thr_return)
{
    int rc = pthread_join(t->t_handle, thr_return);
    t->t_handle = (pthread_t) -1;
    return (rc == 0) ? WW_SUCCESS : WW_ERROR;
}


bool ww_thread_self_compare(ww_thread_t *t)
{
    return t->t_handle == pthread_self();
}


ww_thread_t *ww_thread_get_self(void)
{
    ww_thread_t *t = WW_NEW(ww_thread_t);
    t->t_handle = pthread_self();
    return t;
}

void ww_thread_kill(ww_thread_t *t, int sig)
{
    pthread_kill(t->t_handle, sig);
}
