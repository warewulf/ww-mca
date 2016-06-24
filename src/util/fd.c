/*
 * Copyright (c) 2008-2014 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2009      Sandia National Laboratories. All rights reserved.
 * Copyright (c) 2014-2016 Intel, Inc. All rights reserved.
 *
 * Copyright (c) 2015      Research Organization for Information Science
 *                         and Technology (RIST). All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/ww_config.h>

#include <ww_types.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#include <fcntl.h>

#include "src/util/error.h"
#include "src/util/fd.h"


/*
 * Simple loop over reading from a fd
 */
ww_status_t ww_fd_read(int fd, int len, void *buffer)
{
    int rc;
    char *b = buffer;

    while (len > 0) {
        rc = read(fd, b, len);
        if (rc < 0 && (EAGAIN == errno || EINTR == errno)) {
            continue;
        } else if (rc > 0) {
            len -= rc;
            b += rc;
        } else if (0 == rc) {
            return WW_ERR_TIMEOUT;
        } else {
            return WW_ERR_IN_ERRNO;
        }
    }
    return WW_SUCCESS;
}


/*
 * Simple loop over writing to an fd
 */
ww_status_t ww_fd_write(int fd, int len, const void *buffer)
{
    int rc;
    const char *b = buffer;

    while (len > 0) {
        rc = write(fd, b, len);
        if (rc < 0 && (EAGAIN == errno || EINTR == errno)) {
            continue;
        } else if (rc > 0) {
            len -= rc;
            b += rc;
        } else {
            return WW_ERR_IN_ERRNO;
        }
    }

    return WW_SUCCESS;
}


ww_status_t ww_fd_set_cloexec(int fd)
{
#ifdef FD_CLOEXEC
    int flags;

    /* Stevens says that we should get the fd's flags before we set
       them.  So say we all. */
    flags = fcntl(fd, F_GETFD, 0);
    if (-1 == flags) {
        return WW_ERR_IN_ERRNO;
    }

    if (fcntl(fd, F_SETFD, FD_CLOEXEC | flags) == -1) {
        return WW_ERR_IN_ERRNO;
    }
#endif

    return WW_SUCCESS;
}
