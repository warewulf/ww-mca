/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2016 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2007-2016 Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2007      Voltaire. All rights reserved.
 * Copyright (c) 2010      Oracle and/or its affiliates.  All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef  WW_MUTEX_H
#define  WW_MUTEX_H 1

#include "ww_config.h"

#include "src/sys/atomic.h"
#include "src/include/prefetch.h"

BEGIN_C_DECLS

/**
 * @file:
 *
 * Mutual exclusion functions.
 *
 * Functions for locking of critical sections.
 */

/*
 * declaring this here so that CL does not complain
 */
WW_DECLSPEC extern bool ww_uses_threads;

/**
 * Opaque mutex object
 */
typedef struct ww_mutex_t ww_mutex_t;
typedef struct ww_mutex_t ww_recursive_mutex_t;

/**
 * Try to acquire a mutex.
 *
 * @param mutex         Address of the mutex.
 * @return              0 if the mutex was acquired, 1 otherwise.
 */
static inline int ww_mutex_trylock(ww_mutex_t *mutex);


/**
 * Acquire a mutex.
 *
 * @param mutex         Address of the mutex.
 */
static inline void ww_mutex_lock(ww_mutex_t *mutex);


/**
 * Release a mutex.
 *
 * @param mutex         Address of the mutex.
 */
static inline void ww_mutex_unlock(ww_mutex_t *mutex);


/**
 * Try to acquire a mutex using atomic operations.
 *
 * @param mutex         Address of the mutex.
 * @return              0 if the mutex was acquired, 1 otherwise.
 */
static inline int ww_mutex_atomic_trylock(ww_mutex_t *mutex);


/**
 * Acquire a mutex using atomic operations.
 *
 * @param mutex         Address of the mutex.
 */
static inline void ww_mutex_atomic_lock(ww_mutex_t *mutex);


/**
 * Release a mutex using atomic operations.
 *
 * @param mutex         Address of the mutex.
 */
static inline void ww_mutex_atomic_unlock(ww_mutex_t *mutex);

END_C_DECLS

#include "mutex_unix.h"

BEGIN_C_DECLS

/**
 * Check and see if the process is using multiple threads.
 *
 * @retval true If the process may have more than one thread.
 * @retval false If the process only has a single thread.
 *
 * The value that this function returns is influenced by:
 *
 * - how MPI_INIT or MPI_INIT_THREAD was invoked,
 * - what the final MPI thread level was determined to be,
 * - whether the OMPI or MPI libraries are multi-threaded (Jan 2003:
 *   they're not),
 * - whether configure determined if we have thread support or not
 *
 * MPI_INIT and MPI_INIT_THREAD (specifically, back-end OMPI startup
 * functions) invoke ww_set_using_threads() to influence the value of
 * this function, depending on their situation. Some examples:
 *
 * - if configure determined that we do not have threads, then this
 * value will always be false.
 *
 * - if MPI_INIT is invoked, and the ompi libraries are [still]
 * single-threaded, this value will be false.
 *
 * - if MPI_INIT_THREAD is invoked with MPI_THREAD_MULTIPLE, we have
 * thread support, and the final thread level is determined to be
 * MPI_THREAD_MULTIPLE, this value will be true.
 *
 * - if the process is a single-threaded OMPI executable (e.g., mpicc),
 * this value will be false.
 *
 * Hence, this function will return false if there is guaranteed to
 * only be one thread in the process.  If there is even the
 * possibility that we may have multiple threads, true will be
 * returned.
 */
#define ww_using_threads()  ww_uses_threads

/**
 * Set whether the process is using multiple threads or not.
 *
 * @param have Boolean indicating whether the process is using
 * multiple threads or not.
 *
 * @retval ww_using_threads The new return value from
 * ww_using_threads().
 *
 * This function is used to influence the return value of
 * ww_using_threads().  If configure detected that we have thread
 * support, the return value of future invocations of
 * ww_using_threads() will be the parameter's value.  If configure
 * detected that we have no thread support, then the return from
 * ww_using_threads() will always be false.
 */
static inline bool ww_set_using_threads(bool have)
{
    ww_uses_threads = have;
    return ww_using_threads();
}


/**
 * Lock a mutex if ww_using_threads() says that multiple threads may
 * be active in the process.
 *
 * @param mutex Pointer to a ww_mutex_t to lock.
 *
 * If there is a possibility that multiple threads are running in the
 * process (as determined by ww_using_threads()), this function will
 * block waiting to lock the mutex.
 *
 * If there is no possibility that multiple threads are running in the
 * process, return immediately.
 */
#define WW_THREAD_LOCK(mutex)                 \
    do {                                        \
        if (WW_UNLIKELY(ww_using_threads())) {      \
            ww_mutex_lock(mutex);             \
        }                                       \
    } while (0)


/**
 * Try to lock a mutex if ww_using_threads() says that multiple
 * threads may be active in the process.
 *
 * @param mutex Pointer to a ww_mutex_t to trylock
 *
 * If there is a possibility that multiple threads are running in the
 * process (as determined by ww_using_threads()), this function will
 * trylock the mutex.
 *
 * If there is no possibility that multiple threads are running in the
 * process, return immediately without modifying the mutex.
 *
 * Returns 0 if mutex was locked, non-zero otherwise.
 */
#define WW_THREAD_TRYLOCK(mutex)                      \
    (WW_UNLIKELY(ww_using_threads()) ? ww_mutex_trylock(mutex) : 0)

/**
 * Unlock a mutex if ww_using_threads() says that multiple threads
 * may be active in the process.
 *
 * @param mutex Pointer to a ww_mutex_t to unlock.
 *
 * If there is a possibility that multiple threads are running in the
 * process (as determined by ww_using_threads()), this function will
 * unlock the mutex.
 *
 * If there is no possibility that multiple threads are running in the
 * process, return immediately without modifying the mutex.
 */
#define WW_THREAD_UNLOCK(mutex)               \
    do {                                        \
        if (WW_UNLIKELY(ww_using_threads())) {      \
            ww_mutex_unlock(mutex);           \
        }                                       \
    } while (0)


/**
 * Lock a mutex if ww_using_threads() says that multiple threads may
 * be active in the process for the duration of the specified action.
 *
 * @param mutex    Pointer to a ww_mutex_t to lock.
 * @param action   A scope over which the lock is held.
 *
 * If there is a possibility that multiple threads are running in the
 * process (as determined by ww_using_threads()), this function will
 * acquire the lock before invoking the specified action and release
 * it on return.
 *
 * If there is no possibility that multiple threads are running in the
 * process, invoke the action without acquiring the lock.
 */
#define WW_THREAD_SCOPED_LOCK(mutex, action)  \
    do {                                        \
        if(WW_UNLIKELY(ww_using_threads())) {       \
            ww_mutex_lock(mutex);             \
            action;                             \
            ww_mutex_unlock(mutex);           \
        } else {                                \
            action;                             \
        }                                       \
    } while (0)

/**
 * Use an atomic operation for increment/decrement if ww_using_threads()
 * indicates that threads are in use by the application or library.
 */

static inline int32_t
WW_THREAD_ADD32(volatile int32_t *addr, int delta)
{
    int32_t ret;

    if (WW_UNLIKELY(ww_using_threads())) {
        ret = ww_atomic_add_32(addr, delta);
    } else {
        ret = (*addr += delta);
    }

    return ret;
}

#if WW_HAVE_ATOMIC_MATH_64
static inline int64_t
WW_THREAD_ADD64(volatile int64_t *addr, int delta)
{
    int64_t ret;

    if (WW_UNLIKELY(ww_using_threads())) {
        ret = ww_atomic_add_64(addr, delta);
    } else {
        ret = (*addr += delta);
    }

    return ret;
}
#endif

static inline size_t
WW_THREAD_ADD_SIZE_T(volatile size_t *addr, int delta)
{
    size_t ret;

    if (WW_UNLIKELY(ww_using_threads())) {
        ret = ww_atomic_add_size_t(addr, delta);
    } else {
        ret = (*addr += delta);
    }

    return ret;
}

/* BWB: FIX ME: remove if possible */
#define WW_CMPSET(x, y, z) ((*(x) == (y)) ? ((*(x) = (z)), 1) : 0)

#if WW_HAVE_ATOMIC_CMPSET_32
#define WW_ATOMIC_CMPSET_32(x, y, z) \
    (WW_UNLIKELY(ww_using_threads()) ? ww_atomic_cmpset_32(x, y, z) : WW_CMPSET(x, y, z))
#endif
#if WW_HAVE_ATOMIC_CMPSET_64
#define WW_ATOMIC_CMPSET_64(x, y, z) \
    (WW_UNLIKELY(ww_using_threads()) ? ww_atomic_cmpset_64(x, y, z) : WW_CMPSET(x, y, z))
#endif
#if WW_HAVE_ATOMIC_CMPSET_32 || WW_HAVE_ATOMIC_CMPSET_64
#define WW_ATOMIC_CMPSET(x, y, z) \
    (WW_UNLIKELY(ww_using_threads()) ? ww_atomic_cmpset(x, y, z) : WW_CMPSET(x, y, z))
#endif
#if WW_HAVE_ATOMIC_CMPSET_32 || WW_HAVE_ATOMIC_CMPSET_64
#define WW_ATOMIC_CMPSET_PTR(x, y, z) \
    (ww_using_threads() ? ww_atomic_cmpset_ptr(x, y, z) : WW_CMPSET(x, y, z))
#endif


static inline void *ww_thread_swap_ptr (volatile void *ptr, void *newvalue)
{
    if (ww_using_threads ()) {
        return ww_atomic_swap_ptr (ptr, newvalue);
    }

    void *old = ((void **) ptr)[0];
    ((void **) ptr)[0] = newvalue;

    return old;
}

#define WW_ATOMIC_SWAP_PTR(x, y) ww_thread_swap_ptr (x, y)

END_C_DECLS

#endif                          /* WW_MUTEX_H */
