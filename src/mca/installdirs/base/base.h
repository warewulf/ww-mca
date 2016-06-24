/*
 * Copyright (c) 2006-2013 Los Alamos National Security, LLC.  All rights
 *                         reserved.
 * Copyright (c) 2007-2010 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2010      Sandia National Laboratories. All rights reserved.
 * Copyright (c) 2016      Intel, Inc. All rights reserved
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 */

#ifndef WW_INSTALLDIRS_BASE_H
#define WW_INSTALLDIRS_BASE_H

#include <src/include/ww_config.h>
#include "src/mca/base/mca_base_framework.h"
#include "src/mca/installdirs/installdirs.h"

/*
 * Global functions for MCA overall installdirs open and close
 */
BEGIN_C_DECLS

/**
 * Framework structure declaration
 */
WW_DECLSPEC extern mca_base_framework_t ww_installdirs_base_framework;

/* Just like ww_install_dirs_expand() (see installdirs.h), but will
   also insert the value of the environment variable $WW_DESTDIR, if
   it exists/is set.  This function should *only* be used during the
   setup routines of installdirs. */
char * ww_install_dirs_expand_setup(const char* input);

END_C_DECLS

#endif /* WW_BASE_INSTALLDIRS_H */
