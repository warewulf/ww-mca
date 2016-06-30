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
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef WW_SYS_ARCH_TIMER_H
#define WW_SYS_ARCH_TIMER_H 1

typedef uint64_t ww_timer_t;

#if WW_GCC_INLINE_ASSEMBLY


#if WW_ASSEMBLY_ARCH == WW_SPARCV9_64

static inline ww_timer_t
ww_sys_timer_get_cycles(void)
{
    ww_timer_t ret;

    __asm__ __volatile__("rd %%tick, %0" : "=r"(ret));

    return ret;
}

#else /* WW_SPARCV9_32 */

static inline ww_timer_t
ww_sys_timer_get_cycles(void)
{
    ww_timer_t ret;
    int a, b;

    __asm__ __volatile__("rd %%tick, %0    \n"
                         "srlx %0, 32, %1 " :
                         "=r"(a), "=r"(b)
                         );

    ret = (0x00000000FFFFFFFF & a) | (((ww_timer_t) b) << 32);

    return ret;
}

#endif

#define WW_HAVE_SYS_TIMER_GET_CYCLES 1

#else

#define WW_HAVE_SYS_TIMER_GET_CYCLES 0

#endif /* WW_GCC_INLINE_ASSEMBLY */

#endif /* ! WW_SYS_ARCH_TIMER_H */
