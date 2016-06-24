dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2010-2015 Cisco Systems, Inc.  All rights reserved.
dnl Copyright (c) 2016      Intel, Inc. All right reserved
dnl $COPYRIGHT$
dnl
dnl Additional copyrights may follow
dnl
dnl $HEADER$
dnl

dnl There will only be one component used in this framework, and it will
dnl be selected at configure time by priority.  Components must set
dnl their priorities in their configure.m4 file.

dnl We only want one winning component (vs. STOP_AT_FIRST_PRIORITY,
dnl which will allow all components of the same priority who succeed to
dnl win)
m4_define(MCA_ww_dl_CONFIGURE_MODE, STOP_AT_FIRST)

AC_DEFUN([MCA_ww_dl_CONFIG],[
    WW_HAVE_DL_SUPPORT=0

    # If --disable-dlopen was used, then have all the components fail
    # (we still need to configure them all so that things like "make
    # dist" work", but we just want the MCA system to (artificially)
    # conclude that it can't build any of the components.
    AS_IF([test "$enable_dlopen" = "no"],
          [want_dl=0], [want_dl=1])

    MCA_CONFIGURE_FRAMEWORK([dl], [$want_dl])

    # If we found no suitable static dl component and dlopen support
    # was not specifically disabled, this is an error.
    AS_IF([test "$MCA_ww_dl_STATIC_COMPONENTS" = "" && \
           test "$enable_dlopen" != "no"],
          [AC_MSG_WARN([Did not find a suitable static ww dl component])
           AC_MSG_WARN([You might need to install libltld (and its headers) or])
           AC_MSG_WARN([specify --disable-dlopen to configure.])
           AC_MSG_ERROR([Cannot continue])])

    # If we have a winning component (which, per above, will only
    # happen if --disable-dlopen was *not* specified), do some more
    # logic.
    AS_IF([test "$MCA_ww_dl_STATIC_COMPONENTS" != ""],
       [ # We had a winner -- w00t!

        WW_HAVE_DL_SUPPORT=1
        # If we added any -L flags to ADD_LDFLAGS, then we (might)
        # need to add those directories to LD_LIBRARY_PATH.
        # Otherwise, if we try to AC RUN_IFELSE anything here in
        # configure, it might die because it can't find the libraries
        # we just linked against.
        WW_VAR_SCOPE_PUSH([ww_dl_base_found_l ww_dl_base_token ww_dl_base_tmp ww_dl_base_dir])
        ww_dl_base_found_l=0
        eval "ww_dl_base_tmp=\$ww_dl_${ww_dl_winner}_ADD_LIBS"
        for ww_dl_base_token in $ww_dl_base_tmp; do
            case $ww_dl_base_token in
            -l*) ww_dl_base_found_l=1 ;;
            esac
        done
        AS_IF([test $ww_dl_base_found_l -eq 1],
              [eval "ww_dl_base_tmp=\$ww_dl_${ww_dl_winner}_ADD_LDFLAGS"
               for ww_dl_base_token in $ww_dl_base_tmp; do
                   case $ww_dl_base_token in
                   -L*)
                       ww_dl_base_dir=`echo $ww_dl_base_token | cut -c3-`
                       export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ww_dl_base_dir
                       AC_MSG_WARN([Adding to LD_LIBRARY_PATH: $ww_dl_base_dir])
                       ;;
                   esac
               done])
        WW_VAR_SCOPE_POP
    ])

    AC_DEFINE_UNQUOTED([WW_HAVE_DL_SUPPORT], [$WW_HAVE_DL_SUPPORT],
                       [Whether the WW DL framework is functional or not])
])
