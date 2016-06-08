# -*- shell-script -*-
#
# Copyright (c) 2009-2015 Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2013      Los Alamos National Security, LLC.  All rights reserved.
# Copyright (c) 2013-2015 Intel, Inc. All rights reserved
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

# MCA_libevent_CONFIG([action-if-found], [action-if-not-found])
# --------------------------------------------------------------------
AC_DEFUN([WW_LIBEVENT_CONFIG],[
    AC_ARG_WITH([libevent-header],
                [AC_HELP_STRING([--with-libevent-header=HEADER],
                                [The value that should be included in C files to include event.h])])

    AS_IF([test "$enable_embedded_mode" = "yes"],
          [_WW_LIBEVENT_EMBEDDED_MODE],
          [_WW_LIBEVENT_EXTERNAL])

    AC_MSG_CHECKING([libevent header])
    AC_DEFINE_UNQUOTED([WW_EVENT_HEADER], [$WW_EVENT_HEADER],
                       [Location of event.h])
    AC_MSG_RESULT([$WW_EVENT_HEADER])
    AC_MSG_CHECKING([libevent2/thread header])
    AC_DEFINE_UNQUOTED([WW_EVENT2_THREAD_HEADER], [$WW_EVENT2_THREAD_HEADER],
                       [Location of event2/thread.h])
    AC_MSG_RESULT([$WW_EVENT2_THREAD_HEADER])

    CPPFLAGS="$CPPFLAGS $WW_EVENT_CPPFLAGS"
    LDFLAGS="$LDFLAGS $WW_EVENT_LDFLAGS"
    LIBS="$LIBS $WW_EVENT_LIBS"
])

AC_DEFUN([_WW_LIBEVENT_EMBEDDED_MODE],[
    AC_MSG_CHECKING([for libevent])
    AC_MSG_RESULT([assumed available (embedded mode)])

    WW_EVENT_HEADER="$with_libevent_header"
    WW_EVENT2_THREAD_HEADER="$with_libevent_header"
    WW_EVENT_CPPFLAGS=
    WW_EVENT_LIB=
    WW_EVENT_LDFLAGS=

 ])

AC_DEFUN([_WW_LIBEVENT_EXTERNAL],[
    WW_VAR_SCOPE_PUSH([ww_event_dir ww_event_libdir])

    AC_ARG_WITH([libevent],
                [AC_HELP_STRING([--with-libevent=DIR],
                                [Search for libevent headers and libraries in DIR ])])

    # Bozo check
    AS_IF([test "$with_libevent" = "no"],
          [AC_MSG_WARN([It is not possible to configure Warewulf --without-libevent])
           AC_MSG_ERROR([Cannot continue])])

    AC_ARG_WITH([libevent-libdir],
                [AC_HELP_STRING([--with-libevent-libdir=DIR],
                                [Search for libevent libraries in DIR ])])

    AC_MSG_CHECKING([for libevent in])
    if test ! -z "$with_libevent" && test "$with_libevent" != "yes"; then
        ww_event_dir=$with_libevent
        if test -d $with_libevent/lib; then
            ww_event_libdir=$with_libevent/lib
        elif test -d $with_libevent/lib64; then
            ww_event_libdir=$with_libevent/lib64
        else
            AC_MSG_RESULT([Could not find $with_libevent/lib or $with_libevent/lib64])
            AC_MSG_ERROR([Can not continue])
        fi
        AC_MSG_RESULT([$ww_event_dir and $ww_event_libdir])
    else
        AC_MSG_RESULT([(default search paths)])
    fi
    AS_IF([test ! -z "$with_libevent_libdir" && "$with_libevent_libdir" != "yes"],
          [ww_event_libdir="$with_libevent_libdir"])

    WW_CHECK_PACKAGE([ww_libevent],
                       [event.h],
                       [event],
                       [event_config_new],
                       [-levent -levent_pthreads],
                       [$ww_event_dir],
                       [$ww_event_libdir],
                       [],
                       [AC_MSG_WARN([LIBEVENT SUPPORT NOT FOUND])
                        AC_MSG_ERROR([CANNOT CONTINE])])

    CPPFLAGS="$ww_libevent_CPPFLAGS $CPPFLAGS"
    LIBS="$ww_libevent_LIBS $LIBS"
    LDFLAGS="$ww_libevent_LDFLAGS $LDFLAGS"


    # Ensure that this libevent has the symbol
    # "evthread_set_lock_callbacks", which will only exist if
    # libevent was configured with thread support.
    AC_CHECK_LIB([event], [evthread_set_lock_callbacks],
                 [],
                 [AC_MSG_WARN([External libevent does not have thread support])
                  AC_MSG_WARN([Warewulf requires libevent to be compiled with])
                  AC_MSG_WARN([thread support enabled])
                  AC_MSG_ERROR([Cannot continue])])
    AC_CHECK_LIB([event_pthreads], [evthread_use_pthreads],
                 [],
                 [AC_MSG_WARN([External libevent does not have thread support])
                  AC_MSG_WARN([Warewulf requires libevent to be compiled with])
                  AC_MSG_WARN([thread support enabled])
                  AC_MSG_ERROR([Cannot continue])])
    # Chck if this libevent has the symbol
    # "libevent_global_shutdown", which will only exist in
    # libevent version 2.1.1+
    AC_CHECK_FUNCS([libevent_global_shutdown],[], [])

    # Set output variables
    WW_EVENT_HEADER="<event.h>"
    WW_EVENT2_THREAD_HEADER="<event2/thread.h>"
    WW_EVENT_LIB=-levent
    AS_IF([test "$ww_event_dir" != ""],
        [WW_EVENT_CPPFLAGS="-I$ww_event_dir/include"])
    AS_IF([test "$ww_event_libdir" != ""],
        [WW_EVENT_LDFLAGS="-L$ww_event_libdir"])

    WW_VAR_SCOPE_POP
])dnl
