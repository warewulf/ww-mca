/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2010      Cisco Systems, Inc. All rights reserved.
 * Copyright (c) 2010      Oracle and/or its affiliates.  All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef WW_THREAD_H
#define WW_THREAD_H 1

#include "ww_config.h"

#include <pthread.h>
#include <signal.h>

#include "src/class/ww_object.h"
#if WW_ENABLE_DEBUG
#include "src/util/output.h"
#endif

#include "mutex.h"
#include "condition.h"

BEGIN_C_DECLS

typedef void *(*ww_thread_fn_t) (ww_object_t *);

#define WW_THREAD_CANCELLED   ((void*)1);

struct ww_thread_t {
    ww_object_t super;
    ww_thread_fn_t t_run;
    void* t_arg;
    pthread_t t_handle;
};

typedef struct ww_thread_t ww_thread_t;

typedef struct ww_threadlock_t {
    ww_mutex_t mutex;
    ww_condition_t cond;
    volatile bool in_use;
} ww_threadlock_t;

#if WW_ENABLE_DEBUG
WW_DECLSPEC extern bool ww_debug_threads;
#endif


WW_DECLSPEC WW_CLASS_DECLARATION(ww_thread_t);

#if WW_ENABLE_DEBUG
#define WW_ACQUIRE_THREAD(lck)                                  \
    do {                                                        \
        WW_THREAD_LOCK(&(lck)->mutex);                          \
        if (ww_debug_threads) {                                 \
            ww_output(0, "Waiting for thread %s:%d",            \
                        __FILE__, __LINE__);                    \
        }                                                       \
        while ((lck)->in_use) {                                 \
            ww_condition_wait(&(lck)->cond, &(lck)->mutex);     \
        }                                                       \
        if (ww_debug_threads) {                                 \
            ww_output(0, "Thread obtained %s:%d",               \
                        __FILE__, __LINE__);                    \
        }                                                       \
        (lck)->in_use = true;                                   \
    } while(0);
#else
#define WW_ACQUIRE_THREAD(lck)                                  \
    do {                                                        \
        WW_THREAD_LOCK((lck));                                  \
        while ((lck)->in_use) {                                 \
            ww_condition_wait(&(lck)->cond, &(lck)->mutex);     \
        }                                                       \
        (lck)->in_use = true;                                   \
    } while(0);
#endif


#if WW_ENABLE_DEBUG
#define WW_RELEASE_THREAD(lck)                          \
    do {                                                \
        if (ww_debug_threads) {                         \
            ww_output(0, "Releasing thread %s:%d",      \
                        __FILE__, __LINE__);            \
        }                                               \
        (lck)->in_use = false;                          \
        ww_condition_broadcast((cnd));                  \
        WW_THREAD_UNLOCK((lck));                        \
    } while(0);
#else
#define WW_RELEASE_THREAD(lck, cnd, act)                \
    do {                                                \
        (lck)->in_use = false;                          \
        ww_condition_broadcast(&(lck)->cond);           \
        WW_THREAD_UNLOCK(&(lck)->mutex);                \
    } while(0);
#endif


#define WW_WAKEUP_THREAD(lck)                       \
    do {                                            \
        (lck)->in_use = false;                      \
        ww_condition_broadcast(&(lck)->cond);       \
    } while(0);


WW_DECLSPEC int  ww_thread_start(ww_thread_t *);
WW_DECLSPEC int  ww_thread_join(ww_thread_t *, void **thread_return);
WW_DECLSPEC bool ww_thread_self_compare(ww_thread_t*);
WW_DECLSPEC ww_thread_t *ww_thread_get_self(void);
WW_DECLSPEC void ww_thread_kill(ww_thread_t *, int sig);

END_C_DECLS

#endif /* WW_THREAD_H */
