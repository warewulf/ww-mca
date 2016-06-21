/*
 * Copyright (c) 2013-2016 Intel, Inc. All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer listed
 *   in this license in the documentation and/or other materials
 *   provided with the distribution.
 *
 * - Neither the name of the copyright holders nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * The copyright holders provide no reassurances that the source code
 * provided does not infringe any patent, copyright, or any other
 * intellectual property rights of third parties.  The copyright holders
 * disclaim any liability to any recipient for claims brought against
 * recipient by any third party for infringement of that parties
 * intellectual property rights.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef WW_TYPES_H
#define WW_TYPES_H

#include WW_EVENT_HEADER

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

/****  WW CONSTANTS    ****/
/* WW errors are always negative, with 0 reserved for success */
#define WW_ERR_BASE                   0

typedef int ww_status_t;

#define WW_SUCCESS                              (WW_ERR_BASE)
#define WW_ERROR                                (WW_ERR_BASE -  1)    // general error
/* debugger release flag */
#define WW_ERR_DEBUGGER_RELEASE                 (WW_ERR_BASE -  2)
/* fault tolerance */
#define WW_ERR_PROC_RESTART                     (WW_ERR_BASE -  3)
#define WW_ERR_PROC_CHECKPOINT                  (WW_ERR_BASE -  4)
#define WW_ERR_PROC_MIGRATE                     (WW_ERR_BASE -  5)
#define WW_ERR_UPDATE_ENDPOINTS                 (WW_ERR_BASE -  6)
/* abort */
#define WW_ERR_PROC_ABORTED                     (WW_ERR_BASE -  7)
#define WW_ERR_PROC_REQUESTED_ABORT             (WW_ERR_BASE -  8)
#define WW_ERR_PROC_ABORTING                    (WW_ERR_BASE -  9)
/* communication failures */
#define WW_ERR_UNREACH                          (WW_ERR_BASE - 10)
#define WW_ERR_LOST_CONNECTION_TO_SERVER        (WW_ERR_BASE - 11)
#define WW_ERR_LOST_PEER_CONNECTION             (WW_ERR_BASE - 12)
#define WW_ERR_LOST_CONNECTION_TO_CLIENT        (WW_ERR_BASE - 13)
/* operational */
#define WW_ERR_NO_PERMISSIONS                   (WW_ERR_BASE - 14)
#define WW_ERR_TIMEOUT                          (WW_ERR_BASE - 15)
#define WW_ERR_WOULD_BLOCK                      (WW_ERR_BASE - 16)
#define WW_EXISTS                               (WW_ERR_BASE - 17)
#define WW_ERR_SERVER_FAILED_REQUEST            (WW_ERR_BASE - 18)
#define WW_ERR_NOT_SUPPORTED                    (WW_ERR_BASE - 19)
#define WW_ERR_NOT_FOUND                        (WW_ERR_BASE - 20)
#define WW_ERR_BAD_PARAM                        (WW_ERR_BASE - 21)
#define WW_ERR_DATA_VALUE_NOT_FOUND             (WW_ERR_BASE - 22)
#define WW_ERR_OUT_OF_RESOURCE                  (WW_ERR_BASE - 23)
#define WW_ERR_INVALID_NAMESPACE                (WW_ERR_BASE - 24)
#define WW_ERR_INVALID_SIZE                     (WW_ERR_BASE - 25)
#define WW_ERR_INIT                             (WW_ERR_BASE - 26)
#define WW_ERR_EVENT_REGISTRATION               (WW_ERR_BASE - 27)
/* system failures */
#define WW_ERR_NODE_DOWN                        (WW_ERR_BASE - 28)
#define WW_ERR_NODE_OFFLINE                     (WW_ERR_BASE - 29)
/* used by event handlers */
#define WW_EVENT_NO_ACTION_TAKEN                (WW_ERR_BASE - 30)
#define WW_EVENT_PARTIAL_ACTION_TAKEN           (WW_ERR_BASE - 31)
#define WW_EVENT_ACTION_DEFERRED                (WW_ERR_BASE - 32)
#define WW_EVENT_ACTION_COMPLETE                (WW_ERR_BASE - 33)
/* general errors */
#define WW_ERR_NOT_AVAILABLE                    (WW_ERR_BASE - 34)
#define WW_ERR_FATAL                            (WW_ERR_BASE - 35)
#define WW_ERR_VALUE_OUT_OF_BOUNDS              (WW_ERR_BASE - 36)
#define WW_ERR_PERM                             (WW_ERR_BASE - 37)

#define WW_INTERNAL_ERR_BASE  -500

/* setup some equivalencies for libevent */
typedef struct event_base ww_event_base_t;
typedef struct event ww_event_t;
#define ww_event_del(a)  event_del(a)
#define ww_event_base_create() event_base_new()
#define ww_event_base_free(a)   event_base_free(a)
#define ww_event_add(ev, tv) event_add((ev), (tv))
#define ww_event_set(b, x, fd, fg, cb, arg) event_assign((x), (b), (fd), (fg), (event_callback_fn) (cb), (arg))
#define ww_event_loop(a, b) event_base_loop(a, b)
#define ww_event_base_loopbreak(a) event_base_loopbreak(a)
#define WW_EVLOOP_ONCE  EVLOOP_ONCE
#define WW_EV_PERSIST EV_PERSIST

/* define maximum value and key sizes */
#define WW_MAX_NSLEN     255
#define WW_MAX_KEYLEN    511

/* define an undefined rank identifier value */
#define WW_RANK_UNDEF     INT32_MAX
/* define a wildcard rank identifier value */
#define WW_RANK_WILDCARD  INT32_MAX-1


/****    WW DATA TYPES    ****/
typedef int16_t ww_data_type_t;
#define WW_UNDEF           0
#define WW_BOOL            1 // converted to/from native true/false to uint8 for pack/unpack
#define WW_BYTE            2 // a byte of data
#define WW_STRING          3 // NULL-terminated string
#define WW_SIZE            4 // size_t
#define WW_PID             5 // OS-pid
#define WW_INT             6
#define WW_INT8            7
#define WW_INT16           8
#define WW_INT32           9
#define WW_INT64          10
#define WW_UINT           11
#define WW_UINT8          12
#define WW_UINT16         13
#define WW_UINT32         14
#define WW_UINT64         15
#define WW_FLOAT          16
#define WW_DOUBLE         17
#define WW_TIMEVAL        18
#define WW_TIME           19
#define WW_STATUS         20 // needs to be tracked separately from integer for those times
                               // when we are embedded and it needs to be converted to the
                               // host error definitions
#define WW_HWLOC_TOPO     21
#define WW_VALUE          22
#define WW_INFO_ARRAY     23
#define WW_PROC           24
#define WW_APP            25
#define WW_INFO           26
#define WW_PDATA          27
#define WW_BUFFER         28
#define WW_BYTE_OBJECT    29
#define WW_KVAL           30
#define WW_MODEX          31
#define WW_PERSIST        32
#define WW_POINTER        33


/****    WW PROC OBJECT    ****/
/* define an object by which we can identify
 * individual processes (e.g., submanagers) */
typedef struct {
    char nspace[WW_MAX_NSLEN+1];
    int rank;
} ww_proc_t;
#define WW_PROC_CREATE(m, n)                                  \
    do {                                                        \
        (m) = (ww_proc_t*)malloc((n) * sizeof(ww_proc_t));  \
        memset((m), 0, (n) * sizeof(ww_proc_t));              \
    } while (0)

#define WW_PROC_RELEASE(m)                    \
    do {                                        \
        WW_PROC_FREE((m));                    \
    } while (0)

#define WW_PROC_CONSTRUCT(m)                  \
    do {                                        \
        memset((m), 0, sizeof(ww_proc_t));    \
    } while (0)

#define WW_PROC_DESTRUCT(m)

#define WW_PROC_FREE(m, n)                    \
    do {                                        \
        if (NULL != (m)) {                      \
            free((m));                          \
        }                                       \
    } while (0)


/****    WW BYTE OBJECT    ****/
typedef struct {
    char *bytes;
    size_t size;
} ww_byte_object_t;

/****    WW VALUE STRUCT    ****/
struct ww_info_t;

typedef struct {
    size_t size;
    struct ww_info_t *array;
} ww_info_array_t;
/* NOTE: operations can supply a collection of values under
 * a single key by passing a ww_value_t containing an
 * array of type WW_INFO_ARRAY, with each array element
 * containing its own ww_info_t object */

typedef struct {
    ww_data_type_t type;
    union {
        bool flag;
        uint8_t byte;
        char *string;
        size_t size;
        pid_t pid;
        int integer;
        int8_t int8;
        int16_t int16;
        int32_t int32;
        int64_t int64;
        unsigned int uint;
        uint8_t uint8;
        uint16_t uint16;
        uint32_t uint32;
        uint64_t uint64;
        float fval;
        double dval;
        struct timeval tv;
        ww_status_t status;
        ww_info_array_t array;
        ww_byte_object_t bo;
        void *ptr;
    } data;
} ww_value_t;
/* allocate and initialize a specified number of value structs */
#define WW_VALUE_CREATE(m, n)                                         \
    do {                                                                \
        int _ii;                                                        \
        (m) = (ww_value_t*)malloc((n) * sizeof(ww_value_t));        \
        memset((m), 0, (n) * sizeof(ww_value_t));                     \
        for (_ii=0; _ii < (int)(n); _ii++) {                            \
            (m)[_ii].type = WW_UNDEF;                                 \
        }                                                               \
    } while (0)

/* release a single ww_value_t struct, including its data */
#define WW_VALUE_RELEASE(m)                                           \
    do {                                                                \
        WW_VALUE_DESTRUCT((m));                                       \
        free((m));                                                      \
    } while (0)

/* initialize a single value struct */
#define WW_VALUE_CONSTRUCT(m)                 \
    do {                                        \
        memset((m), 0, sizeof(ww_value_t));   \
        (m)->type = WW_UNDEF;                 \
    } while (0)

/* release the memory in the value struct data field */
#define WW_VALUE_DESTRUCT(m)                                          \
    do {                                                                \
        if (WW_STRING == (m)->type) {                                 \
            if (NULL != (m)->data.string) {                             \
                free((m)->data.string);                                 \
            }                                                           \
        } else if (WW_BYTE_OBJECT == (m)->type) {                     \
            if (NULL != (m)->data.bo.bytes) {                           \
                free((m)->data.bo.bytes);                               \
            }                                                           \
        } else if (WW_INFO_ARRAY == (m)->type) {                      \
            size_t _n;                                                  \
            ww_info_t *_p = (ww_info_t*)((m)->data.array.array);    \
            for (_n=0; _n < (m)->data.array.size; _n++) {               \
                if (WW_STRING == _p[_n].value.type) {                 \
                    if (NULL != _p[_n].value.data.string) {             \
                        free(_p[_n].value.data.string);                 \
                    }                                                   \
                } else if (WW_BYTE_OBJECT == _p[_n].value.type) {     \
                    if (NULL != _p[_n].value.data.bo.bytes) {           \
                        free(_p[_n].value.data.bo.bytes);               \
                    }                                                   \
                }                                                       \
            }                                                           \
            free(_p);                                                   \
        }                                                               \
    } while (0)

#define WW_VALUE_FREE(m, n)                           \
    do {                                                \
        size_t _s;                                      \
        if (NULL != (m)) {                              \
            for (_s=0; _s < (n); _s++) {                \
                WW_VALUE_DESTRUCT(&((m)[_s]));        \
            }                                           \
            free((m));                                  \
        }                                               \
    } while (0)

/* expose a function that is resolved in the
 * WW library, but part of a header that
 * includes internal functions - so we don't
 * want to expose the entire header here
 */
void ww_value_load(ww_value_t *v, void *data, ww_data_type_t type);




/****    WW INFO STRUCT    ****/
typedef struct {
    char key[WW_MAX_KEYLEN+1];  // ensure room for the NULL terminator
    bool required;                // defaults to optional (i.e., required=false)
    ww_value_t value;
} ww_info_t;

/* utility macros for working with ww_info_t structs */
#define WW_INFO_CREATE(m, n)                                  \
    do {                                                        \
        (m) = (ww_info_t*)malloc((n) * sizeof(ww_info_t));  \
        memset((m), 0, (n) * sizeof(ww_info_t));              \
    } while (0)

#define WW_INFO_CONSTRUCT(m)                  \
    do {                                        \
        memset((m), 0, sizeof(ww_info_t));    \
        (m)->value.type = WW_UNDEF;           \
    } while (0)

#define WW_INFO_DESTRUCT(m) \
    do {                                        \
        WW_VALUE_DESTRUCT(&(m)->value);       \
    } while (0)

#define WW_INFO_FREE(m, n)                    \
    do {                                        \
        size_t _s;                              \
        if (NULL != (m)) {                      \
            for (_s=0; _s < (n); _s++) {        \
                WW_INFO_DESTRUCT(&((m)[_s])); \
            }                                   \
            free((m));                          \
        }                                       \
    } while (0)

#define WW_INFO_LOAD(m, k, v, t)                      \
    do {                                                \
        (void)strncpy((m)->key, (k), WW_MAX_KEYLEN);  \
        ww_value_load(&((m)->value), (v), (t));       \
    } while (0)
#define WW_INFO_REQUIRED(m)       \
    (m)->required = true;
#define WW_INFO_OPTIONAL(m)       \
    (m)->required = false;


#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif
