/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2010 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Sun Microsystems, Inc.  All rights reserverd.
 * Copyright (c) 2012-2014 Los Alamos National Security, LLC. All rights
 *                         reserved.
 * Copyright (c) 2016      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#ifndef WW_SYS_ARCH_ATOMIC_H
#define WW_SYS_ARCH_ATOMIC_H 1

/*
 * On amd64, we use cmpxchg.
 */


#define SMPLOCK "lock; "
#define MB() __asm__ __volatile__("": : :"memory")


/**********************************************************************
 *
 * Define constants for AMD64 / x86_64 / EM64T / ...
 *
 *********************************************************************/
#define WW_HAVE_ATOMIC_MEM_BARRIER 1

#define WW_HAVE_ATOMIC_CMPSET_32 1

#define WW_HAVE_ATOMIC_CMPSET_64 1

/**********************************************************************
 *
 * Memory Barriers
 *
 *********************************************************************/
#if WW_GCC_INLINE_ASSEMBLY

static inline void ww_atomic_mb(void)
{
    MB();
}


static inline void ww_atomic_rmb(void)
{
    MB();
}


static inline void ww_atomic_wmb(void)
{
    MB();
}

static inline void ww_atomic_isync(void)
{
}

#endif /* WW_GCC_INLINE_ASSEMBLY */


/**********************************************************************
 *
 * Atomic math operations
 *
 *********************************************************************/
#if WW_GCC_INLINE_ASSEMBLY

static inline int ww_atomic_cmpset_32( volatile int32_t *addr,
                                        int32_t oldval, int32_t newval)
{
   unsigned char ret;
   __asm__ __volatile__ (
                       SMPLOCK "cmpxchgl %3,%2   \n\t"
                               "sete     %0      \n\t"
                       : "=qm" (ret), "+a" (oldval), "+m" (*addr)
                       : "q"(newval)
                       : "memory", "cc");

   return (int)ret;
}

#endif /* WW_GCC_INLINE_ASSEMBLY */

#define ww_atomic_cmpset_acq_32 ww_atomic_cmpset_32
#define ww_atomic_cmpset_rel_32 ww_atomic_cmpset_32

#if WW_GCC_INLINE_ASSEMBLY

static inline int ww_atomic_cmpset_64( volatile int64_t *addr,
                                         int64_t oldval, int64_t newval)
{
   unsigned char ret;
   __asm__ __volatile__ (
                       SMPLOCK "cmpxchgq %3,%2   \n\t"
                               "sete     %0      \n\t"
                       : "=qm" (ret), "+a" (oldval), "+m" (*((volatile long*)addr))
                       : "q"(newval)
                       : "memory", "cc"
                       );

   return (int)ret;
}

#endif /* WW_GCC_INLINE_ASSEMBLY */

#define ww_atomic_cmpset_acq_64 ww_atomic_cmpset_64
#define ww_atomic_cmpset_rel_64 ww_atomic_cmpset_64

#if WW_GCC_INLINE_ASSEMBLY && WW_HAVE_CMPXCHG16B && HAVE_WW_INT128_T

static inline int ww_atomic_cmpset_128 (volatile ww_int128_t *addr, ww_int128_t oldval,
                                          ww_int128_t newval)
{
    unsigned char ret;

    /* cmpxchg16b compares the value at the address with eax:edx (low:high). if the values are
     * the same the contents of ebx:ecx are stores at the address. in all cases the value stored
     * at the address is returned in eax:edx. */
    __asm__ __volatile__ (SMPLOCK "cmpxchg16b (%%rsi)   \n\t"
                                  "sete     %0      \n\t"
                          : "=qm" (ret)
                          : "S" (addr), "b" (((int64_t *)&newval)[0]), "c" (((int64_t *)&newval)[1]),
                            "a" (((int64_t *)&oldval)[0]), "d" (((int64_t *)&oldval)[1])
                          : "memory", "cc");

    return (int) ret;
}

#define WW_HAVE_ATOMIC_CMPSET_128 1

#endif /* WW_GCC_INLINE_ASSEMBLY */


#if WW_GCC_INLINE_ASSEMBLY

#define WW_HAVE_ATOMIC_SWAP_32 1

#define WW_HAVE_ATOMIC_SWAP_64 1

static inline int32_t ww_atomic_swap_32( volatile int32_t *addr,
					   int32_t newval)
{
    int32_t oldval;

    __asm__ __volatile__("xchg %1, %0" :
			 "=r" (oldval), "+m" (*addr) :
			 "0" (newval) :
			 "memory");
    return oldval;
}

#endif /* WW_GCC_INLINE_ASSEMBLY */

#if WW_GCC_INLINE_ASSEMBLY

static inline int64_t ww_atomic_swap_64( volatile int64_t *addr,
                                           int64_t newval)
{
    int64_t oldval;

    __asm__ __volatile__("xchgq %1, %0" :
			 "=r" (oldval), "+m" (*addr) :
			 "0" (newval) :
			 "memory");
    return oldval;
}

#endif /* WW_GCC_INLINE_ASSEMBLY */



#if WW_GCC_INLINE_ASSEMBLY

#define WW_HAVE_ATOMIC_MATH_32 1
#define WW_HAVE_ATOMIC_MATH_64 1

#define WW_HAVE_ATOMIC_ADD_32 1

/**
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type int
 *
 * Atomically adds @i to @v.
 */
static inline int32_t ww_atomic_add_32(volatile int32_t* v, int i)
{
    int ret = i;
   __asm__ __volatile__(
                        SMPLOCK "xaddl %1,%0"
                        :"+m" (*v), "+r" (ret)
                        :
                        :"memory", "cc"
                        );
   return (ret+i);
}

#define WW_HAVE_ATOMIC_ADD_64 1

/**
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type int
 *
 * Atomically adds @i to @v.
 */
static inline int64_t ww_atomic_add_64(volatile int64_t* v, int64_t i)
{
    int64_t ret = i;
   __asm__ __volatile__(
                        SMPLOCK "xaddq %1,%0"
                        :"+m" (*v), "+r" (ret)
                        :
                        :"memory", "cc"
                        );
   return (ret+i);
}

#define WW_HAVE_ATOMIC_SUB_32 1

/**
 * atomic_sub - subtract the atomic variable
 * @i: integer value to subtract
 * @v: pointer of type int
 *
 * Atomically subtracts @i from @v.
 */
static inline int32_t ww_atomic_sub_32(volatile int32_t* v, int i)
{
    int ret = -i;
   __asm__ __volatile__(
                        SMPLOCK "xaddl %1,%0"
                        :"+m" (*v), "+r" (ret)
                        :
                        :"memory", "cc"
                        );
   return (ret-i);
}

#define WW_HAVE_ATOMIC_SUB_64 1

/**
 * atomic_sub - subtract the atomic variable
 * @i: integer value to subtract
 * @v: pointer of type int
 *
 * Atomically subtracts @i from @v.
 */
static inline int64_t ww_atomic_sub_64(volatile int64_t* v, int64_t i)
{
    int64_t ret = -i;
   __asm__ __volatile__(
                        SMPLOCK "xaddq %1,%0"
                        :"+m" (*v), "+r" (ret)
                        :
                        :"memory", "cc"
                        );
   return (ret-i);
}

#endif /* WW_GCC_INLINE_ASSEMBLY */

#endif /* ! WW_SYS_ARCH_ATOMIC_H */
