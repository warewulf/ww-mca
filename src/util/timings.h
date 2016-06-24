/*
 * Copyright (C) 2014      Artem Polyakov <artpol84@gmail.com>
 * Copyright (c) 2014-2015 Intel, Inc. All rights reserved.
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef WW_UTIL_TIMING_H
#define WW_UTIL_TIMING_H

#include <src/include/ww_config.h>


#include "src/class/ww_list.h"

#if WW_ENABLE_TIMING

#define WW_TIMING_DESCR_MAX 1024
#define WW_TIMING_BUFSIZE 32
#define WW_TIMING_OUTBUF_SIZE (10*1024)

typedef enum {
    WW_TIMING_TRACE,
    WW_TIMING_INTDESCR,
    WW_TIMING_INTBEGIN,
    WW_TIMING_INTEND
} ww_event_type_t;

typedef struct {
    ww_list_item_t super;
    int fib;
    ww_event_type_t type;
    const char *func;
    const char *file;
    int line;
    double ts, ts_ovh;
    char descr[WW_TIMING_DESCR_MAX];
    int id;
} ww_timing_event_t;

typedef double (*get_ts_t)(void);

typedef struct ww_timing_t
{
    int next_id_cntr;
    // not thread safe!
    // The whole implementation is not thread safe now
    // since it is supposed to be used in service
    // thread only. Fix in the future or now?
    int current_id;
    ww_list_t *events;
    ww_timing_event_t *buffer;
    size_t buffer_offset, buffer_size;
    get_ts_t get_ts;
} ww_timing_t;

typedef struct {
    ww_timing_t *t;
    ww_timing_event_t *ev;
    int errcode;
} ww_timing_prep_t;

/* Pass down our namespace and rank for pretty-print purposes */
void ww_init_id(char* nspace, int rank);

/**
 * Initialize timing structure.
 *
 * @param t pointer to the timing handler structure
 */
void ww_timing_init(ww_timing_t *t);

/**
 * Prepare timing event, do all printf-like processing.
 * Should not be directly used - for service purposes only.
 *
 * @param t pointer to the timing handler structure
 * @param fmt printf-like format
 * @param ... other parameters that should be converted to string representation
 *
 * @retval partly filled ww_timing_prep_t structure
  */
ww_timing_prep_t ww_timing_prep_ev(ww_timing_t *t, const char *fmt, ...);

/**
 * Prepare timing event, ignore printf-like processing.
 * Should not be directly used - for service purposes only.
 *
 * @param t pointer to the timing handler structure
 * @param fmt printf-like format
 * @param ... other parameters that should be converted to string representation
 *
 * @retval partly filled ww_timing_prep_t structure
  */
ww_timing_prep_t ww_timing_prep_ev_end(ww_timing_t *t, const char *fmt, ...);

/**
 * Enqueue timing event into the list of events in handler 't'.
 *
 * @param p result of ww_timing_prep_ev
 * @param func function name where event occurs
 * @param file file name where event occurs
 * @param line line number in the file
 *
 * @retval
 */
void ww_timing_add_step(ww_timing_prep_t p, const char *func,
                          const char *file, int line);

/**
 * Enqueue the description of the interval into a list of events
 * in handler 't'.
 *
 * @param p result of ww_timing_prep_ev
 * @param func function name where event occurs
 * @param file file name where event occurs
 * @param line line number in the file
 *
 * @retval id of event interval
 */
int ww_timing_descr(ww_timing_prep_t p, const char *func,
                      const char *file, int line);

/**
 * Enqueue the beginning of timing interval that already has the
 * description and assigned id into the list of events
 * in handler 't'.
 *
 * @param p result of ww_timing_prep_ev
 * @param func function name where event occurs
 * @param file file name where event occurs
 * @param line line number in the file
 *
 * @retval
 */
void ww_timing_start_id(ww_timing_t *t, int id, const char *func,
                          const char *file, int line);

/**
 * Enqueue the end of timing interval that already has
 * description and assigned id into the list of events
 * in handler 't'.
 *
 * @param p result of ww_timing_prep_ev
 * @param func function name where event occurs
 * @param file file name where event occurs
 * @param line line number in the file
 *
 * @retval
 */
void ww_timing_end(ww_timing_t *t, int id, const char *func,
                     const char *file, int line );

/**
 * Enqueue both description and start of timing interval
 * into the list of events and assign its id.
 *
 * @param p result of ww_timing_prep_ev
 * @param func function name where event occurs
 * @param file file name where event occurs
 * @param line line number in the file
 *
 * @retval interval id
 */
static inline int ww_timing_start_init(ww_timing_prep_t p,
                                         const char *func,
                                         const char *file, int line)
{
    int id = ww_timing_descr(p, func, file, line);
    if( id < 0 )
        return id;
    ww_timing_start_id(p.t, id, func, file, line);
    return id;
}

/**
 * The wrapper that is used to stop last measurement in WW_TIMING_MNEXT.
 *
 * @param p result of ww_timing_prep_ev
 * @param func function name where event occurs
 * @param file file name where event occurs
 * @param line line number in the file
 *
 * @retval interval id
 */
void ww_timing_end_prep(ww_timing_prep_t p,
                          const char *func, const char *file, int line);

/**
 * Report all events that were enqueued in the timing handler 't'.
 * - if fname == NULL the output will be done using ww_output and
 * each line will be prefixed with "prefix" to ease grep'ing.
 * - otherwise the corresponding file will be used for output in "append" mode
 * WARRNING: not all filesystems provide enough support for that feature, some records may
 * disappear.
 *
 * @param t timing handler
 * @param account_overhead consider malloc overhead introduced by timing code
 * @param prefix prefix to use when no fname was specifyed to ease grep'ing
 * @param fname name of the output file (may be NULL)
 *
 * @retval WW_SUCCESS On success
 * @retval WW_ERROR or WW_ERR_OUT_OF_RESOURCE On failure
 */
ww_status_t ww_timing_report(ww_timing_t *t, char *fname);

/**
 * Report all intervals that were enqueued in the timing handler 't'.
 * - if fname == NULL the output will be done using ww_output and
 * each line will be prefixed with "prefix" to ease grep'ing.
 * - otherwise the corresponding file will be used for output in "append" mode
 * WARRNING: not all filesystems provide enough support for that feature, some records may
 * disappear.
 *
 * @param t timing handler
 * @param account_overhead consider malloc overhead introduced by timing code
  * @param fname name of the output file (may be NULL)
 *
 * @retval WW_SUCCESS On success
 * @retval WW_ERROR or WW_ERR_OUT_OF_RESOURCE On failure
 */
ww_status_t ww_timing_deltas(ww_timing_t *t, char *fname);

/**
 * Release all memory allocated for the timing handler 't'.
 *
 * @param t timing handler
 *
 * @retval
 */
void ww_timing_release(ww_timing_t *t);

/**
 * Macro for passing down process id - compiled out
 * when configured without --enable-timing
 */
#define WW_TIMING_ID(n, r) ww_timing_id((n), (r));

/**
 * Main macro for use in declaring ww timing handler;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 */
#define WW_TIMING_DECLARE(t) ww_timing_t t;   /* need semicolon here to avoid warnings when not enabled */

/**
 * Main macro for use in declaring external ww timing handler;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 */
#define WW_TIMING_DECLARE_EXT(x, t) x extern ww_timing_t t;  /* need semicolon here to avoid warnings when not enabled */

/**
 * Main macro for use in initializing ww timing handler;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 * @see ww_timing_init()
 */
#define WW_TIMING_INIT(t) ww_timing_init(t)

/**
 * Macro that enqueues event with its description to the specified
 * timing handler;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 * @see ww_timing_add_step()
 */
#define WW_TIMING_EVENT(x) ww_timing_add_step( ww_timing_prep_ev x, __FUNCTION__, __FILE__, __LINE__)

/**
 * MDESCR: Measurement DESCRiption
 * Introduce new timing measurement with string description for the specified
 * timing handler;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 * @see ww_timing_descr()
 */
#define WW_TIMING_MDESCR(x) ww_timing_descr( ww_timing_prep_ev x, __FUNCTION__, __FILE__, __LINE__)

/**
 * MSTART_ID: Measurement START by ID.
 * Marks the beginning of the measurement with ID=id on the
 * specified timing handler;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 * @see ww_timing_start_id()
 */
#define WW_TIMING_MSTART_ID(t, id) ww_timing_start_id(t, id, __FUNCTION__, __FILE__, __LINE__)

/**
 * MSTART: Measurement START
 * Introduce new timing measurement conjuncted with its start
 * on the specifyed timing handler;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 * @see ww_timing_start_init()
 */
#define WW_TIMING_MSTART(x) ww_timing_start_init( ww_timing_prep_ev x, __FUNCTION__, __FILE__, __LINE__)

/**
 * MSTOP: STOP Measurement
 * Finishes the most recent measurement on the specifyed timing handler;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 * @see ww_timing_end()
 */
#define WW_TIMING_MSTOP(t) ww_timing_end(t, -1, __FUNCTION__, __FILE__, __LINE__)

/**
 * MSTOP_ID: STOP Measurement with ID=id.
 * Finishes the measurement with give ID on the specifyed timing handler;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 * @see ww_timing_end()
 */
#define WW_TIMING_MSTOP_ID(t, id) ww_timing_end(t, id, __FUNCTION__, __FILE__, __LINE__)

/**
 * MNEXT: start NEXT Measurement
 * Convinient macro, may be implemented with the sequence of three previously
 * defined macroses:
 * - finish current measurement (WW_TIMING_MSTOP);
 * - introduce new timing measurement (WW_TIMING_MDESCR);
 * - starts next measurement (WW_TIMING_MSTART_ID)
 * on the specifyed timing handler;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 * @see ww_timing_start_init()
 */
#define WW_TIMING_MNEXT(x) ( \
    ww_timing_end_prep(ww_timing_prep_ev_end x,             \
                            __FUNCTION__, __FILE__, __LINE__ ), \
    ww_timing_start_init( ww_timing_prep_ev x,              \
                            __FUNCTION__, __FILE__, __LINE__)   \
)

/**
 * The macro for use in reporting collected events with absolute values;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 * @param enable flag that enables/disables reporting. Used for fine-grained timing.
 * @see ww_timing_report()
 */
#define WW_TIMING_REPORT(enable, t) { \
    if( enable ) { \
        ww_timing_report(t, ww_timing_output); \
    } \
}

/**
 * The macro for use in reporting collected events with relative times;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 * @param enable flag that enables/disables reporting. Used for fine-grained timing.
 * @see ww_timing_deltas()
 */
#define WW_TIMING_DELTAS(enable, t) { \
    if( enable ) { \
        ww_timing_deltas(t, ww_timing_output); \
    } \
}

/**
 * Main macro for use in releasing allocated resources;
 * will be "compiled out" when WW is configured without
 * --enable-timing.
 *
 * @see ww_timing_release()
 */
#define WW_TIMING_RELEASE(t) ww_timing_release(t)

#else

#define WW_TIMING_ID(n, r)

#define WW_TIMING_DECLARE(t)

#define WW_TIMING_DECLARE_EXT(x, t)

#define WW_TIMING_INIT(t)

#define WW_TIMING_EVENT(x)

#define WW_TIMING_MDESCR(x)

#define WW_TIMING_MSTART_ID(t, id)

#define WW_TIMING_MSTART(x)

#define WW_TIMING_MSTOP(t)

#define WW_TIMING_MSTOP_ID(t, id)

#define WW_TIMING_MNEXT(x)

#define WW_TIMING_REPORT(enable, t)

#define WW_TIMING_DELTAS(enable, t)

#define WW_TIMING_RELEASE(t)

#endif

#endif
