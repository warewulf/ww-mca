/*
 * Copyright (c) 2008      The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2016      Broadcom Limited. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef WW_SYS_ARCH_TIMER_H
#define WW_SYS_ARCH_TIMER_H 1

#include <sys/times.h>

typedef uint64_t ww_timer_t;

static inline ww_timer_t
ww_sys_timer_get_cycles(void)
{
    ww_timer_t ret;

     __asm__ __volatile__ ("mrs %0,  CNTVCT_EL0" : "=r" (ret));

    return ret;
}


static inline ww_timer_t
ww_sys_timer_freq(void)
{
        ww_timer_t freq;
        __asm__ __volatile__ ("mrs %0,  CNTFRQ_EL0" : "=r" (freq));
        return (ww_timer_t)(freq);
}

#define WW_HAVE_SYS_TIMER_GET_CYCLES 1

#endif /* ! WW_SYS_ARCH_TIMER_H */
