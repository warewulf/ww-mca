/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2007 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>

#include "src/class/ww_value_array.h"


static void ww_value_array_construct(ww_value_array_t* array)
{
    array->array_items = NULL;
    array->array_size = 0;
    array->array_item_sizeof = 0;
    array->array_alloc_size = 0;
}

static void ww_value_array_destruct(ww_value_array_t* array)
{
    if (NULL != array->array_items)
        free(array->array_items);
}

WW_CLASS_INSTANCE(
    ww_value_array_t,
    ww_object_t,
    ww_value_array_construct,
    ww_value_array_destruct
);


int ww_value_array_set_size(ww_value_array_t* array, size_t size)
{
#if WW_ENABLE_DEBUG
    if(array->array_item_sizeof == 0) {
        ww_output(0, "ww_value_array_set_size: item size must be initialized");
        return WW_ERR_BAD_PARAM;
    }
#endif

    if(size > array->array_alloc_size) {
        while(array->array_alloc_size < size)
            array->array_alloc_size <<= 1;
        array->array_items = (unsigned char *)realloc(array->array_items,
            array->array_alloc_size * array->array_item_sizeof);
        if (NULL == array->array_items)
            return WW_ERR_OUT_OF_RESOURCE;
    }
    array->array_size = size;
    return WW_SUCCESS;
}

