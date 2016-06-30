/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2014 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2016      Broadcom Limited. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/** @file
 *
 * Cycle counter reading instructions.  Do not use directly - see the
 * timer interface instead
 */

#ifndef WW_SYS_TIMER_H
#define WW_SYS_TIMER_H 1

#include "ww_config.h"

#include "ww/sys/architecture.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

/* do some quick #define cleanup in cases where we are doing
   testing... */
#ifdef WW_DISABLE_INLINE_ASM
#undef WW_C_GCC_INLINE_ASSEMBLY
#define WW_C_GCC_INLINE_ASSEMBLY 0
#undef WW_CXX_GCC_INLINE_ASSEMBLY
#define WW_CXX_GCC_INLINE_ASSEMBLY 0
#undef WW_C_DEC_INLINE_ASSEMBLY
#define WW_C_DEC_INLINE_ASSEMBLY 0
#undef WW_CXX_DEC_INLINE_ASSEMBLY
#define WW_CXX_DEC_INLINE_ASSEMBLY 0
#undef WW_C_XLC_INLINE_ASSEMBLY
#define WW_C_XLC_INLINE_ASSEMBLY 0
#undef WW_CXX_XLC_INLINE_ASSEMBLY
#define WW_CXX_XLC_INLINE_ASSEMBLY 0
#endif

/* define WW_{GCC,DEC,XLC}_INLINE_ASSEMBLY based on the
   WW_{C,CXX}_{GCC,DEC,XLC}_INLINE_ASSEMBLY defines and whether we
   are in C or C++ */
#if defined(c_plusplus) || defined(__cplusplus)
#define WW_GCC_INLINE_ASSEMBLY WW_CXX_GCC_INLINE_ASSEMBLY
#define WW_DEC_INLINE_ASSEMBLY WW_CXX_DEC_INLINE_ASSEMBLY
#define WW_XLC_INLINE_ASSEMBLY WW_CXX_XLC_INLINE_ASSEMBLY
#else
#define WW_GCC_INLINE_ASSEMBLY WW_C_GCC_INLINE_ASSEMBLY
#define WW_DEC_INLINE_ASSEMBLY WW_C_DEC_INLINE_ASSEMBLY
#define WW_XLC_INLINE_ASSEMBLY WW_C_XLC_INLINE_ASSEMBLY
#endif

/**********************************************************************
 *
 * Load the appropriate architecture files and set some reasonable
 * default values for our support
 *
 *********************************************************************/

/* By default we suppose all timers are monotonic per node. */
#define WW_TIMER_MONOTONIC 1

BEGIN_C_DECLS

/* If you update this list, you probably also want to update
   ww/mca/timer/linux/configure.m4.  Or not. */

#if defined(DOXYGEN)
/* don't include system-level gorp when generating doxygen files */
#elif WW_ASSEMBLY_ARCH == WW_AMD64
#include "ww/sys/amd64/timer.h"
#elif WW_ASSEMBLY_ARCH == WW_ARM
#include "ww/sys/arm/timer.h"
#elif WW_ASSEMBLY_ARCH == WW_ARM64
#include "ww/sys/arm64/timer.h"
#elif WW_ASSEMBLY_ARCH == WW_IA32
#include "ww/sys/ia32/timer.h"
#elif WW_ASSEMBLY_ARCH == WW_IA64
#include "ww/sys/ia64/timer.h"
#elif WW_ASSEMBLY_ARCH == WW_POWERPC32
#include "ww/sys/powerpc/timer.h"
#elif WW_ASSEMBLY_ARCH == WW_POWERPC64
#include "ww/sys/powerpc/timer.h"
#elif WW_ASSEMBLY_ARCH == WW_SPARCV9_32
#include "ww/sys/sparcv9/timer.h"
#elif WW_ASSEMBLY_ARCH == WW_SPARCV9_64
#include "ww/sys/sparcv9/timer.h"
#elif WW_ASSEMBLY_ARCH == WW_MIPS
#include "ww/sys/mips/timer.h"
#endif

#ifndef DOXYGEN
#ifndef WW_HAVE_SYS_TIMER_GET_CYCLES
#define WW_HAVE_SYS_TIMER_GET_CYCLES 0

typedef long ww_timer_t;
#endif
#endif

END_C_DECLS

#endif /* WW_SYS_TIMER_H */
