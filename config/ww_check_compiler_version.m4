dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
dnl Copyright (c) 2013      Intel, Inc. All rights reserved
dnl
dnl $COPYRIGHT$
dnl
dnl Additional copyrights may follow
dnl
dnl $HEADER$
dnl


# WW_CHECK_COMPILER_VERSION_ID()
# ----------------------------------------------------
# Try to figure out the compiler's name and version to detect cases,
# where users compile Warewulf with one version and compile the application
# with a different compiler.
#
AC_DEFUN([WW_CHECK_COMPILER_VERSION_ID],
[
    WW_CHECK_COMPILER(FAMILYID)
    WW_CHECK_COMPILER_STRINGIFY(FAMILYNAME)
    WW_CHECK_COMPILER(VERSION)
    WW_CHECK_COMPILER_STRINGIFY(VERSION_STR)
])dnl


AC_DEFUN([WW_CHECK_COMPILER], [
    lower=m4_tolower($1)
    AC_CACHE_CHECK([for compiler $lower], ww_cv_compiler_[$1],
    [
            CPPFLAGS_orig=$CPPFLAGS
            CPPFLAGS="-I${top_ww_srcdir}/src/include $CPPFLAGS"
            AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include "ww_portable_platform.h"

int main (int argc, char * argv[])
{
    FILE * f;
    f=fopen("conftestval", "w");
    if (!f) exit(1);
    fprintf (f, "%d", PLATFORM_COMPILER_$1);
    return 0;
}
            ], [
                eval ww_cv_compiler_$1=`cat conftestval`;
            ], [
                eval ww_cv_compiler_$1=0
            ], [
                eval ww_cv_compiler_$1=0
            ])
            CPPFLAGS=$CPPFLAGS_orig
    ])
    AC_DEFINE_UNQUOTED([WW_BUILD_PLATFORM_COMPILER_$1], $ww_cv_compiler_[$1],
                       [The compiler $lower which Warewulf was built with])
])dnl


AC_DEFUN([WW_CHECK_COMPILER_STRINGIFY], [
    lower=m4_tolower($1)
    AC_CACHE_CHECK([for compiler $lower], ww_cv_compiler_[$1],
    [
            CPPFLAGS_orig=$CPPFLAGS
            CPPFLAGS="-I${top_ww_srcdir}/src/include $CPPFLAGS"
            AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include "ww_portable_platform.h"

int main (int argc, char * argv[])
{
    FILE * f;
    f=fopen("conftestval", "w");
    if (!f) exit(1);
    fprintf (f, "%s", _STRINGIFY(PLATFORM_COMPILER_$1));
    return 0;
}
            ], [
                eval ww_cv_compiler_$1=`cat conftestval`;
            ], [
                eval ww_cv_compiler_$1=UNKNOWN
            ], [
                eval ww_cv_compiler_$1=UNKNOWN
            ])
            CPPFLAGS=$CPPFLAGS_orig
    ])
    AC_DEFINE_UNQUOTED([WW_BUILD_PLATFORM_COMPILER_$1], $ww_cv_compiler_[$1],
                       [The compiler $lower which Warewulf was built with])
])dnl
