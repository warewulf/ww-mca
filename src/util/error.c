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
 * Copyright (c) 2007-2012 Los Alamos National Security, LLC.
 *                         All rights reserved.
 * Copyright (c) 2014-2016 Intel, Inc. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <errno.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <ww_types.h>

#include "src/util/error.h"

const char* WW_Error_string(ww_status_t errnum)
{
    switch(errnum) {
    case WW_ERR_UNPACK_READ_PAST_END_OF_BUFFER:
        return "UNPACK-PAST-END";
    case WW_ERR_NOT_SUPPORTED:
        return "NOT-SUPPORTED";
    case WW_ERR_NOT_FOUND:
        return "NOT-FOUND";
    case WW_ERR_SERVER_NOT_AVAIL:
        return "SERVER-NOT-AVAIL";
    case WW_ERR_INVALID_NAMESPACE:
        return "INVALID-NAMESPACE";
    case WW_ERR_INVALID_SIZE:
        return "INVALID-SIZE";
    case WW_ERR_INVALID_KEYVALP:
        return "INVALID-KEYVAL";
    case WW_ERR_INVALID_NUM_PARSED:
        return "INVALID-NUM-PARSED";

    case WW_ERR_INVALID_ARGS:
        return "INVALID-ARGS";
    case WW_ERR_INVALID_NUM_ARGS:
        return "INVALID-NUM-ARGS";
    case WW_ERR_INVALID_LENGTH:
        return "INVALID-LENGTH";
    case WW_ERR_INVALID_VAL_LENGTH:
        return "INVALID-VAL-LENGTH";
    case WW_ERR_INVALID_VAL:
        return "INVALID-VAL";
    case WW_ERR_INVALID_KEY_LENGTH:
        return "INVALID-KEY-LENGTH";
    case WW_ERR_INVALID_KEY:
        return "INVALID-KEY";
    case WW_ERR_INVALID_ARG:
        return "INVALID-ARG";
    case WW_ERR_NOMEM:
        return "NO-MEM";
    case WW_ERR_INIT:
        return "INIT";

    case WW_ERR_DATA_VALUE_NOT_FOUND:
        return "DATA-VALUE-NOT-FOUND";
    case WW_ERR_OUT_OF_RESOURCE:
        return "OUT-OF-RESOURCE";
    case WW_ERR_RESOURCE_BUSY:
        return "RESOURCE-BUSY";
    case WW_ERR_BAD_PARAM:
        return "BAD-PARAM";
    case WW_ERR_IN_ERRNO:
        return "ERR-IN-ERRNO";
    case WW_ERR_UNREACH:
        return "UNREACHABLE";
    case WW_ERR_TIMEOUT:
        return "TIMEOUT";
    case WW_ERR_NO_PERMISSIONS:
        return "NO-PERMISSIONS";
    case WW_ERR_PACK_MISMATCH:
        return "PACK-MISMATCH";
    case WW_ERR_PACK_FAILURE:
        return "PACK-FAILURE";

    case WW_ERR_UNPACK_FAILURE:
        return "UNPACK-FAILURE";
    case WW_ERR_UNPACK_INADEQUATE_SPACE:
        return "UNPACK-INADEQUATE-SPACE";
    case WW_ERR_TYPE_MISMATCH:
        return "TYPE-MISMATCH";
    case WW_ERR_PROC_ENTRY_NOT_FOUND:
        return "PROC-ENTRY-NOT-FOUND";
    case WW_ERR_UNKNOWN_DATA_TYPE:
        return "UNKNOWN-DATA-TYPE";
    case WW_ERR_WOULD_BLOCK:
        return "WOULD-BLOCK";
    case WW_ERR_READY_FOR_HANDSHAKE:
        return "READY-FOR-HANDSHAKE";
    case WW_ERR_HANDSHAKE_FAILED:
        return "HANDSHAKE-FAILED";
    case WW_ERR_INVALID_CRED:
        return "INVALID-CREDENTIAL";
    case WW_EXISTS:
        return "EXISTS";
    case WW_ERR_SERVER_FAILED_REQUEST:
        return "SERVER FAILED REQUEST";
    case WW_ERR_PROC_MIGRATE:
        return "PROC-MIGRATE";
    case WW_ERR_PROC_CHECKPOINT:
        return "PROC-CHECKPOINT-ERROR";
    case WW_ERR_PROC_RESTART:
        return "PROC_RESTART";
    case WW_ERR_PROC_ABORTING:
        return "PROC-ABORTING";
    case WW_ERR_PROC_REQUESTED_ABORT:
        return "PROC-ABORT-REQUESTED";
    case WW_ERR_PROC_ABORTED:
        return "PROC-ABORTED";
    case WW_ERR_DEBUGGER_RELEASE:
        return "DEBUGGER-RELEASE";
    case WW_ERR_SILENT:
        return "SILENT_ERROR";
    case WW_ERROR:
        return "ERROR";
    case WW_ERR_NOT_AVAILABLE:
        return "WW_ERR_NOT_AVAILABLE";
    case WW_ERR_FATAL:
        return "WW_ERR_FATAL";
    case WW_ERR_VALUE_OUT_OF_BOUNDS:
        return "WW_ERR_VALUE_OUT_OF_BOUNDS";
    case WW_ERR_PERM:
        return "WW_ERR_PERM";
    case WW_SUCCESS:
        return "SUCCESS";

    }
    return "ERROR STRING NOT FOUND";
}
