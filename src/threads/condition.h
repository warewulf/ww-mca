/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#ifndef WW_CONDITION_SPINLOCK_H
#define WW_CONDITION_SPINLOCK_H

#include "ww_config.h"
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <time.h>
#include <pthread.h>

#include "src/threads/mutex.h"

BEGIN_C_DECLS

/*
 * Combine pthread support w/ polled progress to allow run-time selection
 * of threading vs. non-threading progress.
 */

struct ww_condition_t {
    ww_object_t super;
    volatile int c_waiting;
    volatile int c_signaled;
};
typedef struct ww_condition_t ww_condition_t;

WW_DECLSPEC WW_CLASS_DECLARATION(ww_condition_t);

static void ww_progress(void)
{
    /* provide a short quiet period so we
     * don't hammer the cpu while waiting
     */
    struct timespec tp = {0, 100000};
    nanosleep(&tp, NULL);
}

static inline int ww_condition_wait(ww_condition_t *c, ww_mutex_t *m)
{
    int rc = 0;
    c->c_waiting++;

    if (ww_using_threads()) {
        if (c->c_signaled) {
            c->c_waiting--;
            ww_mutex_unlock(m);
            ww_progress();
            ww_mutex_lock(m);
            return 0;
        }
        while (c->c_signaled == 0) {
            ww_mutex_unlock(m);
            ww_progress();
            ww_mutex_lock(m);
        }
    } else {
        while (c->c_signaled == 0) {
            ww_progress();
        }
    }

    c->c_signaled--;
    c->c_waiting--;
    return rc;
}

static inline int ww_condition_timedwait(ww_condition_t *c,
                                           ww_mutex_t *m,
                                           const struct timespec *abstime)
{
    struct timeval tv;
    struct timeval absolute;
    int rc = 0;

    c->c_waiting++;
    if (ww_using_threads()) {
        absolute.tv_sec = abstime->tv_sec;
        absolute.tv_usec = abstime->tv_nsec / 1000;
        gettimeofday(&tv,NULL);
        if (c->c_signaled == 0) {
            do {
                ww_mutex_unlock(m);
                ww_progress();
                gettimeofday(&tv,NULL);
                ww_mutex_lock(m);
                } while (c->c_signaled == 0 &&
                         (tv.tv_sec <= absolute.tv_sec ||
                          (tv.tv_sec == absolute.tv_sec && tv.tv_usec < absolute.tv_usec)));
        }
    } else {
        absolute.tv_sec = abstime->tv_sec;
        absolute.tv_usec = abstime->tv_nsec / 1000;
        gettimeofday(&tv,NULL);
        if (c->c_signaled == 0) {
            do {
                ww_progress();
                gettimeofday(&tv,NULL);
                } while (c->c_signaled == 0 &&
                         (tv.tv_sec <= absolute.tv_sec ||
                          (tv.tv_sec == absolute.tv_sec && tv.tv_usec < absolute.tv_usec)));
        }
    }

    if (c->c_signaled != 0) c->c_signaled--;
    c->c_waiting--;
    return rc;
}

static inline int ww_condition_signal(ww_condition_t *c)
{
    if (c->c_waiting) {
        c->c_signaled++;
    }
    return 0;
}

static inline int ww_condition_broadcast(ww_condition_t *c)
{
    c->c_signaled = c->c_waiting;
    return 0;
}

END_C_DECLS

#endif

