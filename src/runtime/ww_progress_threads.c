/*
 * Copyright (c) 2014-2016 Intel, Inc.  All rights reserved.
 * Copyright (c) 2015      Cisco Systems, Inc.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>
#include "ww_types.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <pthread.h>
#include WW_EVENT_HEADER

#include "src/class/ww_list.h"
#include "src/util/error.h"
#include "src/util/fd.h"

#include "src/runtime/ww_progress_threads.h"

/* define a thread object */
#define WW_THREAD_CANCELLED   ((void*)1);
typedef void *(*ww_thread_fn_t) (ww_object_t *);

typedef struct ww_thread_t {
    ww_object_t super;
    ww_thread_fn_t t_run;
    void* t_arg;
    pthread_t t_handle;
} ww_thread_t;
WW_CLASS_INSTANCE(ww_thread_t,
                  ww_object_t,
                  NULL, NULL);

static int ww_thread_start(ww_thread_t *t)
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


static int ww_thread_join(ww_thread_t *t, void **thr_return)
{
    int rc = pthread_join(t->t_handle, thr_return);
    t->t_handle = (pthread_t) -1;
    return (rc == 0) ? WW_SUCCESS : WW_ERROR;
}


/* create a tracking object for progress threads */
typedef struct {
    ww_list_item_t super;

    int refcount;
    char *name;

    ww_event_base_t *ev_base;

    /* This will be set to false when it is time for the progress
       thread to exit */
    volatile bool ev_active;

    /* This event will always be set on the ev_base (so that the
       ev_base is not empty!) */
    ww_event_t block;

    bool engine_constructed;
    ww_thread_t engine;
} ww_progress_tracker_t;

static void tracker_constructor(ww_progress_tracker_t *p)
{
    p->refcount = 1;  // start at one since someone created it
    p->name = NULL;
    p->ev_base = NULL;
    p->ev_active = false;
    p->engine_constructed = false;
}

static void tracker_destructor(ww_progress_tracker_t *p)
{
    ww_event_del(&p->block);

    if (NULL != p->name) {
        free(p->name);
    }
    if (NULL != p->ev_base) {
        ww_event_base_free(p->ev_base);
    }
    if (p->engine_constructed) {
        WW_DESTRUCT(&p->engine);
    }
}

static WW_CLASS_INSTANCE(ww_progress_tracker_t,
                          ww_list_item_t,
                          tracker_constructor,
                          tracker_destructor);

static bool inited = false;
static ww_list_t tracking;
static struct timeval long_timeout = {
    .tv_sec = 3600,
    .tv_usec = 0
};
static const char *shared_thread_name = "WW-wide async progress thread";

/*
 * If this event is fired, just restart it so that this event base
 * continues to have something to block on.
 */
static void dummy_timeout_cb(int fd, short args, void *cbdata)
{
    ww_progress_tracker_t *trk = (ww_progress_tracker_t*)cbdata;

    ww_event_add(&trk->block, &long_timeout);
}

/*
 * Main for the progress thread
 */
static void* progress_engine(ww_object_t *obj)
{
    ww_thread_t *t = (ww_thread_t*)obj;
    ww_progress_tracker_t *trk = (ww_progress_tracker_t*)t->t_arg;

    while (trk->ev_active) {
        ww_event_loop(trk->ev_base, WW_EVLOOP_ONCE);
    }

    return WW_THREAD_CANCELLED;
}

static void stop_progress_engine(ww_progress_tracker_t *trk)
{
    assert(trk->ev_active);
    trk->ev_active = false;

    /* break the event loop - this will cause the loop to exit upon
       completion of any current event */
    ww_event_base_loopbreak(trk->ev_base);

    ww_thread_join(&trk->engine, NULL);
}

static int start_progress_engine(ww_progress_tracker_t *trk)
{
    assert(!trk->ev_active);
    trk->ev_active = true;

    /* fork off a thread to progress it */
    trk->engine.t_run = progress_engine;
    trk->engine.t_arg = trk;

    int rc = ww_thread_start(&trk->engine);
    if (WW_SUCCESS != rc) {
        WW_ERROR_LOG(rc);
    }

    return rc;
}

ww_event_base_t *ww_progress_thread_init(const char *name)
{
    ww_progress_tracker_t *trk;
    int rc;

    if (!inited) {
        WW_CONSTRUCT(&tracking, ww_list_t);
        inited = true;
    }

    if (NULL == name) {
        name = shared_thread_name;
    }

    /* check if we already have this thread */
    WW_LIST_FOREACH(trk, &tracking, ww_progress_tracker_t) {
        if (0 == strcmp(name, trk->name)) {
            /* we do, so up the refcount on it */
            ++trk->refcount;
            /* return the existing base */
            return trk->ev_base;
        }
    }

    trk = WW_NEW(ww_progress_tracker_t);
    if (NULL == trk) {
        WW_ERROR_LOG(WW_ERR_OUT_OF_RESOURCE);
        return NULL;
    }

    trk->name = strdup(name);
    if (NULL == trk->name) {
        WW_ERROR_LOG(WW_ERR_OUT_OF_RESOURCE);
        WW_RELEASE(trk);
        return NULL;
    }

    if (NULL == (trk->ev_base = ww_event_base_create())) {
        WW_ERROR_LOG(WW_ERR_OUT_OF_RESOURCE);
        WW_RELEASE(trk);
        return NULL;
    }

    /* add an event to the new event base (if there are no events,
       ww_event_loop() will return immediately) */
    ww_event_set(trk->ev_base, &trk->block, -1, WW_EV_PERSIST,
                   dummy_timeout_cb, trk);
    ww_event_add(&trk->block, &long_timeout);

    /* construct the thread object */
    WW_CONSTRUCT(&trk->engine, ww_thread_t);
    trk->engine_constructed = true;
    if (WW_SUCCESS != (rc = start_progress_engine(trk))) {
        WW_ERROR_LOG(rc);
        WW_RELEASE(trk);
        return NULL;
    }
    ww_list_append(&tracking, &trk->super);

    return trk->ev_base;
}

int ww_progress_thread_finalize(const char *name)
{
    ww_progress_tracker_t *trk;

    if (!inited) {
        /* nothing we can do */
        return WW_ERR_NOT_FOUND;
    }

    if (NULL == name) {
        name = shared_thread_name;
    }

    /* find the specified engine */
    WW_LIST_FOREACH(trk, &tracking, ww_progress_tracker_t) {
        if (0 == strcmp(name, trk->name)) {
            /* decrement the refcount */
            --trk->refcount;

            /* If the refcount is still above 0, we're done here */
            if (trk->refcount > 0) {
                return WW_SUCCESS;
            }

            /* If the progress thread is active, stop it */
            if (trk->ev_active) {
                stop_progress_engine(trk);
            }

            ww_list_remove_item(&tracking, &trk->super);
            WW_RELEASE(trk);
            return WW_SUCCESS;
        }
    }

    return WW_ERR_NOT_FOUND;
}

/*
 * Stop the progress thread, but don't delete the tracker (or event base)
 */
int ww_progress_thread_pause(const char *name)
{
    ww_progress_tracker_t *trk;

    if (!inited) {
        /* nothing we can do */
        return WW_ERR_NOT_FOUND;
    }

    if (NULL == name) {
        name = shared_thread_name;
    }

    /* find the specified engine */
    WW_LIST_FOREACH(trk, &tracking, ww_progress_tracker_t) {
        if (0 == strcmp(name, trk->name)) {
            if (trk->ev_active) {
                stop_progress_engine(trk);
            }

            return WW_SUCCESS;
        }
    }

    return WW_ERR_NOT_FOUND;
}

int ww_progress_thread_resume(const char *name)
{
    ww_progress_tracker_t *trk;

    if (!inited) {
        /* nothing we can do */
        return WW_ERR_NOT_FOUND;
    }

    if (NULL == name) {
        name = shared_thread_name;
    }

    /* find the specified engine */
    WW_LIST_FOREACH(trk, &tracking, ww_progress_tracker_t) {
        if (0 == strcmp(name, trk->name)) {
            if (trk->ev_active) {
                return WW_ERR_RESOURCE_BUSY;
            }

            return start_progress_engine(trk);
        }
    }

    return WW_ERR_NOT_FOUND;
}
