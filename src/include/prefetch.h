/*
 * Copyright (c) 2004-2006 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2014      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/** @file
 *
 * Compiler-specific prefetch functions
 *
 * A small set of prefetch / prediction interfaces for using compiler
 * directives to improve memory prefetching and branch prediction
 */

#ifndef WW_PREFETCH_H
#define WW_PREFETCH_H

#if WW_C_HAVE_BUILTIN_EXPECT
#define WW_LIKELY(expression) __builtin_expect(!!(expression), 1)
#define WW_UNLIKELY(expression) __builtin_expect(!!(expression), 0)
#else
#define WW_LIKELY(expression) (expression)
#define WW_UNLIKELY(expression) (expression)
#endif

#if WW_C_HAVE_BUILTIN_PREFETCH
#define WW_PREFETCH(address,rw,locality) __builtin_prefetch(address,rw,locality)
#else
#define WW_PREFETCH(address,rw,locality)
#endif

#endif
