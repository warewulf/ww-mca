/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2011      Sandia National Laboratories. All rights reserved.
 * Copyright (c) 2014      Intel, Inc. All rights reserved
 * Copyright (c) 2016      Los Alamos National Security, LLC. All rights
 *                         reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/*
 * List of supported architectures
 */

#ifndef WW_SYS_ARCHITECTURE_H
#define WW_SYS_ARCHITECTURE_H

/* Architectures */
#define WW_UNSUPPORTED    0000
#define WW_IA32           0010
#define WW_IA64           0020
#define WW_AMD64          0030
#define WW_POWERPC32      0050
#define WW_POWERPC64      0051
#define WW_SPARC          0060
#define WW_SPARCV9_32     0061
#define WW_SPARCV9_64     0062
#define WW_MIPS           0070
#define WW_ARM            0100
#define WW_ARM64          0101
#define WW_BUILTIN_SYNC   0200
#define WW_BUILTIN_OSX    0201
#define WW_BUILTIN_GCC    0202
#define WW_BUILTIN_NO     0203

/* Formats */
#define WW_DEFAULT        1000  /* standard for given architecture */
#define WW_DARWIN         1001  /* Darwin / OS X on PowerPC */
#define WW_PPC_LINUX      1002  /* Linux on PowerPC */
#define WW_AIX            1003  /* AIX on Power / PowerPC */

#endif /* #ifndef WW_SYS_ARCHITECTURE_H */
