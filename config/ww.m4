dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2004-2010 The Trustees of Indiana University and Indiana
dnl                         University Research and Technology
dnl                         Corporation.  All rights reserved.
dnl Copyright (c) 2004-2005 The University of Tennessee and The University
dnl                         of Tennessee Research Foundation.  All rights
dnl                         reserved.
dnl Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
dnl                         University of Stuttgart.  All rights reserved.
dnl Copyright (c) 2004-2005 The Regents of the University of California.
dnl                         All rights reserved.
dnl Copyright (c) 2006-2016 Cisco Systems, Inc.  All rights reserved.
dnl Copyright (c) 2007      Sun Microsystems, Inc.  All rights reserved.
dnl Copyright (c) 2009      IBM Corporation.  All rights reserved.
dnl Copyright (c) 2009      Los Alamos National Security, LLC.  All rights
dnl                         reserved.
dnl Copyright (c) 2009-2011 Oak Ridge National Labs.  All rights reserved.
dnl Copyright (c) 2011-2013 NVIDIA Corporation.  All rights reserved.
dnl Copyright (c) 2013-2016 Intel, Inc. All rights reserved
dnl Copyright (c) 2015      Research Organization for Information Science
dnl                         and Technology (RIST). All rights reserved.
dnl Copyright (c) 2016      Mellanox Technologies, Inc.
dnl                         All rights reserved.
dnl
dnl $COPYRIGHT$
dnl
dnl Additional copyrights may follow
dnl
dnl $HEADER$
dnl

AC_DEFUN([WW_SETUP_CORE],[

    AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])
    AC_REQUIRE([AC_CANONICAL_TARGET])
    AC_REQUIRE([AC_PROG_CC])

    # If no prefix was defined, set a good value
    m4_ifval([$1],
             [m4_define([ww_config_prefix],[$1/])],
             [m4_define([ww_config_prefix], [])])

    # Get Warewulf's absolute top builddir (which may not be the same as
    # the real $top_builddir)
    WW_startdir=`pwd`
    if test x"ww_config_prefix" != "x" && test ! -d "ww_config_prefix"; then
        mkdir -p "ww_config_prefix"
    fi
    if test x"ww_config_prefix" != "x"; then
        cd "ww_config_prefix"
    fi
    WW_top_builddir=`pwd`
    AC_SUBST(WW_top_builddir)

    # Get Warewulf's absolute top srcdir (which may not be the same as the
    # real $top_srcdir.  First, go back to the startdir incase the
    # $srcdir is relative.

    cd "$WW_startdir"
    cd "$srcdir"/ww_config_prefix
    WW_top_srcdir="`pwd`"
    AC_SUBST(WW_top_srcdir)

    # Go back to where we started
    cd "$WW_startdir"

    AC_MSG_NOTICE([Warewulf builddir: $WW_top_builddir])
    AC_MSG_NOTICE([Warewulf srcdir: $WW_top_srcdir])
    if test "$WW_top_builddir" != "$WW_top_srcdir"; then
        AC_MSG_NOTICE([Detected VPATH build])
    fi

    # Get the version of Warewulf that we are installing
    AC_MSG_CHECKING([for Warewulf version])
    WW_VERSION="`$WW_top_srcdir/config/ww_get_version.sh $WW_top_srcdir/VERSION`"
    if test "$?" != "0"; then
        AC_MSG_ERROR([Cannot continue])
    fi
    AC_MSG_RESULT([$WW_VERSION])
    AC_SUBST(WW_VERSION)

    WW_MAJOR_VERSION="`$WW_top_srcdir/config/ww_get_version.sh $WW_top_srcdir/VERSION --major`"
    if test "$?" != "0"; then
        AC_MSG_ERROR([Cannot continue])
    fi
    AC_DEFINE_UNQUOTED([WW_MAJOR_VERSION], [$WW_MAJOR_VERSION], [Warewulf Major Version])

    WW_MINOR_VERSION="`$WW_top_srcdir/config/ww_get_version.sh $WW_top_srcdir/VERSION --minor`"
    if test "$?" != "0"; then
        AC_MSG_ERROR([Cannot continue])
    fi
    AC_DEFINE_UNQUOTED([WW_MINOR_VERSION], [$WW_MINOR_VERSION], [Warewulf Minor Version])

    WW_RELEASE_VERSION="`$WW_top_srcdir/config/ww_get_version.sh $WW_top_srcdir/VERSION --release`"
    if test "$?" != "0"; then
        AC_MSG_ERROR([Cannot continue])
    fi
    AC_DEFINE_UNQUOTED([WW_RELEASE_VERSION], [$WW_RELEASE_VERSION], [Warewulf Release Version])

    WW_RELEASE_DATE="`$WW_top_srcdir/config/ww_get_version.sh $WW_top_srcdir/VERSION --release-date`"
    AC_DEFINE_UNQUOTED([WW_VERSION], ["$WW_VERSION"],
                       [The library version is always available, contrary to VERSION])
    AC_SUBST(WW_RELEASE_DATE)

    # Debug mode?
    AC_MSG_CHECKING([if want Warewulf maintainer support])
    ww_debug=
    AS_IF([test "$ww_debug" = "" && test "$enable_debug" = "yes"],
          [ww_debug=1
           ww_debug_msg="enabled"])
    AS_IF([test "$ww_debug" = ""],
          [ww_debug=0
           ww_debug_msg="disabled"])
    # Grr; we use #ifndef for WW_DEBUG!  :-(
    AH_TEMPLATE(WW_ENABLE_DEBUG, [Whether we are in debugging mode or not])
    AS_IF([test "$ww_debug" = "1"], [AC_DEFINE([WW_ENABLE_DEBUG])])
    AC_MSG_RESULT([$ww_debug_msg])

    AC_MSG_CHECKING([for Warewulf directory prefix])
    AC_MSG_RESULT(m4_ifval([$1], ww_config_prefix, [(none)]))

    # Configure the header file
    AC_CONFIG_HEADERS(ww_config_prefix[include/warewulf/autogen/config.h])

    # GCC specifics.
    if test "x$GCC" = "xyes"; then
        WW_GCC_CFLAGS="-Wall -Wmissing-prototypes -Wundef"
        WW_GCC_CFLAGS="$WW_GCC_CFLAGS -Wpointer-arith -Wcast-align"
    fi

    ############################################################################
    # Check for compilers and preprocessors
    ############################################################################
    ww_show_title "Compiler and preprocessor tests"

    #
    # Check for some types
    #

    AC_CHECK_TYPES(int8_t)
    AC_CHECK_TYPES(uint8_t)
    AC_CHECK_TYPES(int16_t)
    AC_CHECK_TYPES(uint16_t)
    AC_CHECK_TYPES(int32_t)
    AC_CHECK_TYPES(uint32_t)
    AC_CHECK_TYPES(int64_t)
    AC_CHECK_TYPES(uint64_t)
    AC_CHECK_TYPES(long long)

    AC_CHECK_TYPES(intptr_t)
    AC_CHECK_TYPES(uintptr_t)
    AC_CHECK_TYPES(ptrdiff_t)

    #
    # Check for type sizes
    #

    AC_CHECK_SIZEOF(_Bool)
    AC_CHECK_SIZEOF(char)
    AC_CHECK_SIZEOF(short)
    AC_CHECK_SIZEOF(int)
    AC_CHECK_SIZEOF(long)
    if test "$ac_cv_type_long_long" = yes; then
        AC_CHECK_SIZEOF(long long)
    fi
    AC_CHECK_SIZEOF(float)
    AC_CHECK_SIZEOF(double)

    AC_CHECK_SIZEOF(void *)
    AC_CHECK_SIZEOF(size_t)
    if test "$ac_cv_type_ssize_t" = yes ; then
        AC_CHECK_SIZEOF(ssize_t)
    fi
    if test "$ac_cv_type_ptrdiff_t" = yes; then
        AC_CHECK_SIZEOF(ptrdiff_t)
    fi
    AC_CHECK_SIZEOF(wchar_t)

    AC_CHECK_SIZEOF(pid_t)

    #
    # Check for type alignments
    #

    WW_C_GET_ALIGNMENT(bool, WW_ALIGNMENT_BOOL)
    WW_C_GET_ALIGNMENT(int8_t, WW_ALIGNMENT_INT8)
    WW_C_GET_ALIGNMENT(int16_t, WW_ALIGNMENT_INT16)
    WW_C_GET_ALIGNMENT(int32_t, WW_ALIGNMENT_INT32)
    WW_C_GET_ALIGNMENT(int64_t, WW_ALIGNMENT_INT64)
    WW_C_GET_ALIGNMENT(char, WW_ALIGNMENT_CHAR)
    WW_C_GET_ALIGNMENT(short, WW_ALIGNMENT_SHORT)
    WW_C_GET_ALIGNMENT(wchar_t, WW_ALIGNMENT_WCHAR)
    WW_C_GET_ALIGNMENT(int, WW_ALIGNMENT_INT)
    WW_C_GET_ALIGNMENT(long, WW_ALIGNMENT_LONG)
    if test "$ac_cv_type_long_long" = yes; then
        WW_C_GET_ALIGNMENT(long long, WW_ALIGNMENT_LONG_LONG)
    fi
    WW_C_GET_ALIGNMENT(float, WW_ALIGNMENT_FLOAT)
    WW_C_GET_ALIGNMENT(double, WW_ALIGNMENT_DOUBLE)
    if test "$ac_cv_type_long_double" = yes; then
        WW_C_GET_ALIGNMENT(long double, WW_ALIGNMENT_LONG_DOUBLE)
    fi
    WW_C_GET_ALIGNMENT(void *, WW_ALIGNMENT_VOID_P)
    WW_C_GET_ALIGNMENT(size_t, WW_ALIGNMENT_SIZE_T)


    #
    # Does the C compiler native support "bool"? (i.e., without
    # <stdbool.h> or any other help)
    #

    WW_VAR_SCOPE_PUSH([MSG])
    AC_MSG_CHECKING(for C bool type)
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
                                          AC_INCLUDES_DEFAULT],
                                       [[bool bar, foo = true; bar = foo;]])],
                      [WW_NEED_C_BOOL=0 MSG=yes],[WW_NEED_C_BOOL=1 MSG=no])
    AC_DEFINE_UNQUOTED(WW_NEED_C_BOOL, $WW_NEED_C_BOOL,
                       [Whether the C compiler supports "bool" without any other help (such as <stdbool.h>)])
    AC_MSG_RESULT([$MSG])
    AC_CHECK_SIZEOF(_Bool)
    WW_VAR_SCOPE_POP

    #
    # Check for other compiler characteristics
    #

    WW_VAR_SCOPE_PUSH([WW_CFLAGS_save])
    if test "$GCC" = "yes"; then

        # gcc 2.96 will emit oodles of warnings if you use "inline" with
        # -pedantic (which we do in developer builds).  However,
        # "__inline__" is ok.  So we have to force gcc to select the
        # right one.  If you use -pedantic, the AC_C_INLINE test will fail
        # (because it names a function foo() -- without the (void)).  So
        # we turn off all the picky flags, turn on -ansi mode (which is
        # implied by -pedantic), and set warnings to be errors.  Hence,
        # this does the following (for 2.96):
        #
        # - causes the check for "inline" to emit a warning, which then
        # fails
        # - checks for __inline__, which then emits no error, and works
        #
        # This also works nicely for gcc 3.x because "inline" will work on
        # the first check, and all is fine.  :-)

        WW_CFLAGS_save=$CFLAGS
        CFLAGS="$WW_CFLAGS_BEFORE_PICKY -Werror -ansi"
    fi
    AC_C_INLINE
    if test "$GCC" = "yes"; then
        CFLAGS=$WW_CFLAGS_save
    fi
    WW_VAR_SCOPE_POP

    if test "x$CC" = "xicc"; then
        WW_CHECK_ICC_VARARGS
    fi


    ##################################
    # Only after setting up
    # C do we check compiler attributes.
    ##################################

    ww_show_subtitle "Compiler characteristics"

    WW_CHECK_ATTRIBUTES
    WW_CHECK_COMPILER_VERSION_ID

    ##################################
    # Header files
    ##################################

    ww_show_title "Header file tests"

    AC_CHECK_HEADERS([arpa/inet.h \
                      fcntl.h inttypes.h libgen.h \
                      netinet/in.h \
                      stdint.h stddef.h \
                      stdlib.h string.h strings.h \
                      sys/param.h \
                      sys/select.h sys/socket.h \
                      stdarg.h sys/stat.h sys/time.h \
                      sys/types.h sys/un.h sys/uio.h net/uio.h \
                      sys/wait.h syslog.h \
                      time.h unistd.h \
                      crt_externs.h signal.h \
                      ioLib.h sockLib.h hostLib.h limits.h \
                      sys/statfs.h sys/statvfs.h])

    # Note that sometimes we have <stdbool.h>, but it doesn't work (e.g.,
    # have both Portland and GNU installed; using pgcc will find GNU's
    # <stdbool.h>, which all it does -- by standard -- is define "bool" to
    # "_Bool" [see
    # http://www.opengroup.org/onlinepubs/009695399/basedefs/stdbool.h.html],
    # and Portland has no idea what to do with _Bool).

    # So first figure out if we have <stdbool.h> (i.e., check the value of
    # the macro HAVE_STDBOOL_H from the result of AC_CHECK_HEADERS,
    # above).  If we do have it, then check to see if it actually works.
    # Define WW_USE_STDBOOL_H as approrpaite.
    AC_CHECK_HEADERS([stdbool.h], [have_stdbool_h=1], [have_stdbool_h=0])
    AC_MSG_CHECKING([if <stdbool.h> works])
    if test "$have_stdbool_h" = "1"; then
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([AC_INCLUDES_DEFAULT[
                                                   #if HAVE_STDBOOL_H
                                                   #include <stdbool.h>
                                                   #endif
                                               ]],
                                           [[bool bar, foo = true; bar = foo;]])],
                          [WW_USE_STDBOOL_H=1 MSG=yes],[WW_USE_STDBOOL_H=0 MSG=no])
    else
        WW_USE_STDBOOL_H=0
        MSG="no (don't have <stdbool.h>)"
    fi
    AC_DEFINE_UNQUOTED(WW_USE_STDBOOL_H, $WW_USE_STDBOOL_H,
                       [Whether to use <stdbool.h> or not])
    AC_MSG_RESULT([$MSG])

    # checkpoint results
    AC_CACHE_SAVE

    ##################################
    # Types
    ##################################

    ww_show_title "Type tests"

    AC_CHECK_TYPES([socklen_t, struct sockaddr_in, struct sockaddr_un,
                    struct sockaddr_in6, struct sockaddr_storage],
                   [], [], [AC_INCLUDES_DEFAULT
                            #if HAVE_SYS_SOCKET_H
                            #include <sys/socket.h>
                            #endif
                            #if HAVE_SYS_UN_H
                            #include <sys/un.h>
                            #endif
                            #ifdef HAVE_NETINET_IN_H
                            #include <netinet/in.h>
                            #endif
                           ])

    AC_CHECK_DECLS([AF_UNSPEC, PF_UNSPEC, AF_INET6, PF_INET6],
                   [], [], [AC_INCLUDES_DEFAULT
                            #if HAVE_SYS_SOCKET_H
                            #include <sys/socket.h>
                            #endif
                            #ifdef HAVE_NETINET_IN_H
                            #include <netinet/in.h>
                            #endif
                           ])

    # SA_RESTART in signal.h
    WW_VAR_SCOPE_PUSH([MSG2])
    AC_MSG_CHECKING([if SA_RESTART defined in signal.h])
                        AC_EGREP_CPP(yes, [
                                            #include <signal.h>
                                            #ifdef SA_RESTART
                                            yes
                                            #endif
                                        ], [MSG2=yes VALUE=1], [MSG2=no VALUE=0])
    AC_DEFINE_UNQUOTED(WW_HAVE_SA_RESTART, $VALUE,
                       [Whether we have SA_RESTART in <signal.h> or not])
    AC_MSG_RESULT([$MSG2])
    WW_VAR_SCOPE_POP

    AC_CHECK_MEMBERS([struct sockaddr.sa_len], [], [], [
                         #include <sys/types.h>
                         #if HAVE_SYS_SOCKET_H
                         #include <sys/socket.h>
                         #endif
                     ])

    AC_CHECK_MEMBERS([struct dirent.d_type], [], [], [
                         #include <sys/types.h>
                         #include <dirent.h>])

    AC_CHECK_MEMBERS([siginfo_t.si_fd],,,[#include <signal.h>])
    AC_CHECK_MEMBERS([siginfo_t.si_band],,,[#include <signal.h>])

    #
    # Checks for struct member names in struct statfs
    #
    AC_CHECK_MEMBERS([struct statfs.f_type], [], [], [
                         AC_INCLUDES_DEFAULT
                         #ifdef HAVE_SYS_VFS_H
                         #include <sys/vfs.h>
                         #endif
                         #ifdef HAVE_SYS_STATFS_H
                         #include <sys/statfs.h>
                         #endif
                     ])

    AC_CHECK_MEMBERS([struct statfs.f_fstypename], [], [], [
                         AC_INCLUDES_DEFAULT
                         #ifdef HAVE_SYS_PARAM_H
                         #include <sys/param.h>
                         #endif
                         #ifdef HAVE_SYS_MOUNT_H
                         #include <sys/mount.h>
                         #endif
                         #ifdef HAVE_SYS_VFS_H
                         #include <sys/vfs.h>
                         #endif
                         #ifdef HAVE_SYS_STATFS_H
                         #include <sys/statfs.h>
                         #endif
                     ])

    #
    # Checks for struct member names in struct statvfs
    #
    AC_CHECK_MEMBERS([struct statvfs.f_basetype], [], [], [
                         AC_INCLUDES_DEFAULT
                         #ifdef HAVE_SYS_STATVFS_H
                         #include <sys/statvfs.h>
                         #endif
                     ])

    AC_CHECK_MEMBERS([struct statvfs.f_fstypename], [], [], [
                         AC_INCLUDES_DEFAULT
                         #ifdef HAVE_SYS_STATVFS_H
                         #include <sys/statvfs.h>
                         #endif
                     ])

    #
    # Check for ptrdiff type.  Yes, there are platforms where
    # sizeof(void*) != sizeof(long) (64 bit Windows, apparently).
    #
    AC_MSG_CHECKING([for pointer diff type])
    if test $ac_cv_type_ptrdiff_t = yes ; then
        ww_ptrdiff_t="ptrdiff_t"
        ww_ptrdiff_size=$ac_cv_sizeof_ptrdiff_t
    elif test $ac_cv_sizeof_void_p -eq $ac_cv_sizeof_long ; then
        ww_ptrdiff_t="long"
        ww_ptrdiff_size=$ac_cv_sizeof_long
    elif test $ac_cv_type_long_long = yes && test $ac_cv_sizeof_void_p -eq $ac_cv_sizeof_long_long ; then
        ww_ptrdiff_t="long long"
        ww_ptrdiff_size=$ac_cv_sizeof_long_long
        #else
        #    AC_MSG_ERROR([Could not find datatype to emulate ptrdiff_t.  Cannot continue])
    fi
    AC_DEFINE_UNQUOTED([WW_PTRDIFF_TYPE], [$ww_ptrdiff_t],
                       [type to use for ptrdiff_t])
    AC_MSG_RESULT([$ww_ptrdiff_t (size: $ww_ptrdiff_size)])

    ##################################
    # Libraries
    ##################################

    ww_show_title "Library and Function tests"

    WW_SEARCH_LIBS_CORE([socket], [socket])

    # IRIX and CentOS have dirname in -lgen, usually in libc
    WW_SEARCH_LIBS_CORE([dirname], [gen])

    # Darwin doesn't need -lm, as it's a symlink to libSystem.dylib
    WW_SEARCH_LIBS_CORE([ceil], [m])

    AC_CHECK_FUNCS([asprintf snprintf vasprintf vsnprintf strsignal socketpair strncpy_s usleep statfs statvfs])

    # On some hosts, htonl is a define, so the AC_CHECK_FUNC will get
    # confused.  On others, it's in the standard library, but stubbed with
    # the magic glibc foo as not implemented.  and on other systems, it's
    # just not there.  This covers all cases.
    AC_CACHE_CHECK([for htonl define],
                   [ompi_cv_htonl_define],
                   [AC_PREPROC_IFELSE([AC_LANG_PROGRAM([
                                                          #ifdef HAVE_SYS_TYPES_H
                                                          #include <sys/types.h>
                                                          #endif
                                                          #ifdef HAVE_NETINET_IN_H
                                                          #include <netinet/in.h>
                                                          #endif
                                                          #ifdef HAVE_ARPA_INET_H
                                                          #include <arpa/inet.h>
                                                          #endif],[
                                                          #ifndef ntohl
                                                          #error "ntohl not defined"
                                                          #endif
                                                      ])], [ompi_cv_htonl_define=yes], [ompi_cv_htonl_define=no])])
    AC_CHECK_FUNC([htonl], [ompi_have_htonl=yes], [ompi_have_htonl=no])
    AS_IF([test "$ompi_cv_htonl_define" = "yes" || test "$ompi_have_htonl" = "yes"],
          [AC_DEFINE_UNQUOTED([HAVE_UNIX_BYTESWAP], [1],
                              [whether unix byteswap routines -- htonl, htons, nothl, ntohs -- are available])])

    #
    # Make sure we can copy va_lists (need check declared, not linkable)
    #

    AC_CHECK_DECL(va_copy, WW_HAVE_VA_COPY=1, WW_HAVE_VA_COPY=0,
                  [#include <stdarg.h>])
    AC_DEFINE_UNQUOTED(WW_HAVE_VA_COPY, $WW_HAVE_VA_COPY,
                       [Whether we have va_copy or not])

    AC_CHECK_DECL(__va_copy, WW_HAVE_UNDERSCORE_VA_COPY=1,
                  WW_HAVE_UNDERSCORE_VA_COPY=0, [#include <stdarg.h>])
    AC_DEFINE_UNQUOTED(WW_HAVE_UNDERSCORE_VA_COPY, $WW_HAVE_UNDERSCORE_VA_COPY,
                       [Whether we have __va_copy or not])

    AC_CHECK_DECLS(__func__)

    # checkpoint results
    AC_CACHE_SAVE

    ##################################
    # System-specific tests
    ##################################

    ww_show_title "System-specific tests"

    AC_C_BIGENDIAN
    WW_CHECK_BROKEN_QSORT

    ##################################
    # Visibility
    ##################################

    # Check the visibility declspec at the end to avoid problem with
    # the previous tests that are not necessarily prepared for
    # the visibility feature.
    ww_show_title "Symbol visibility feature"

    WW_CHECK_VISIBILITY

    ##################################
    # Libevent
    ##################################
    ww_show_title "Libevent"

    WW_LIBEVENT_CONFIG

    ##################################
    # MCA
    ##################################

    ww_show_title "Modular Component Architecture (MCA) setup"

    AC_MSG_CHECKING([for subdir args])
    WW_CONFIG_SUBDIR_ARGS([ww_subdir_args])
    AC_MSG_RESULT([$ww_subdir_args])

    WW_MCA

    ############################################################################
    # final compiler config
    ############################################################################

    ww_show_subtitle "Set path-related compiler flags"

    #
    # This is needed for VPATH builds, so that it will -I the appropriate
    # include directory.  We delayed doing it until now just so that
    # '-I$(top_srcdir)' doesn't show up in any of the configure output --
    # purely aesthetic.
    #
    # Because ww_config.h is created by AC_CONFIG_HEADERS, we
    # don't need to -I the builddir for Warewulf/include. However, if we
    # are VPATH building, we do need to include the source directories.
    #
    if test "$WW_top_builddir" != "$WW_top_srcdir"; then
        # Note the embedded m4 directives here -- we must embed them
        # rather than have successive assignments to these shell
        # variables, lest the $(foo) names try to get evaluated here.
        # Yuck!
        CPPFLAGS='-I$(WW_top_builddir) -I$(WW_top_srcdir) -I$(WW_top_srcdir)/src -I$(WW_top_builddir)/include -I$(WW_top_srcdir)/include'" $CPPFLAGS"
    else
        CPPFLAGS='-I$(WW_top_srcdir) -I$(WW_top_srcdir)/src -I$(WW_top_srcdir)/include'" $CPPFLAGS"
    fi

    # wwdatadir, wwlibdir, and wwinclude are essentially the same as
    # pkg*dir, but will always be */warewulf.
    wwdatadir='${datadir}/warewulf'
    wwlibdir='${libdir}/warewulf'
    Wwwincludedir='${includedir}/warewulf'
    AC_SUBST(wwdatadir)
    AC_SUBST(wwlibdir)
    AC_SUBST(wwincludedir)

    ############################################################################
    # final output
    ############################################################################

    ww_show_subtitle "Final output"

    AC_CONFIG_FILES(
        ww_config_prefix[Makefile]
        ww_config_prefix[config/Makefile]
        ww_config_prefix[include/Makefile]
        ww_config_prefix[src/Makefile]
        ww_config_prefix[src/mca/base/Makefile]
        warewulf.spec
        bin/Makefile
        etc/Makefile
        lib/Makefile
        lib/Warewulf/ACVars.pm
        lib/Warewulf/Makefile
        )

    # Success
    $2
])dnl

AC_DEFUN([WW_DEFINE_ARGS],[
#
# Is this a developer copy?
#

if test -d .git; then
    WW_DEVEL=1
else
    WW_DEVEL=0
fi


#
# Developer picky compiler options
#

AC_MSG_CHECKING([if want developer-level compiler pickyness])
AC_ARG_ENABLE(picky,
    AC_HELP_STRING([--enable-picky],
                   [enable developer-level compiler pickyness when building Warewulf (default: disabled)]))
if test "$enable_picky" = "yes"; then
    AC_MSG_RESULT([yes])
    WANT_PICKY_COMPILER=1
else
    AC_MSG_RESULT([no])
    WANT_PICKY_COMPILER=0
fi
#################### Early development override ####################
if test "$WANT_PICKY_COMPILER" = "0" && test -z "$enable_picky" && test "$WW_DEVEL" = "1"; then
    WANT_PICKY_COMPILER=1
    echo "--> developer override: enable picky compiler by default"
fi
#################### Early development override ####################

#
# Developer debugging
#

AC_MSG_CHECKING([if want developer-level debugging code])
AC_ARG_ENABLE(debug,
    AC_HELP_STRING([--enable-debug],
                   [enable developer-level debugging code (not for general Warewulf users!) (default: disabled)]))
if test "$enable_debug" = "yes"; then
    AC_MSG_RESULT([yes])
    WANT_DEBUG=1
else
    AC_MSG_RESULT([no])
    WANT_DEBUG=0
fi
#################### Early development override ####################
if test "$WANT_DEBUG" = "0" && test -z "$enable_debug" && test "$WW_DEVEL" = "1"; then
    WANT_DEBUG=1
    echo "--> developer override: enable debugging code by default"
fi
#################### Early development override ####################
if test "$WANT_DEBUG" = "0"; then
    CFLAGS="-DNDEBUG $CFLAGS"
    CXXFLAGS="-DNDEBUG $CXXFLAGS"
fi
AC_DEFINE_UNQUOTED(WW_ENABLE_DEBUG, $WANT_DEBUG,
                   [Whether we want developer-level debugging code or not])

AC_ARG_ENABLE(debug-symbols,
              AC_HELP_STRING([--disable-debug-symbols],
                             [Disable adding compiler flags to enable debugging symbols if --enable-debug is specified.  For non-debugging builds, this flag has no effect.]))

#
# Do we want to install the internal devel headers?
#
AC_MSG_CHECKING([if want to install project-internal header files])
AC_ARG_WITH(devel-headers,
    AC_HELP_STRING([--with-devel-headers],
                   [normal Warewulf users/applications do not need this (ww.h and friends are ALWAYS installed).  Developer headers are only necessary for authors doing deeper integration (default: disabled).]))
if test "$with_devel_headers" = "yes"; then
    AC_MSG_RESULT([yes])
    WANT_INSTALL_HEADERS=1
else
    AC_MSG_RESULT([no])
    WANT_INSTALL_HEADERS=0
fi
AM_CONDITIONAL(WANT_INSTALL_HEADERS, test "$WANT_INSTALL_HEADERS" = 1)

#
# Support per-user config files?
#
AC_ARG_ENABLE([per-user-config-files],
   [AC_HELP_STRING([--enable-per-user-config-files],
      [Disable per-user configuration files, to save disk accesses during job start-up.  This is likely desirable for large jobs.  Note that this can also be acheived by environment variables at run-time.  (default: enabled)])])
if test "$enable_per_user_config_files" = "no" ; then
  result=0
else
  result=1
fi
AC_DEFINE_UNQUOTED([WW_WANT_HOME_CONFIG_FILES], [$result],
     [Enable per-user config files])

#
# Do we want the pretty-print stack trace feature?
#

AC_MSG_CHECKING([if want pretty-print stacktrace])
AC_ARG_ENABLE([pretty-print-stacktrace],
              [AC_HELP_STRING([--enable-pretty-print-stacktrace],
                              [Pretty print stacktrace on process signal (default: enabled)])])
if test "$enable_pretty_print_stacktrace" = "no" ; then
    AC_MSG_RESULT([no])
    WANT_PRETTY_PRINT_STACKTRACE=0
else
    AC_MSG_RESULT([yes])
    WANT_PRETTY_PRINT_STACKTRACE=1
fi
AC_DEFINE_UNQUOTED([WW_WANT_PRETTY_PRINT_STACKTRACE],
                   [$WANT_PRETTY_PRINT_STACKTRACE],
                   [if want pretty-print stack trace feature])

#
# Ident string
#
AC_MSG_CHECKING([if want ident string])
AC_ARG_WITH([ident-string],
            [AC_HELP_STRING([--with-ident-string=STRING],
                            [Embed an ident string into Warewulf object files])])
if test "$with_ident_string" = "" || test "$with_ident_string" = "no"; then
    with_ident_string="%VERSION%"
fi
# This is complicated, because $WW_VERSION may have spaces in it.
# So put the whole sed expr in single quotes -- i.e., directly
# substitute %VERSION% for (not expanded) $WW_VERSION.
with_ident_string="`echo $with_ident_string | sed -e 's/%VERSION%/$WW_VERSION/'`"

# Now eval an echo of that so that the "$WW_VERSION" token is
# replaced with its value.  Enclose the whole thing in "" so that it
# ends up as 1 token.
with_ident_string="`eval echo $with_ident_string`"

AC_DEFINE_UNQUOTED([WW_IDENT_STRING], ["$with_ident_string"],
                   [ident string for Warewulf])
AC_MSG_RESULT([$with_ident_string])

])dnl
