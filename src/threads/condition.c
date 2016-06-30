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

#include "ww_config.h"

#include "src/threads/condition.h"


static void ww_condition_construct(ww_condition_t *c)
{
    c->c_waiting = 0;
    c->c_signaled = 0;
}


static void ww_condition_destruct(ww_condition_t *c)
{
}

WW_CLASS_INSTANCE(ww_condition_t,
                   ww_object_t,
                   ww_condition_construct,
                   ww_condition_destruct);
