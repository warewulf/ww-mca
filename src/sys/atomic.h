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
 * Copyright (c) 2007      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2011      Sandia National Laboratories. All rights reserved.
 * Copyright (c) 2011-2015 Los Alamos National Security, LLC. All rights
 *                         reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/** @file
 *
 * Atomic operations.
 *
 * This API is patterned after the FreeBSD kernel atomic interface
 * (which is influenced by Intel's ia64 architecture).  The
 * FreeBSD interface is documented at
 *
 * http://www.freebsd.org/cgi/man.cgi?query=atomic&sektion=9
 *
 * Only the necessary subset of functions are implemented here.
 *
 * The following #defines will be true / false based on
 * assembly support:
 *
 *  - \c WW_HAVE_ATOMIC_MEM_BARRIER atomic memory barriers
 *  - \c WW_HAVE_ATOMIC_SPINLOCKS atomic spinlocks
 *  - \c WW_HAVE_ATOMIC_MATH_32 if 32 bit add/sub/cmpset can be done "atomicly"
 *  - \c WW_HAVE_ATOMIC_MATH_64 if 64 bit add/sub/cmpset can be done "atomicly"
 *
 * Note that for the Atomic math, atomic add/sub may be implemented as
 * C code using ww_atomic_cmpset.  The appearance of atomic
 * operation will be upheld in these cases.
 */

#ifndef WW_SYS_ATOMIC_H
#define WW_SYS_ATOMIC_H 1

#include "ww_config.h"

#include "src/sys/architecture.h"
#include "src/include/ww_stdint.h"

/* do some quick #define cleanup in cases where we are doing
   testing... */
#ifdef WW_DISABLE_INLINE_ASM
#undef WW_C_GCC_INLINE_ASSEMBLY
#define WW_C_GCC_INLINE_ASSEMBLY 0
#undef WW_C_DEC_INLINE_ASSEMBLY
#define WW_C_DEC_INLINE_ASSEMBLY 0
#undef WW_C_XLC_INLINE_ASSEMBLY
#define WW_C_XLC_INLINE_ASSEMBLY 0
#endif

/* define WW_{GCC,DEC,XLC}_INLINE_ASSEMBLY based on the
   WW_C_{GCC,DEC,XLC}_INLINE_ASSEMBLY defines and whether we
   are in C or C++ */
#if defined(c_plusplus) || defined(__cplusplus)
/* We no longer support inline assembly for C++ as WW is a C-only interface */
#define WW_GCC_INLINE_ASSEMBLY 0
#define WW_DEC_INLINE_ASSEMBLY 0
#define WW_XLC_INLINE_ASSEMBLY 0
#else
#define WW_GCC_INLINE_ASSEMBLY WW_C_GCC_INLINE_ASSEMBLY
#define WW_DEC_INLINE_ASSEMBLY WW_C_DEC_INLINE_ASSEMBLY
#define WW_XLC_INLINE_ASSEMBLY WW_C_XLC_INLINE_ASSEMBLY
#endif


BEGIN_C_DECLS
/**********************************************************************
 *
 * Data structures for atomic ops
 *
 *********************************************************************/
/**
 * Volatile lock object (with optional padding).
 *
 * \note The internals of the lock are included here, but should be
 * considered private.  The implementation currently in use may choose
 * to use an int or unsigned char as the lock value - the user is not
 * informed either way.
 */
struct ww_atomic_lock_t {
    union {
        volatile int32_t lock;     /**< The lock address (an integer) */
        volatile unsigned char sparc_lock; /**< The lock address on sparc */
        char padding[sizeof(int)]; /**< Array for optional padding */
    } u;
};
typedef struct ww_atomic_lock_t ww_atomic_lock_t;

/**********************************************************************
 *
 * Set or unset these macros in the architecture-specific atomic.h
 * files if we need to specify them as inline or non-inline
 *
 *********************************************************************/
#if !WW_GCC_INLINE_ASSEMBLY
#define WW_HAVE_INLINE_ATOMIC_MEM_BARRIER 0
#define WW_HAVE_INLINE_ATOMIC_CMPSET_32 0
#define WW_HAVE_INLINE_ATOMIC_CMPSET_64 0
#define WW_HAVE_INLINE_ATOMIC_ADD_32 0
#define WW_HAVE_INLINE_ATOMIC_SUB_32 0
#define WW_HAVE_INLINE_ATOMIC_ADD_64 0
#define WW_HAVE_INLINE_ATOMIC_SUB_64 0
#define WW_HAVE_INLINE_ATOMIC_SWAP_32 0
#define WW_HAVE_INLINE_ATOMIC_SWAP_64 0
#else
#define WW_HAVE_INLINE_ATOMIC_MEM_BARRIER 1
#define WW_HAVE_INLINE_ATOMIC_CMPSET_32 1
#define WW_HAVE_INLINE_ATOMIC_CMPSET_64 1
#define WW_HAVE_INLINE_ATOMIC_ADD_32 1
#define WW_HAVE_INLINE_ATOMIC_SUB_32 1
#define WW_HAVE_INLINE_ATOMIC_ADD_64 1
#define WW_HAVE_INLINE_ATOMIC_SUB_64 1
#define WW_HAVE_INLINE_ATOMIC_SWAP_32 1
#define WW_HAVE_INLINE_ATOMIC_SWAP_64 1
#endif

/**
 * Enumeration of lock states
 */
enum {
    WW_ATOMIC_UNLOCKED = 0,
    WW_ATOMIC_LOCKED = 1
};

/**********************************************************************
 *
 * Load the appropriate architecture files and set some reasonable
 * default values for our support
 *
 *********************************************************************/
#if defined(DOXYGEN)
/* don't include system-level gorp when generating doxygen files */
#elif WW_ASSEMBLY_BUILTIN == WW_BUILTIN_SYNC
#include "src/sys/sync_builtin/atomic.h"
#elif WW_ASSEMBLY_BUILTIN == WW_BUILTIN_GCC
#include "src/sys/gcc_builtin/atomic.h"
#elif WW_ASSEMBLY_BUILTIN == WW_BUILTIN_OSX
#include "src/sys/osx/atomic.h"
#elif WW_ASSEMBLY_ARCH == WW_AMD64
#include "src/sys/amd64/atomic.h"
#elif WW_ASSEMBLY_ARCH == WW_ARM
#include "src/sys/arm/atomic.h"
#elif WW_ASSEMBLY_ARCH == WW_ARM64
#include "src/sys/arm64/atomic.h"
#elif WW_ASSEMBLY_ARCH == WW_IA32
#include "src/sys/ia32/atomic.h"
#elif WW_ASSEMBLY_ARCH == WW_IA64
#include "src/sys/ia64/atomic.h"
#elif WW_ASSEMBLY_ARCH == WW_MIPS
#include "src/sys/mips/atomic.h"
#elif WW_ASSEMBLY_ARCH == WW_POWERPC32
#include "src/sys/powerpc/atomic.h"
#elif WW_ASSEMBLY_ARCH == WW_POWERPC64
#include "src/sys/powerpc/atomic.h"
#elif WW_ASSEMBLY_ARCH == WW_SPARC
#include "src/sys/sparc/atomic.h"
#elif WW_ASSEMBLY_ARCH == WW_SPARCV9_32
#include "src/sys/sparcv9/atomic.h"
#elif WW_ASSEMBLY_ARCH == WW_SPARCV9_64
#include "src/sys/sparcv9/atomic.h"
#endif

#ifndef DOXYGEN
/* compare and set operations can't really be emulated from software,
   so if these defines aren't already set, they should be set to 0
   now */
#ifndef WW_HAVE_ATOMIC_CMPSET_32
#define WW_HAVE_ATOMIC_CMPSET_32 0
#endif
#ifndef WW_HAVE_ATOMIC_CMPSET_64
#define WW_HAVE_ATOMIC_CMPSET_64 0
#endif
#ifndef WW_HAVE_ATOMIC_CMPSET_128
#define WW_HAVE_ATOMIC_CMPSET_128 0
#endif
#ifndef WW_HAVE_ATOMIC_LLSC_32
#define WW_HAVE_ATOMIC_LLSC_32 0
#endif
#ifndef WW_HAVE_ATOMIC_LLSC_64
#define WW_HAVE_ATOMIC_LLSC_64 0
#endif
#endif /* DOXYGEN */

/**********************************************************************
 *
 * Memory Barriers - defined here if running doxygen or have barriers
 *                   but can't inline
 *
 *********************************************************************/
#if !defined(WW_HAVE_ATOMIC_MEM_BARRIER) && !defined(DOXYGEN)
/* no way to emulate in C code */
#define WW_HAVE_ATOMIC_MEM_BARRIER 0
#endif

#if defined(DOXYGEN) || WW_HAVE_ATOMIC_MEM_BARRIER
/**
 * Memory barrier
 *
 * Will use system-specific features to instruct the processor and
 * memory controller that all writes and reads that have been posted
 * before the call to \c ww_atomic_mb() must appear to have
 * completed before the next read or write.
 *
 * \note This can have some expensive side effects, including flushing
 * the pipeline, preventing the cpu from reordering instructions, and
 * generally grinding the memory controller's performance.  Use only
 * if you need *both* read and write barriers.
 */

#if WW_HAVE_INLINE_ATOMIC_MEM_BARRIER
static inline
#endif
void ww_atomic_mb(void);

/**
 * Read memory barrier
 *
 * Use system-specific features to instruct the processor and memory
 * conrtoller that all reads that have been posted before the call to
 * \c ww_atomic_rmb() must appear to have been completed before the
 * next read.  Nothing is said about the ordering of writes when using
 * \c ww_atomic_rmb().
 */

#if WW_HAVE_INLINE_ATOMIC_MEM_BARRIER
static inline
#endif
void ww_atomic_rmb(void);

/**
 * Write memory barrier.
 *
 * Use system-specific features to instruct the processor and memory
 * conrtoller that all writes that have been posted before the call to
 * \c ww_atomic_wmb() must appear to have been completed before the
 * next write.  Nothing is said about the ordering of reads when using
 * \c ww_atomic_wmb().
 */

#if WW_HAVE_INLINE_ATOMIC_MEM_BARRIER
static inline
#endif
void ww_atomic_wmb(void);

#endif /* defined(DOXYGEN) || WW_HAVE_ATOMIC_MEM_BARRIER */


/**********************************************************************
 *
 * Atomic spinlocks - always inlined, if have atomic cmpset
 *
 *********************************************************************/

#if !defined(WW_HAVE_ATOMIC_SPINLOCKS) && !defined(DOXYGEN)
/* 0 is more like "pending" - we'll fix up at the end after all
   the static inline functions are declared */
#define WW_HAVE_ATOMIC_SPINLOCKS 0
#endif

#if defined(DOXYGEN) || WW_HAVE_ATOMIC_SPINLOCKS || (WW_HAVE_ATOMIC_CMPSET_32 || WW_HAVE_ATOMIC_CMPSET_64)

/**
 * Initialize a lock to value
 *
 * @param lock         Address of the lock
 * @param value        Initial value to set lock to
 */
#if WW_HAVE_ATOMIC_SPINLOCKS == 0
static inline
#endif
void ww_atomic_init(ww_atomic_lock_t* lock, int32_t value);


/**
 * Try to acquire a lock.
 *
 * @param lock          Address of the lock.
 * @return              0 if the lock was acquired, 1 otherwise.
 */
#if WW_HAVE_ATOMIC_SPINLOCKS == 0
static inline
#endif
int ww_atomic_trylock(ww_atomic_lock_t *lock);


/**
 * Acquire a lock by spinning.
 *
 * @param lock          Address of the lock.
 */
#if WW_HAVE_ATOMIC_SPINLOCKS == 0
static inline
#endif
void ww_atomic_lock(ww_atomic_lock_t *lock);


/**
 * Release a lock.
 *
 * @param lock          Address of the lock.
 */
#if WW_HAVE_ATOMIC_SPINLOCKS == 0
static inline
#endif
void ww_atomic_unlock(ww_atomic_lock_t *lock);


#if WW_HAVE_ATOMIC_SPINLOCKS == 0
#undef WW_HAVE_ATOMIC_SPINLOCKS
#define WW_HAVE_ATOMIC_SPINLOCKS (WW_HAVE_ATOMIC_CMPSET_32 || WW_HAVE_ATOMIC_CMPSET_64)
#define WW_NEED_INLINE_ATOMIC_SPINLOCKS 1
#endif

#endif /* WW_HAVE_ATOMIC_SPINLOCKS */


/**********************************************************************
 *
 * Atomic math operations
 *
 *********************************************************************/
#if !defined(WW_HAVE_ATOMIC_CMPSET_32) && !defined(DOXYGEN)
#define WW_HAVE_ATOMIC_CMPSET_32 0
#endif
#if defined(DOXYGEN) || WW_HAVE_ATOMIC_CMPSET_32

#if WW_HAVE_INLINE_ATOMIC_CMPSET_32
static inline
#endif
int ww_atomic_cmpset_32(volatile int32_t *addr, int32_t oldval,
                          int32_t newval);

#if WW_HAVE_INLINE_ATOMIC_CMPSET_32
static inline
#endif
int ww_atomic_cmpset_acq_32(volatile int32_t *addr, int32_t oldval,
                              int32_t newval);

#if WW_HAVE_INLINE_ATOMIC_CMPSET_32
static inline
#endif
int ww_atomic_cmpset_rel_32(volatile int32_t *addr, int32_t oldval,
                              int32_t newval);
#endif


#if !defined(WW_HAVE_ATOMIC_CMPSET_64) && !defined(DOXYGEN)
#define WW_HAVE_ATOMIC_CMPSET_64 0
#endif
#if defined(DOXYGEN) || WW_HAVE_ATOMIC_CMPSET_64

#if WW_HAVE_INLINE_ATOMIC_CMPSET_64
static inline
#endif
int ww_atomic_cmpset_64(volatile int64_t *addr, int64_t oldval,
                          int64_t newval);

#if WW_HAVE_INLINE_ATOMIC_CMPSET_64
static inline
#endif
int ww_atomic_cmpset_acq_64(volatile int64_t *addr, int64_t oldval,
                              int64_t newval);

#if WW_HAVE_INLINE_ATOMIC_CMPSET_64
static inline
#endif
int ww_atomic_cmpset_rel_64(volatile int64_t *addr, int64_t oldval,
                              int64_t newval);

#endif

#if !defined(WW_HAVE_ATOMIC_MATH_32) && !defined(DOXYGEN)
  /* define to 0 for these tests.  WIll fix up later. */
  #define WW_HAVE_ATOMIC_MATH_32 0
#endif

#if defined(DOXYGEN) || WW_HAVE_ATOMIC_MATH_32 || WW_HAVE_ATOMIC_CMPSET_32

/* WW_HAVE_INLINE_ATOMIC_*_32 will be 1 if <arch>/atomic.h provides
   a static inline version of it (in assembly).  If we have to fall
   back on cmpset 32, that too will be inline. */
#if WW_HAVE_INLINE_ATOMIC_ADD_32 || (!defined(WW_HAVE_ATOMIC_ADD_32) && WW_HAVE_ATOMIC_CMPSET_32)
static inline
#endif
int32_t ww_atomic_add_32(volatile int32_t *addr, int delta);

/* WW_HAVE_INLINE_ATOMIC_*_32 will be 1 if <arch>/atomic.h provides
   a static inline version of it (in assembly).  If we have to fall
   back to cmpset 32, that too will be inline. */
#if WW_HAVE_INLINE_ATOMIC_SUB_32 || (!defined(WW_HAVE_ATOMIC_ADD_32) && WW_HAVE_ATOMIC_CMPSET_32)
static inline
#endif
int32_t ww_atomic_sub_32(volatile int32_t *addr, int delta);

#endif /* WW_HAVE_ATOMIC_MATH_32 */

#if ! WW_HAVE_ATOMIC_MATH_32
/* fix up the value of ww_have_atomic_math_32 to allow for C versions */
#undef WW_HAVE_ATOMIC_MATH_32
#define WW_HAVE_ATOMIC_MATH_32 WW_HAVE_ATOMIC_CMPSET_32
#endif

#ifndef WW_HAVE_ATOMIC_MATH_64
/* define to 0 for these tests.  WIll fix up later. */
#define WW_HAVE_ATOMIC_MATH_64 0
#endif

#if defined(DOXYGEN) || WW_HAVE_ATOMIC_MATH_64 || WW_HAVE_ATOMIC_CMPSET_64

/* WW_HAVE_INLINE_ATOMIC_*_64 will be 1 if <arch>/atomic.h provides
   a static inline version of it (in assembly).  If we have to fall
   back to cmpset 64, that too will be inline */
#if WW_HAVE_INLINE_ATOMIC_ADD_64 || (!defined(WW_HAVE_ATOMIC_ADD_64) && WW_HAVE_ATOMIC_CMPSET_64)
static inline
#endif
int64_t ww_atomic_add_64(volatile int64_t *addr, int64_t delta);

/* WW_HAVE_INLINE_ATOMIC_*_64 will be 1 if <arch>/atomic.h provides
   a static inline version of it (in assembly).  If we have to fall
   back to cmpset 64, that too will be inline */
#if WW_HAVE_INLINE_ATOMIC_SUB_64 || (!defined(WW_HAVE_ATOMIC_ADD_64) && WW_HAVE_ATOMIC_CMPSET_64)
static inline
#endif
int64_t ww_atomic_sub_64(volatile int64_t *addr, int64_t delta);

#endif /* WW_HAVE_ATOMIC_MATH_32 */

#if ! WW_HAVE_ATOMIC_MATH_64
/* fix up the value of ww_have_atomic_math_64 to allow for C versions */
#undef WW_HAVE_ATOMIC_MATH_64
#define WW_HAVE_ATOMIC_MATH_64 WW_HAVE_ATOMIC_CMPSET_64
#endif

/* provide a size_t add/subtract.  When in debug mode, make it an
 * inline function so that we don't have any casts in the
 *  interface and can catch type errors.  When not in debug mode,
 * just make it a macro, so that there's no performance penalty
 */
#if defined(DOXYGEN) || WW_ENABLE_DEBUG
static inline size_t
ww_atomic_add_size_t(volatile size_t *addr, int delta)
{
#if SIZEOF_SIZE_T == 4
    return (size_t) ww_atomic_add_32((int32_t*) addr, delta);
#elif SIZEOF_SIZE_T == 8
    return (size_t) ww_atomic_add_64((int64_t*) addr, delta);
#else
#error "Unknown size_t size"
#endif
}
static inline size_t
ww_atomic_sub_size_t(volatile size_t *addr, int delta)
{
#if SIZEOF_SIZE_T == 4
    return (size_t) ww_atomic_sub_32((int32_t*) addr, delta);
#elif SIZEOF_SIZE_T == 8
    return (size_t) ww_atomic_sub_64((int64_t*) addr, delta);
#else
#error "Unknown size_t size"
#endif
}
#else
#if SIZEOF_SIZE_T == 4
#define ww_atomic_add_size_t(addr, delta) ((size_t) ww_atomic_add_32((int32_t*) addr, delta))
#define ww_atomic_sub_size_t(addr, delta) ((size_t) ww_atomic_sub_32((int32_t*) addr, delta))
#elif SIZEOF_SIZE_T ==8
#define ww_atomic_add_size_t(addr, delta) ((size_t) ww_atomic_add_64((int64_t*) addr, delta))
#define ww_atomic_sub_size_t(addr, delta) ((size_t) ww_atomic_sub_64((int64_t*) addr, delta))
#else
#error "Unknown size_t size"
#endif
#endif

#if defined(DOXYGEN) || (WW_HAVE_ATOMIC_CMPSET_32 || WW_HAVE_ATOMIC_CMPSET_64)
/* these are always done with inline functions, so always mark as
   static inline */
static inline int ww_atomic_cmpset_xx(volatile void* addr, int64_t oldval,
                                        int64_t newval, size_t length);
static inline int ww_atomic_cmpset_acq_xx(volatile void* addr,
                                            int64_t oldval,  int64_t newval,
                                            size_t length);
static inline int ww_atomic_cmpset_rel_xx(volatile void* addr,
                                            int64_t oldval, int64_t newval,
                                            size_t length);

static inline int ww_atomic_cmpset_ptr(volatile void* addr,
                                         void* oldval,
                                         void* newval);
static inline int ww_atomic_cmpset_acq_ptr(volatile void* addr,
                                             void* oldval,
                                             void* newval);
static inline int ww_atomic_cmpset_rel_ptr(volatile void* addr,
                                             void* oldval,
                                             void* newval);

/**
 * Atomic compare and set of pointer with relaxed semantics. This
 * macro detect at compile time the type of the first argument and
 * choose the correct function to be called.
 *
 * \note This macro should only be used for integer types.
 *
 * @param addr          Address of <TYPE>.
 * @param oldval        Comparison value <TYPE>.
 * @param newval        New value to set if comparision is true <TYPE>.
 *
 * See ww_atomic_cmpset_* for pseudo-code.
 */
#define ww_atomic_cmpset( ADDR, OLDVAL, NEWVAL )                  \
   ww_atomic_cmpset_xx( (volatile void*)(ADDR), (intptr_t)(OLDVAL), \
                          (intptr_t)(NEWVAL), sizeof(*(ADDR)) )

/**
 * Atomic compare and set of pointer with acquire semantics. This
 * macro detect at compile time the type of the first argument
 * and choose the correct function to be called.
 *
 * \note This macro should only be used for integer types.
 *
 * @param addr          Address of <TYPE>.
 * @param oldval        Comparison value <TYPE>.
 * @param newval        New value to set if comparision is true <TYPE>.
 *
 * See ww_atomic_cmpset_acq_* for pseudo-code.
 */
#define ww_atomic_cmpset_acq( ADDR, OLDVAL, NEWVAL )           \
   ww_atomic_cmpset_acq_xx( (volatile void*)(ADDR), (int64_t)(OLDVAL), \
                              (int64_t)(NEWVAL), sizeof(*(ADDR)) )


/**
 * Atomic compare and set of pointer with release semantics. This
 * macro detect at compile time the type of the first argument
 * and choose the correct function to b
 *
 * \note This macro should only be used for integer types.
 *
 * @param addr          Address of <TYPE>.
 * @param oldval        Comparison value <TYPE>.
 * @param newval        New value to set if comparision is true <TYPE>.
 *
 * See ww_atomic_cmpsetrel_* for pseudo-code.
 */
#define ww_atomic_cmpset_rel( ADDR, OLDVAL, NEWVAL )           \
   ww_atomic_cmpset_rel_xx( (volatile void*)(ADDR), (int64_t)(OLDVAL), \
                              (int64_t)(NEWVAL), sizeof(*(ADDR)) )

#endif /* (WW_HAVE_ATOMIC_CMPSET_32 || WW_HAVE_ATOMIC_CMPSET_64) */

#if defined(DOXYGEN) || (WW_HAVE_ATOMIC_MATH_32 || WW_HAVE_ATOMIC_MATH_64)

static inline void ww_atomic_add_xx(volatile void* addr,
                                      int32_t value, size_t length);
static inline void ww_atomic_sub_xx(volatile void* addr,
                                      int32_t value, size_t length);
#if SIZEOF_VOID_P == 4 && WW_HAVE_ATOMIC_CMPSET_32
static inline int32_t ww_atomic_add_ptr( volatile void* addr, void* delta );
static inline int32_t ww_atomic_sub_ptr( volatile void* addr, void* delta );
#elif SIZEOF_VOID_P == 8 && WW_HAVE_ATOMIC_CMPSET_64
static inline int64_t ww_atomic_add_ptr( volatile void* addr, void* delta );
static inline int64_t ww_atomic_sub_ptr( volatile void* addr, void* delta );
#else
#error Atomic arithmetic on pointers not supported
#endif

/**
 * Atomically increment the content depending on the type. This
 * macro detect at compile time the type of the first argument
 * and choose the correct function to be called.
 *
 * \note This macro should only be used for integer types.
 *
 * @param addr          Address of <TYPE>
 * @param delta         Value to add (converted to <TYPE>).
 */
#define ww_atomic_add( ADDR, VALUE )                                  \
   ww_atomic_add_xx( (volatile void*)(ADDR), (int32_t)(VALUE), \
                       sizeof(*(ADDR)) )

/**
 * Atomically decrement the content depending on the type. This
 * macro detect at compile time the type of the first argument
 * and choose the correct function to be called.
 *
 * \note This macro should only be used for integer types.
 *
 * @param addr          Address of <TYPE>
 * @param delta         Value to substract (converted to <TYPE>).
 */
#define ww_atomic_sub( ADDR, VALUE )                                  \
   ww_atomic_sub_xx( (volatile void*)(ADDR), (int32_t)(VALUE),        \
                      sizeof(*(ADDR)) )

#endif /* WW_HAVE_ATOMIC_MATH_32 || WW_HAVE_ATOMIC_MATH_64 */


/*
 * Include inline implementations of everything not defined directly
 * in assembly
 */
#include "src/sys/atomic_impl.h"

END_C_DECLS

#endif /* WW_SYS_ATOMIC_H */
