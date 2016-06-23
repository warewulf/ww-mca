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
 * Copyright (c) 2015-2016 Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#ifndef WW_UTIL_ERROR_H
#define WW_UTIL_ERROR_H

#include <src/include/ww_config.h>


#include <ww_types.h>
#include "src/util/output.h"

 BEGIN_C_DECLS

#define WW_ERROR_LOG(r)                                               \
 do {                                                               \
    if (WW_ERR_SILENT != (r)) {                                   \
        ww_output(0, "WW ERROR: %s in file %s at line %d",      \
                    WW_Error_string((r)), __FILE__, __LINE__);    \
    }                                                               \
} while (0)

#define WW_ERR_INVALID_CRED                           (WW_INTERNAL_ERR_BASE -  1)
#define WW_ERR_HANDSHAKE_FAILED                       (WW_INTERNAL_ERR_BASE -  2)
#define WW_ERR_READY_FOR_HANDSHAKE                    (WW_INTERNAL_ERR_BASE -  3)
#define WW_ERR_UNKNOWN_DATA_TYPE                      (WW_INTERNAL_ERR_BASE -  4)
#define WW_ERR_TYPE_MISMATCH                          (WW_INTERNAL_ERR_BASE -  5)
#define WW_ERR_UNPACK_INADEQUATE_SPACE                (WW_INTERNAL_ERR_BASE -  6)
#define WW_ERR_UNPACK_FAILURE                         (WW_INTERNAL_ERR_BASE -  7)
#define WW_ERR_PACK_FAILURE                           (WW_INTERNAL_ERR_BASE -  8)
#define WW_ERR_PACK_MISMATCH                          (WW_INTERNAL_ERR_BASE -  9)
#define WW_ERR_PROC_ENTRY_NOT_FOUND                   (WW_INTERNAL_ERR_BASE - 10)
#define WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER         (WW_INTERNAL_ERR_BASE - 11)
#define WW_ERR_SERVER_NOT_AVAIL                       (WW_INTERNAL_ERR_BASE - 12)
#define WW_ERR_INVALID_KEYVALP                        (WW_INTERNAL_ERR_BASE - 13)
#define WW_ERR_INVALID_NUM_PARSED                     (WW_INTERNAL_ERR_BASE - 14)
#define WW_ERR_INVALID_ARGS                           (WW_INTERNAL_ERR_BASE - 15)
#define WW_ERR_INVALID_NUM_ARGS                       (WW_INTERNAL_ERR_BASE - 16)
#define WW_ERR_INVALID_LENGTH                         (WW_INTERNAL_ERR_BASE - 17)
#define WW_ERR_INVALID_VAL_LENGTH                     (WW_INTERNAL_ERR_BASE - 18)
#define WW_ERR_INVALID_VAL                            (WW_INTERNAL_ERR_BASE - 19)
#define WW_ERR_INVALID_KEY_LENGTH                     (WW_INTERNAL_ERR_BASE - 20)
#define WW_ERR_INVALID_KEY                            (WW_INTERNAL_ERR_BASE - 21)
#define WW_ERR_INVALID_ARG                            (WW_INTERNAL_ERR_BASE - 22)
#define WW_ERR_NOMEM                                  (WW_INTERNAL_ERR_BASE - 23)
#define WW_ERR_IN_ERRNO                               (WW_INTERNAL_ERR_BASE - 24)
#define WW_ERR_SILENT                                 (WW_INTERNAL_ERR_BASE - 25)
#define WW_ERR_UNKNOWN_DATATYPE                       (WW_INTERNAL_ERR_BASE - 26)
#define WW_ERR_RESOURCE_BUSY                          (WW_INTERNAL_ERR_BASE - 27)

WW_EXPORT const char* WW_Error_string(ww_status_t errnum);

END_C_DECLS

#endif /* WW_UTIL_ERROR_H */
