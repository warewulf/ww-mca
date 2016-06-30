/* -*- Mode: C; c-basic-offset:4 ; -*- */
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
 * Copyright (c) 2007      Voltaire All rights reserved.
 * Copyright (c) 2013      Los Alamos National Security, LLC. All rights
 *                         reserved.
 * Copyright (c) 2013-2016 Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
/**
 * @file
 *
 * The ww_list_t interface is used to provide a generic
 * doubly-linked list container for WW.  It was inspired by (but
 * is slightly different than) the Standard Template Library (STL)
 * std::list class.  One notable difference from std::list is that
 * when an ww_list_t is destroyed, all of the ww_list_item_t
 * objects that it contains are orphaned -- they are \em not
 * destroyed.
 *
 * The general idea is that ww_list_item_t objects can be put on an
 * ww_list_t.  Hence, you create a new type that derives from
 * ww_list_item_t; this new type can then be used with ww_list_t
 * containers.
 *
 * NOTE: ww_list_item_t instances can only be on \em one list at a
 * time.  Specifically, if you add an ww_list_item_t to one list,
 * and then add it to another list (without first removing it from the
 * first list), you will effectively be hosing the first list.  You
 * have been warned.
 *
 * If WW_ENABLE_DEBUG is true, a bunch of checks occur, including
 * some spot checks for a debugging reference count in an attempt to
 * ensure that an ww_list_item_t is only one *one* list at a time.
 * Given the highly concurrent nature of this class, these spot checks
 * cannot guarantee that an item is only one list at a time.
 * Specifically, since it is a desirable attribute of this class to
 * not use locks for normal operations, it is possible that two
 * threads may [erroneously] modify an ww_list_item_t concurrently.
 *
 * The only way to guarantee that a debugging reference count is valid
 * for the duration of an operation is to lock the item_t during the
 * operation.  But this fundamentally changes the desirable attribute
 * of this class (i.e., no locks).  So all we can do is spot-check the
 * reference count in a bunch of places and check that it is still the
 * value that we think it should be.  But this doesn't mean that you
 * can run into "unlucky" cases where two threads are concurrently
 * modifying an item_t, but all the spot checks still return the
 * "right" values.  All we can do is hope that we have enough spot
 * checks to statistically drive down the possibility of the unlucky
 * cases happening.
 */

#ifndef WW_LIST_H
#define WW_LIST_H

#include <src/include/ww_config.h>
#include <stdio.h>
#include <stdlib.h>
#if HAVE_STDBOOL_H
#include <stdbool.h>
#endif

#include "src/class/ww_object.h"
#include "src/threads/threads.h"

BEGIN_C_DECLS

/**
 * \internal
 *
 * The class for the list container.
 */
WW_DECLSPEC WW_CLASS_DECLARATION(ww_list_t);
/**
 * \internal
 *
 * Base class for items that are put in list (ww_list_t) containers.
 */
WW_DECLSPEC WW_CLASS_DECLARATION(ww_list_item_t);


/**
 * \internal
 *
 * Struct of an ww_list_item_t
 */
struct ww_list_item_t
{
    ww_object_t super;
    /**< Generic parent class for all WW objects */
    volatile struct ww_list_item_t *ww_list_next;
    /**< Pointer to next list item */
    volatile struct ww_list_item_t *ww_list_prev;
    /**< Pointer to previous list item */
    int32_t item_free;

#if WW_ENABLE_DEBUG
    /** Atomic reference count for debugging */
    volatile int32_t ww_list_item_refcount;
    /** The list this item belong to */
    volatile struct ww_list_t* ww_list_item_belong_to;
#endif
};
/**
 * Base type for items that are put in a list (ww_list_t) containers.
 */
typedef struct ww_list_item_t ww_list_item_t;


/**
 * Get the next item in a list.
 *
 * @param item A list item.
 *
 * @returns The next item in the list
 */
#define ww_list_get_next(item) \
    ((item) ? ((ww_list_item_t*) ((ww_list_item_t*)(item))->ww_list_next) : NULL)

/**
 * Get the next item in a list.
 *
 * @param item A list item.
 *
 * @returns The next item in the list
 */
#define ww_list_get_prev(item) \
    ((item) ? ((ww_list_item_t*) ((ww_list_item_t*)(item))->ww_list_prev) : NULL)


/**
 * \internal
 *
 * Struct of an ww_list_t
 */
struct ww_list_t
{
    ww_object_t       super; // Generic parent class for all WW objects
    ww_list_item_t    ww_list_sentinel; // Head and tail item of the list
    volatile size_t     ww_list_length; // Quick reference to the number of items in the list
    ww_threadlock_t lock;      // thread support
};
/**
 * List container type.
 */
typedef struct ww_list_t ww_list_t;

/** Cleanly destruct a list
 *
 * The ww_list_t destructor doesn't release the items on the
 * list - so provide two convenience macros that do so and then
 * destruct/release the list object itself
 *
 * @param[in] list List to destruct or release
 */
#define WW_LIST_DESTRUCT(list)                                              \
    do {                                                                    \
        ww_list_item_t *it;                                                 \
        while (NULL != (it = ww_list_remove_first(list))) {                 \
            WW_RELEASE(it);                                                 \
        }                                                                   \
        WW_DESTRUCT(list);                                                  \
    } while (0)

#define WW_LIST_RELEASE(list)                                               \
    do {                                                                    \
        ww_list_item_t *it;                                                 \
        while (NULL != (it = ww_list_remove_first(list))) {                 \
            WW_RELEASE(it);                                                 \
        }                                                                   \
        WW_RELEASE(list);                                                   \
    } while (0)


/**
 * Loop over a list.
 *
 * @param[in] item Storage for each item
 * @param[in] list List to iterate over
 * @param[in] type Type of each list item
 *
 * This macro provides a simple way to loop over the items in an ww_list_t. It
 * is not safe to call ww_list_remove_item from within the loop.
 *
 * Example Usage:
 *
 * class_foo_t *foo;
 * WW_ACQUIRE_THREAD(&list->lock);
 * ww_list_foreach(foo, list, class_foo_t) {
 *    do something;
 * }
 * WW_RELEASE_THREAD(&list->lock);
 */
#define WW_LIST_FOREACH(item, list, type)                               \
  for (item = (type *) (list)->ww_list_sentinel.ww_list_next ;          \
       item != (type *) &(list)->ww_list_sentinel ;                     \
       item = (type *) ((ww_list_item_t *) (item))->ww_list_next)

/**
 * Loop over a list in reverse.
 *
 * @param[in] item Storage for each item
 * @param[in] list List to iterate over
 * @param[in] type Type of each list item
 *
 * This macro provides a simple way to loop over the items in an ww_list_t. It
 * is not safe to call ww_list_remove_item from within the loop.
 *
 * Example Usage:
 *
 * class_foo_t *foo;
 * WW_ACQUIRE_THREAD(&list->lock);
 * ww_list_foreach_rev(foo, list, class_foo_t) {
 *    do something;
 * }
 * WW_RELEASE_THREAD(&list->lock);
 */
#define WW_LIST_FOREACH_REV(item, list, type)                         \
  for (item = (type *) (list)->ww_list_sentinel.ww_list_prev ;      \
       item != (type *) &(list)->ww_list_sentinel ;                   \
       item = (type *) ((ww_list_item_t *) (item))->ww_list_prev)

/**
 * Loop over a list in a *safe* way
 *
 * @param[in] item Storage for each item
 * @param[in] next Storage for next item
 * @param[in] list List to iterate over
 * @param[in] type Type of each list item
 *
 * This macro provides a simple way to loop over the items in an ww_list_t. It
 * is safe to call ww_list_remove_item(list, item) from within the loop.
 *
 * Example Usage:
 *
 * class_foo_t *foo, *next;
 * WW_ACQUIRE_THREAD(&list->lock);
 * ww_list_foreach_safe(foo, next, list, class_foo_t) {
 *    do something;
 *    ww_list_remove_item (list, (ww_list_item_t *) foo);
 * }
 * WW_RELEASE_THREAD(&list->lock);
 */
#define WW_LIST_FOREACH_SAFE(item, next, list, type)                  \
  for (item = (type *) (list)->ww_list_sentinel.ww_list_next,       \
         next = (type *) ((ww_list_item_t *) (item))->ww_list_next ;\
       item != (type *) &(list)->ww_list_sentinel ;                   \
       item = next, next = (type *) ((ww_list_item_t *) (item))->ww_list_next)

/**
 * Loop over a list in a *safe* way
 *
 * @param[in] item Storage for each item
 * @param[in] next Storage for next item
 * @param[in] list List to iterate over
 * @param[in] type Type of each list item
 *
 * This macro provides a simple way to loop over the items in an ww_list_t. If
 * is safe to call ww_list_remove_item(list, item) from within the loop.
 *
 * Example Usage:
 *
 * class_foo_t *foo, *next;
 * WW_ACQUIRE_THREAD(&list->lock);
 * ww_list_foreach_safe(foo, next, list, class_foo_t) {
 *    do something;
 *    ww_list_remove_item (list, (ww_list_item_t *) foo);
 * }
 * WW_RELEASE_THREAD(&list->lock);
 */
#define WW_LIST_FOREACH_SAFE_REV(item, prev, list, type)              \
  for (item = (type *) (list)->ww_list_sentinel.ww_list_prev,       \
         prev = (type *) ((ww_list_item_t *) (item))->ww_list_prev ;\
       item != (type *) &(list)->ww_list_sentinel ;                   \
       item = prev, prev = (type *) ((ww_list_item_t *) (item))->ww_list_prev)


/**
 * Check for empty list
 *
 * @param list The list container
 *
 * @returns true if list's size is 0, false otherwise
 *
 * This is an O(1) operation.
 *
 * This is an inlined function in compilers that support inlining,
 * so it's usually a cheap operation.
 */
static inline bool ww_list_is_empty(ww_list_t* list)
{
    return (list->ww_list_sentinel.ww_list_next ==
        &(list->ww_list_sentinel) ? true : false);
}


/**
 * Return the first item on the list (does not remove it).
 *
 * @param list The list container
 *
 * @returns A pointer to the first item on the list
 *
 * This is an O(1) operation to return the first item on the list.  It
 * should be compared against the returned value from
 * ww_list_get_end() to ensure that the list is not empty.
 *
 * This is an inlined function in compilers that support inlining, so
 * it's usually a cheap operation.
 */
static inline ww_list_item_t* ww_list_get_first(ww_list_t* list)
{
    ww_list_item_t* item = (ww_list_item_t*)list->ww_list_sentinel.ww_list_next;
#if WW_ENABLE_DEBUG
    /* Spot check: ensure that the first item is only on one list */

    assert(1 == item->ww_list_item_refcount);
    assert( list == item->ww_list_item_belong_to );
#endif

    return item;
}

/**
 * Return the last item on the list (does not remove it).
 *
 * @param list The list container
 *
 * @returns A pointer to the last item on the list
 *
 * This is an O(1) operation to return the last item on the list.  It
 * should be compared against the returned value from
 * ww_list_get_begin() to ensure that the list is not empty.
 *
 * This is an inlined function in compilers that support inlining, so
 * it's usually a cheap operation.
 */
static inline ww_list_item_t* ww_list_get_last(ww_list_t* list)
{
    ww_list_item_t* item = (ww_list_item_t *)list->ww_list_sentinel.ww_list_prev;
#if WW_ENABLE_DEBUG
    /* Spot check: ensure that the last item is only on one list */

    assert( 1 == item->ww_list_item_refcount );
    assert( list == item->ww_list_item_belong_to );
#endif

    return item;
}

/**
 * Return the beginning of the list; an invalid list entry suitable
 * for comparison only.
 *
 * @param list The list container
 *
 * @returns A pointer to the beginning of the list.
 *
 * This is an O(1) operation to return the beginning of the list.
 * Similar to the STL, this is a special invalid list item -- it
 * should \em not be used for storage.  It is only suitable for
 * comparison to other items in the list to see if they are valid or
 * not; it's ususally used when iterating through the items in a list.
 *
 * This is an inlined function in compilers that support inlining, so
 * it's usually a cheap operation.
 */
static inline ww_list_item_t* ww_list_get_begin(ww_list_t* list)
{
    return &(list->ww_list_sentinel);
}

/**
 * Return the end of the list; an invalid list entry suitable for
 * comparison only.
 *
 * @param list The list container
 *
 * @returns A pointer to the end of the list.
 *
 * This is an O(1) operation to return the end of the list.
 * Similar to the STL, this is a special invalid list item -- it
 * should \em not be used for storage.  It is only suitable for
 * comparison to other items in the list to see if they are valid or
 * not; it's ususally used when iterating through the items in a list.
 *
 * This is an inlined function in compilers that support inlining, so
 * it's usually a cheap operation.
 */
static inline ww_list_item_t* ww_list_get_end(ww_list_t* list)
{
    return &(list->ww_list_sentinel);
}


/**
 * Return the number of items in a list
 *
 * @param list The list container
 *
 * @returns The size of the list (size_t)
 *
 * This is an O(1) lookup to return the size of the list.
 *
 * This is an inlined function in compilers that support inlining, so
 * it's usually a cheap operation.
 *
 * \warning The size of the list is cached as part of the list.  In
 * the future, calling \c ww_list_splice or \c ww_list_join may
 * result in this function recomputing the list size, which would be
 * an O(N) operation.  If \c ww_list_splice or \c ww_list_join is
 * never called on the specified list, this function will always be
 * O(1).
 */
static inline size_t ww_list_get_size(ww_list_t* list)
{
#if WW_ENABLE_DEBUG && 0
    /* not sure if we really want this running in devel, as it does
     * slow things down.  Wanted for development of splice / join to
     * make sure length was reset properly
     */
    size_t check_len = 0;
    ww_list_item_t *item;

    for (item = ww_list_get_first(list) ;
         item != ww_list_get_end(list) ;
         item = ww_list_get_next(item)) {
        check_len++;
    }

    if (check_len != list->ww_list_length) {
        fprintf(stderr," Error :: ww_list_get_size - ww_list_length does not match actual list length\n");
        fflush(stderr);
        abort();
    }
#endif

    return list->ww_list_length;
}


/**
 * Remove an item from a list.
 *
 * @param list The list container
 * @param item The item to remove
 *
 * @returns A pointer to the item on the list previous to the one
 * that was removed.
 *
 * This is an O(1) operation to remove an item from the list.  The
 * forward / reverse pointers in the list are updated and the item is
 * removed.  The list item that is returned is now "owned" by the
 * caller -- they are responsible for WW_RELEASE()'ing it.
 *
 * If debugging is enabled (specifically, if --enable-debug was used
 * to configure Warewulf), this is an O(N) operation because it checks
 * to see if the item is actually in the list first.
 *
 * This is an inlined function in compilers that support inlining, so
 * it's usually a cheap operation.
 */
static inline ww_list_item_t *ww_list_remove_item
  (ww_list_t *list, ww_list_item_t *item)
{
#if WW_ENABLE_DEBUG
    ww_list_item_t *item_ptr;
    bool found = false;

    /* check to see that the item is in the list */
    for (item_ptr = ww_list_get_first(list);
            item_ptr != ww_list_get_end(list);
            item_ptr = (ww_list_item_t *)(item_ptr->ww_list_next)) {
        if (item_ptr == (ww_list_item_t *) item) {
            found = true;
            break;
        }
    }
    if (!found) {
        fprintf(stderr," Warning :: ww_list_remove_item - the item %p is not on the list %p \n",(void*) item, (void*) list);
        fflush(stderr);
        return (ww_list_item_t *)NULL;
    }

    assert( list == item->ww_list_item_belong_to );
#endif

    /* reset next pointer of previous element */
    item->ww_list_prev->ww_list_next=item->ww_list_next;

    /* reset previous pointer of next element */
    item->ww_list_next->ww_list_prev=item->ww_list_prev;

    list->ww_list_length--;

#if WW_ENABLE_DEBUG
    /* Spot check: ensure that this item is still only on one list */

    item->ww_list_item_refcount -= 1;
    assert(0 == item->ww_list_item_refcount);
    item->ww_list_item_belong_to = NULL;
#endif

    return (ww_list_item_t *)item->ww_list_prev;
}


/**
 * Append an item to the end of the list.
 *
 * @param list The list container
 * @param item The item to append
 *
 * This is an O(1) operation to append an item to the end of a list.
 * The ww_list_item_t is not WW_RETAIN()'ed; it is assumed that
 * "ownership" of the item is passed from the caller to the list.
 *
 * This is an inlined function in compilers that support inlining, so
 * it's usually a cheap operation.
 */

#if WW_ENABLE_DEBUG
#define ww_list_append(l,i) \
_ww_list_append(l,i,__FILE__,__LINE__)
#else
#define ww_list_append(l,i) \
_ww_list_append(l,i)
#endif  /* WW_ENABLE_DEBUG */

static inline void _ww_list_append(ww_list_t *list, ww_list_item_t *item
#if WW_ENABLE_DEBUG
                                     , const char* FILE_NAME, int LINENO
#endif  /* WW_ENABLE_DEBUG */
                                     )
{
    ww_list_item_t* sentinel = &(list->ww_list_sentinel);
#if WW_ENABLE_DEBUG
  /* Spot check: ensure that this item is previously on no lists */

  assert(0 == item->ww_list_item_refcount);
  assert( NULL == item->ww_list_item_belong_to );
  item->super.cls_init_file_name = FILE_NAME;
  item->super.cls_init_lineno    = LINENO;
#endif

  /* set new element's previous pointer */
  item->ww_list_prev = sentinel->ww_list_prev;

  /* reset previous pointer on current last element */
  sentinel->ww_list_prev->ww_list_next = item;

  /* reset new element's next pointer */
  item->ww_list_next = sentinel;

  /* reset the list's tail element previous pointer */
  sentinel->ww_list_prev = item;

  /* increment list element counter */
  list->ww_list_length++;

#if WW_ENABLE_DEBUG
  /* Spot check: ensure this item is only on the list that we just
     appended it to */

  item->ww_list_item_refcount += 1;
  assert(1 == item->ww_list_item_refcount);
  item->ww_list_item_belong_to = list;
#endif
}


/**
 * Prepend an item to the beginning of the list.
 *
 * @param list The list container
 * @param item The item to prepend
 *
 * This is an O(1) operation to prepend an item to the beginning of a
 * list.  The ww_list_item_t is not WW_RETAIN()'ed; it is assumed
 * that "ownership" of the item is passed from the caller to the list.
 *
 * This is an inlined function in compilers that support inlining, so
 * it's usually a cheap operation.
 */
static inline void ww_list_prepend(ww_list_t *list,
                                     ww_list_item_t *item)
{
    ww_list_item_t* sentinel = &(list->ww_list_sentinel);
#if WW_ENABLE_DEBUG
  /* Spot check: ensure that this item is previously on no lists */

  assert(0 == item->ww_list_item_refcount);
  assert( NULL == item->ww_list_item_belong_to );
#endif

  /* reset item's next pointer */
  item->ww_list_next = sentinel->ww_list_next;

  /* reset item's previous pointer */
  item->ww_list_prev = sentinel;

  /* reset previous first element's previous poiner */
  sentinel->ww_list_next->ww_list_prev = item;

  /* reset head's next pointer */
  sentinel->ww_list_next = item;

  /* increment list element counter */
  list->ww_list_length++;

#if WW_ENABLE_DEBUG
  /* Spot check: ensure this item is only on the list that we just
     prepended it to */

  item->ww_list_item_refcount += 1;
  assert(1 == item->ww_list_item_refcount);
  item->ww_list_item_belong_to = list;
#endif
}


/**
 * Remove the first item from the list and return it.
 *
 * @param list The list container
 *
 * @returns The first item on the list.  If the list is empty,
 * NULL will be returned
 *
 * This is an O(1) operation to return the first item on the list.  If
 * the list is not empty, a pointer to the first item in the list will
 * be returned.  Ownership of the item is transferred from the list to
 * the caller; no calls to WW_RETAIN() or WW_RELEASE() are invoked.
 *
 * This is an inlined function in compilers that support inlining, so
 * it's usually a cheap operation.
 */
static inline ww_list_item_t *ww_list_remove_first(ww_list_t *list)
{
  /*  Removes and returns first item on list.
      Caller now owns the item and should release the item
      when caller is done with it.
  */
  volatile ww_list_item_t *item;
  if ( 0 == list->ww_list_length ) {
    return (ww_list_item_t *)NULL;
  }

#if WW_ENABLE_DEBUG
  /* Spot check: ensure that the first item is only on this list */

  assert(1 == list->ww_list_sentinel.ww_list_next->ww_list_item_refcount);
#endif

  /* reset list length counter */
  list->ww_list_length--;

  /* get pointer to first element on the list */
  item = list->ww_list_sentinel.ww_list_next;

  /* reset previous pointer of next item on the list */
  item->ww_list_next->ww_list_prev = item->ww_list_prev;

  /* reset the head next pointer */
  list->ww_list_sentinel.ww_list_next = item->ww_list_next;

#if WW_ENABLE_DEBUG
  assert( list == item->ww_list_item_belong_to );
  item->ww_list_item_belong_to = NULL;
  item->ww_list_prev=(ww_list_item_t *)NULL;
  item->ww_list_next=(ww_list_item_t *)NULL;

  /* Spot check: ensure that the item we're returning is now on no
     lists */

  item->ww_list_item_refcount -= 1;
  assert(0 == item->ww_list_item_refcount);
#endif

  return (ww_list_item_t *) item;
}


/**
 * Remove the last item from the list and return it.
 *
 * @param list The list container
 *
 * @returns The last item on the list.  If the list is empty,
 * NULL will be returned
 *
 * This is an O(1) operation to return the last item on the list.  If
 * the list is not empty, a pointer to the last item in the list will
 * be returned.  Ownership of the item is transferred from the list to
 * the caller; no calls to WW_RETAIN() or WW_RELEASE() are invoked.
 *
 * This is an inlined function in compilers that support inlining, so
 * it's usually a cheap operation.
 */
static inline ww_list_item_t *ww_list_remove_last(ww_list_t *list)
{
  /*  Removes, releases and returns last item on list.
      Caller now owns the item and should release the item
      when caller is done with it.
  */
  volatile ww_list_item_t  *item;
  if ( 0 == list->ww_list_length ) {
      return (ww_list_item_t *)NULL;
  }

#if WW_ENABLE_DEBUG
  /* Spot check: ensure that the first item is only on this list */

  assert(1 == list->ww_list_sentinel.ww_list_prev->ww_list_item_refcount);
#endif

  /* reset list length counter */
  list->ww_list_length--;

  /* get item */
  item = list->ww_list_sentinel.ww_list_prev;

  /* reset previous pointer on next to last pointer */
  item->ww_list_prev->ww_list_next = item->ww_list_next;

  /* reset tail's previous pointer */
  list->ww_list_sentinel.ww_list_prev = item->ww_list_prev;

#if WW_ENABLE_DEBUG
  assert( list == item->ww_list_item_belong_to );
  item->ww_list_next = item->ww_list_prev = (ww_list_item_t *)NULL;

  /* Spot check: ensure that the item we're returning is now on no
     lists */

  item->ww_list_item_refcount -= 1;
  assert(0 == item->ww_list_item_refcount);
  item->ww_list_item_belong_to = NULL;
#endif

  return (ww_list_item_t *) item;
}

  /**
   * Add an item to the list before a given element
   *
   * @param list The list container
   * @param pos List element to insert \c item before
   * @param item The item to insert
   *
   * Inserts \c item before \c pos.  This is an O(1) operation.
   */
static inline void ww_list_insert_pos(ww_list_t *list, ww_list_item_t *pos,
                                        ww_list_item_t *item)
{
#if WW_ENABLE_DEBUG
    /* Spot check: ensure that the item we're insertting is currently
       not on any list */

    assert(0 == item->ww_list_item_refcount);
    assert( NULL == item->ww_list_item_belong_to );
#endif

    /* point item at the existing elements */
    item->ww_list_next = pos;
    item->ww_list_prev = pos->ww_list_prev;

    /* splice into the list */
    pos->ww_list_prev->ww_list_next = item;
    pos->ww_list_prev = item;

    /* reset list length counter */
    list->ww_list_length++;

#if WW_ENABLE_DEBUG
    /* Spot check: double check that this item is only on the list
       that we just added it to */

    item->ww_list_item_refcount += 1;
    assert(1 == item->ww_list_item_refcount);
    item->ww_list_item_belong_to = list;
#endif
}

  /**
   * Add an item to the list at a specific index location in the list.
   *
   * @param list The list container
   * @param item The item to insert
   * @param index Location to add the item
   *
   * @returns true if insertion succeeded; otherwise false
   *
   * This is potentially an O(N) operation to traverse down to the
   * correct location in the list and add an item.
   *
   * Example: if idx = 2 and list = item1->item2->item3->item4, then
   * after insert, list = item1->item2->item->item3->item4.
   *
   * If index is greater than the length of the list, no action is
   * performed and false is returned.
   */
  WW_DECLSPEC bool ww_list_insert(ww_list_t *list, ww_list_item_t *item,
                                      long long idx);


    /**
     * Join a list into another list
     *
     * @param thislist List container for list being operated on
     * @param pos List item in \c thislist marking the position before
     *              which items are inserted
     * @param xlist List container for list being spliced from
     *
     * Join a list into another list.  All of the elements of \c xlist
     * are inserted before \c pos and removed from \c xlist.
     *
     * This operation is an O(1) operation.  Both \c thislist and \c
     * xlist must be valid list containsers.  \c xlist will be empty
     * but valid after the call.  All pointers to \c ww_list_item_t
     * containers remain valid, including those that point to elements
     * in \c xlist.
     */
    WW_DECLSPEC void ww_list_join(ww_list_t *thislist, ww_list_item_t *pos,
                                      ww_list_t *xlist);


    /**
     * Splice a list into another list
     *
     * @param thislist List container for list being operated on
     * @param pos List item in \c thislist marking the position before
     *             which items are inserted
     * @param xlist List container for list being spliced from
     * @param first List item in \c xlist marking the start of elements
     *             to be copied into \c thislist
     * @param last List item in \c xlist marking the end of elements
     * to be copied into \c thislist
     *
     * Splice a subset of a list into another list.  The \c [first,
     * last) elements of \c xlist are moved into \c thislist,
     * inserting them before \c pos.  \c pos must be a valid iterator
     * in \c thislist and \c [first, last) must be a valid range in \c
     * xlist.  \c postition must not be in the range \c [first, last).
     * It is, however, valid for \c xlist and \c thislist to be the
     * same list.
     *
     * This is an O(N) operation because the length of both lists must
     * be recomputed.
     */
    WW_DECLSPEC void ww_list_splice(ww_list_t *thislist, ww_list_item_t *pos,
                                        ww_list_t *xlist, ww_list_item_t *first,
                                        ww_list_item_t *last);

    /**
     * Comparison function for ww_list_sort(), below.
     *
     * @param a Pointer to a pointer to an ww_list_item_t.
     * Explanation below.
     * @param b Pointer to a pointer to an ww_list_item_t.
     * Explanation below.
     * @retval 1 if \em a is greater than \em b
     * @retval 0 if \em a is equal to \em b
     * @retval 11 if \em a is less than \em b
     *
     * This function is invoked by qsort(3) from within
     * ww_list_sort().  It is important to understand what
     * ww_list_sort() does before invoking qsort, so go read that
     * documentation first.
     *
     * The important thing to realize here is that a and b will be \em
     * double pointers to the items that you need to compare.  Here's
     * a sample compare function to illustrate this point:
     */
    typedef int (*ww_list_item_compare_fn_t)(ww_list_item_t **a,
                                               ww_list_item_t **b);

    /**
     * Sort a list with a provided compare function.
     *
     * @param list The list to sort
     * @param compare Compare function
     *
     * Put crassly, this function's complexity is O(N) + O(log(N)).
     * Its algorithm is:
     *
     * - remove every item from the list and put the corresponding
     *    (ww_list_item_t*)'s in an array
     * - call qsort(3) with that array and your compare function
     * - re-add every element of the now-sorted array to the list
     *
     * The resulting list is now ordered.  Note, however, that since
     * an array of pointers is sorted, the comparison function must do
     * a double de-reference to get to the actual ww_list_item_t (or
     * whatever the underlying type is).  See the documentation of
     * ww_list_item_compare_fn_t for an example).
     */
    WW_DECLSPEC int ww_list_sort(ww_list_t* list, ww_list_item_compare_fn_t compare);

END_C_DECLS

#endif /* WW_LIST_H */
