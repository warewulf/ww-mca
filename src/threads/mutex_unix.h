/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
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
 * Copyright (c) 2007-2015 Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef  WW_MUTEX_UNIX_H
#define  WW_MUTEX_UNIX_H 1

/**
 * @file:
 *
 * Mutual exclusion functions: Unix implementation.
 *
 * Functions for locking of critical sections.
 *
 * On unix, use pthreads or our own atomic operations as
 * available.
 */

#include "ww_config.h"

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#include <errno.h>
#include <stdio.h>

#include "src/class/ww_object.h"
#include "src/sys/atomic.h"

BEGIN_C_DECLS

struct ww_mutex_t {
    ww_object_t super;

    pthread_mutex_t m_lock_pthread;

#if WW_ENABLE_DEBUG
    int m_lock_debug;
    const char *m_lock_file;
    int m_lock_line;
#endif

    ww_atomic_lock_t m_lock_atomic;
};
WW_DECLSPEC WW_CLASS_DECLARATION(ww_mutex_t);
WW_DECLSPEC WW_CLASS_DECLARATION(ww_recursive_mutex_t);

#if defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)
#define WW_PTHREAD_RECURSIVE_MUTEX_INITIALIZER PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#elif defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
#define WW_PTHREAD_RECURSIVE_MUTEX_INITIALIZER PTHREAD_RECURSIVE_MUTEX_INITIALIZER
#endif

#if WW_ENABLE_DEBUG
#define WW_MUTEX_STATIC_INIT                                          \
    {                                                                   \
        .super = WW_OBJ_STATIC_INIT(ww_mutex_t),                    \
        .m_lock_pthread = PTHREAD_MUTEX_INITIALIZER,                    \
        .m_lock_debug = 0,                                              \
        .m_lock_file = NULL,                                            \
        .m_lock_line = 0,                                               \
        .m_lock_atomic = { .u = { .lock = WW_ATOMIC_UNLOCKED } },     \
    }
#else
#define WW_MUTEX_STATIC_INIT                                          \
    {                                                                   \
        .super = WW_OBJ_STATIC_INIT(ww_mutex_t),                    \
        .m_lock_pthread = PTHREAD_MUTEX_INITIALIZER,                    \
        .m_lock_atomic = { .u = { .lock = WW_ATOMIC_UNLOCKED } },     \
    }
#endif

#if defined(WW_PTHREAD_RECURSIVE_MUTEX_INITIALIZER)

#if WW_ENABLE_DEBUG
#define WW_RECURSIVE_MUTEX_STATIC_INIT                                \
    {                                                                   \
        .super = WW_OBJ_STATIC_INIT(ww_mutex_t),                    \
        .m_lock_pthread = WW_PTHREAD_RECURSIVE_MUTEX_INITIALIZER,     \
        .m_lock_debug = 0,                                              \
        .m_lock_file = NULL,                                            \
        .m_lock_line = 0,                                               \
        .m_lock_atomic = { .u = { .lock = WW_ATOMIC_UNLOCKED } },     \
    }
#else
#define WW_RECURSIVE_MUTEX_STATIC_INIT                                \
    {                                                                   \
        .super = WW_OBJ_STATIC_INIT(ww_mutex_t),                    \
        .m_lock_pthread = WW_PTHREAD_RECURSIVE_MUTEX_INITIALIZER,     \
        .m_lock_atomic = { .u = { .lock = WW_ATOMIC_UNLOCKED } },     \
    }
#endif

#endif

/************************************************************************
 *
 * mutex operations (non-atomic versions)
 *
 ************************************************************************/

static inline int ww_mutex_trylock(ww_mutex_t *m)
{
#if WW_ENABLE_DEBUG
    int ret = pthread_mutex_trylock(&m->m_lock_pthread);
    if (ret == EDEADLK) {
        errno = ret;
        perror("ww_mutex_trylock()");
        abort();
    }
    return ret;
#else
    return pthread_mutex_trylock(&m->m_lock_pthread);
#endif
}

static inline void ww_mutex_lock(ww_mutex_t *m)
{
#if WW_ENABLE_DEBUG
    int ret = pthread_mutex_lock(&m->m_lock_pthread);
    if (ret == EDEADLK) {
        errno = ret;
        perror("ww_mutex_lock()");
        abort();
    }
#else
    pthread_mutex_lock(&m->m_lock_pthread);
#endif
}

static inline void ww_mutex_unlock(ww_mutex_t *m)
{
#if WW_ENABLE_DEBUG
    int ret = pthread_mutex_unlock(&m->m_lock_pthread);
    if (ret == EPERM) {
        errno = ret;
        perror("ww_mutex_unlock");
        abort();
    }
#else
    pthread_mutex_unlock(&m->m_lock_pthread);
#endif
}

/************************************************************************
 *
 * mutex operations (atomic versions)
 *
 ************************************************************************/

#if WW_HAVE_ATOMIC_SPINLOCKS

/************************************************************************
 * Spin Locks
 ************************************************************************/

static inline int ww_mutex_atomic_trylock(ww_mutex_t *m)
{
    return ww_atomic_trylock(&m->m_lock_atomic);
}

static inline void ww_mutex_atomic_lock(ww_mutex_t *m)
{
    ww_atomic_lock(&m->m_lock_atomic);
}

static inline void ww_mutex_atomic_unlock(ww_mutex_t *m)
{
    ww_atomic_unlock(&m->m_lock_atomic);
}

#else

/************************************************************************
 * Standard locking
 ************************************************************************/

static inline int ww_mutex_atomic_trylock(ww_mutex_t *m)
{
    return ww_mutex_trylock(m);
}

static inline void ww_mutex_atomic_lock(ww_mutex_t *m)
{
    ww_mutex_lock(m);
}

static inline void ww_mutex_atomic_unlock(ww_mutex_t *m)
{
    ww_mutex_unlock(m);
}

#endif

END_C_DECLS

#endif                          /* WW_MUTEX_UNIX_H */
