/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
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
 * Copyright (c) 2007-2016 Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "ww_config.h"

#include "src/threads/mutex.h"

/*
 * Wait and see if some upper layer wants to use threads, if support
 * exists.
 */
bool ww_uses_threads = false;

static void ww_mutex_construct(ww_mutex_t *m)
{
#if WW_ENABLE_DEBUG
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

    /* set type to ERRORCHECK so that we catch recursive locks */
#if WW_HAVE_PTHREAD_MUTEX_ERRORCHECK_NP
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
#elif WW_HAVE_PTHREAD_MUTEX_ERRORCHECK
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
#endif /* WW_HAVE_PTHREAD_MUTEX_ERRORCHECK_NP */

    pthread_mutex_init(&m->m_lock_pthread, &attr);
    pthread_mutexattr_destroy(&attr);

    m->m_lock_debug = 0;
    m->m_lock_file = NULL;
    m->m_lock_line = 0;
#else

    /* Without debugging, choose the fastest available mutexes */
    pthread_mutex_init(&m->m_lock_pthread, NULL);

#endif /* WW_ENABLE_DEBUG */

#if WW_HAVE_ATOMIC_SPINLOCKS
    ww_atomic_init( &m->m_lock_atomic, WW_ATOMIC_UNLOCKED );
#endif
}

static void ww_mutex_destruct(ww_mutex_t *m)
{
    pthread_mutex_destroy(&m->m_lock_pthread);
}

WW_CLASS_INSTANCE(ww_mutex_t,
                   ww_object_t,
                   ww_mutex_construct,
                   ww_mutex_destruct);

static void ww_recursive_mutex_construct(ww_recursive_mutex_t *m)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

#if WW_ENABLE_DEBUG
    m->m_lock_debug = 0;
    m->m_lock_file = NULL;
    m->m_lock_line = 0;
#endif

    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&m->m_lock_pthread, &attr);
    pthread_mutexattr_destroy(&attr);

#if WW_HAVE_ATOMIC_SPINLOCKS
    ww_atomic_init( &m->m_lock_atomic, WW_ATOMIC_UNLOCKED );
#endif
}

WW_CLASS_INSTANCE(ww_recursive_mutex_t,
                   ww_object_t,
                   ww_recursive_mutex_construct,
                   ww_mutex_destruct);
