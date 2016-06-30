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
 * Copyright (c) 2016      Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "ww_config.h"

#include "src/sys/atomic.h"
#include "src/sys/architecture.h"

#if WW_ASSEMBLY_ARCH == WW_SPARC

#define LOCKS_TABLE_SIZE 8
/* make sure to get into reasonably useful bits (so shift at least 5) */
#define FIND_LOCK(addr) (&(locks_table[(((unsigned long) addr) >> 8) &  \
                                       (LOCKS_TABLE_SIZE - 1)]))

/* have to fix if you change LOCKS_TABLE_SIZE */
static ww_atomic_lock_t locks_table[LOCKS_TABLE_SIZE] = {
    { { WW_ATOMIC_UNLOCKED } },
    { { WW_ATOMIC_UNLOCKED } },
    { { WW_ATOMIC_UNLOCKED } },
    { { WW_ATOMIC_UNLOCKED } },
    { { WW_ATOMIC_UNLOCKED } },
    { { WW_ATOMIC_UNLOCKED } },
    { { WW_ATOMIC_UNLOCKED } },
    { { WW_ATOMIC_UNLOCKED } }
};


int32_t
ww_atomic_add_32(volatile int32_t *addr, int delta)
{
    int32_t ret;

    ww_atomic_lock(FIND_LOCK(addr));

    ret = (*addr += delta);

    ww_atomic_unlock(FIND_LOCK(addr));

    return ret;
}


int32_t
ww_atomic_sub_32(volatile int32_t *addr, int delta)
{
    int32_t ret;

    ww_atomic_lock(FIND_LOCK(addr));

    ret = (*addr -= delta);

    ww_atomic_unlock(FIND_LOCK(addr));

    return ret;
}


#endif /* WW_ASSEMBLY_ARCH == WW_SPARC32 */
