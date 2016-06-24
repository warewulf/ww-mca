/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2007 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Voltaire All rights reserved.
 * Copyright (c) 2013-2015 Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>
#include "ww_types.h"
#include "src/class/ww_list.h"
#include <ww.h>

/*
 *  List classes
 */

static void ww_list_item_construct(ww_list_item_t*);
static void ww_list_item_destruct(ww_list_item_t*);

WW_CLASS_INSTANCE(
    ww_list_item_t,
    ww_object_t,
    ww_list_item_construct,
    ww_list_item_destruct
);

static void ww_list_construct(ww_list_t*);
static void ww_list_destruct(ww_list_t*);

WW_CLASS_INSTANCE(
    ww_list_t,
    ww_object_t,
    ww_list_construct,
    ww_list_destruct
);


/*
 *
 *      ww_list_link_item_t interface
 *
 */

static void ww_list_item_construct(ww_list_item_t *item)
{
    item->ww_list_next = item->ww_list_prev = NULL;
    item->item_free = 1;
#if WW_ENABLE_DEBUG
    item->ww_list_item_refcount = 0;
    item->ww_list_item_belong_to = NULL;
#endif
}

static void ww_list_item_destruct(ww_list_item_t *item)
{
#if WW_ENABLE_DEBUG
    assert( 0 == item->ww_list_item_refcount );
    assert( NULL == item->ww_list_item_belong_to );
#endif  /* WW_ENABLE_DEBUG */
}


/*
 *
 *      ww_list_list_t interface
 *
 */

static void ww_list_construct(ww_list_t *list)
{
#if WW_ENABLE_DEBUG
    /* These refcounts should never be used in assertions because they
       should never be removed from this list, added to another list,
       etc.  So set them to sentinel values. */

    WW_CONSTRUCT( &(list->ww_list_sentinel), ww_list_item_t );
    list->ww_list_sentinel.ww_list_item_refcount  = 1;
    list->ww_list_sentinel.ww_list_item_belong_to = list;
#endif

    list->ww_list_sentinel.ww_list_next = &list->ww_list_sentinel;
    list->ww_list_sentinel.ww_list_prev = &list->ww_list_sentinel;
    list->ww_list_length = 0;
}


/*
 * Reset all the pointers to be NULL -- do not actually destroy
 * anything.
 */
static void ww_list_destruct(ww_list_t *list)
{
    ww_list_construct(list);
}


/*
 * Insert an item at a specific place in a list
 */
bool ww_list_insert(ww_list_t *list, ww_list_item_t *item, long long idx)
{
    /* Adds item to list at index and retains item. */
    int     i;
    volatile ww_list_item_t *ptr, *next;

    if ( idx >= (long long)list->ww_list_length ) {
        return false;
    }

    if ( 0 == idx )
    {
        ww_list_prepend(list, item);
    } else {
#if WW_ENABLE_DEBUG
        /* Spot check: ensure that this item is previously on no
           lists */

        assert(0 == item->ww_list_item_refcount);
#endif
        /* pointer to element 0 */
        ptr = list->ww_list_sentinel.ww_list_next;
        for ( i = 0; i < idx-1; i++ )
            ptr = ptr->ww_list_next;

        next = ptr->ww_list_next;
        item->ww_list_next = next;
        item->ww_list_prev = ptr;
        next->ww_list_prev = item;
        ptr->ww_list_next = item;

#if WW_ENABLE_DEBUG
        /* Spot check: ensure this item is only on the list that we
           just insertted it into */

        item->ww_list_item_refcount += 1;
        assert(1 == item->ww_list_item_refcount);
        item->ww_list_item_belong_to = list;
#endif
    }

    list->ww_list_length++;
    return true;
}


static
void
ww_list_transfer(ww_list_item_t *pos, ww_list_item_t *begin,
                   ww_list_item_t *end)
{
    volatile ww_list_item_t *tmp;

    if (pos != end) {
        /* remove [begin, end) */
        end->ww_list_prev->ww_list_next = pos;
        begin->ww_list_prev->ww_list_next = end;
        pos->ww_list_prev->ww_list_next = begin;

        /* splice into new position before pos */
        tmp = pos->ww_list_prev;
        pos->ww_list_prev = end->ww_list_prev;
        end->ww_list_prev = begin->ww_list_prev;
        begin->ww_list_prev = tmp;
#if WW_ENABLE_DEBUG
        {
            volatile ww_list_item_t* item = begin;
            while( pos != item ) {
                item->ww_list_item_belong_to = pos->ww_list_item_belong_to;
                item = item->ww_list_next;
                assert(NULL != item);
            }
        }
#endif  /* WW_ENABLE_DEBUG */
    }
}


void
ww_list_join(ww_list_t *thislist, ww_list_item_t *pos,
               ww_list_t *xlist)
{
    if (0 != ww_list_get_size(xlist)) {
        ww_list_transfer(pos, ww_list_get_first(xlist),
                           ww_list_get_end(xlist));

        /* fix the sizes */
        thislist->ww_list_length += xlist->ww_list_length;
        xlist->ww_list_length = 0;
    }
}


void
ww_list_splice(ww_list_t *thislist, ww_list_item_t *pos,
                 ww_list_t *xlist, ww_list_item_t *first,
                 ww_list_item_t *last)
{
    size_t change = 0;
    ww_list_item_t *tmp;

    if (first != last) {
        /* figure out how many things we are going to move (have to do
         * first, since last might be end and then we wouldn't be able
         * to run the loop)
         */
        for (tmp = first ; tmp != last ; tmp = ww_list_get_next(tmp)) {
            change++;
        }

        ww_list_transfer(pos, first, last);

        /* fix the sizes */
        thislist->ww_list_length += change;
        xlist->ww_list_length -= change;
    }
}


int ww_list_sort(ww_list_t* list, ww_list_item_compare_fn_t compare)
{
    ww_list_item_t* item;
    ww_list_item_t** items;
    size_t i, index=0;

    if (0 == list->ww_list_length) {
        return WW_SUCCESS;
    }
    items = (ww_list_item_t**)malloc(sizeof(ww_list_item_t*) *
                                       list->ww_list_length);

    if (NULL == items) {
        return WW_ERR_OUT_OF_RESOURCE;
    }

    while(NULL != (item = ww_list_remove_first(list))) {
        items[index++] = item;
    }

    qsort(items, index, sizeof(ww_list_item_t*),
          (int(*)(const void*,const void*))compare);
    for (i=0; i<index; i++) {
        ww_list_append(list,items[i]);
    }
    free(items);
    return WW_SUCCESS;
}
