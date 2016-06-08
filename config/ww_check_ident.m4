dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2007      Sun Microsystems, Inc.  All rights reserved.
dnl Copyright (c) 2015      Intel, Inc. All rights reserved
dnl $COPYRIGHT$
dnl
dnl Additional copyrights may follow
dnl
dnl $HEADER$
dnl
dnl defines:
dnl   WW_$1_USE_PRAGMA_IDENT
dnl   WW_$1_USE_IDENT
dnl   WW_$1_USE_CONST_CHAR_IDENT
dnl

# WW_CHECK_IDENT(compiler-env, compiler-flags,
# file-suffix, lang) Try to compile a source file containing
# a #pragma ident, and determine whether the ident was
# inserted into the resulting object file
# -----------------------------------------------------------
AC_DEFUN([WW_CHECK_IDENT], [
    AC_MSG_CHECKING([for $4 ident string support])

    ww_pragma_ident_happy=0
    ww_ident_happy=0
    ww_static_const_char_happy=0
    _WW_CHECK_IDENT(
        [$1], [$2], [$3],
        [[#]pragma ident], [],
        [ww_pragma_ident_happy=1
         ww_message="[#]pragma ident"],
        _WW_CHECK_IDENT(
            [$1], [$2], [$3],
            [[#]ident], [],
            [ww_ident_happy=1
             ww_message="[#]ident"],
            _WW_CHECK_IDENT(
                [$1], [$2], [$3],
                [[#]pragma comment(exestr, ], [)],
                [ww_pragma_comment_happy=1
                 ww_message="[#]pragma comment"],
                [ww_static_const_char_happy=1
                 ww_message="static const char[[]]"])))

    AC_DEFINE_UNQUOTED([WW_$1_USE_PRAGMA_IDENT],
        [$ww_pragma_ident_happy], [Use #pragma ident strings for $4 files])
    AC_DEFINE_UNQUOTED([WW_$1_USE_IDENT],
        [$ww_ident_happy], [Use #ident strings for $4 files])
    AC_DEFINE_UNQUOTED([WW_$1_USE_PRAGMA_COMMENT],
        [$ww_pragma_comment_happy], [Use #pragma comment for $4 files])
    AC_DEFINE_UNQUOTED([WW_$1_USE_CONST_CHAR_IDENT],
        [$ww_static_const_char_happy], [Use static const char[] strings for $4 files])

    AC_MSG_RESULT([$ww_message])

    unset ww_pragma_ident_happy ww_ident_happy ww_static_const_char_happy ww_message
])

# _WW_CHECK_IDENT(compiler-env, compiler-flags,
# file-suffix, header_prefix, header_suffix, action-if-success, action-if-fail)
# Try to compile a source file containing a #-style ident,
# and determine whether the ident was inserted into the
# resulting object file
# -----------------------------------------------------------
AC_DEFUN([_WW_CHECK_IDENT], [
    eval ww_compiler="\$$1"
    eval ww_flags="\$$2"

    ww_ident="string_not_coincidentally_inserted_by_the_compiler"
    cat > conftest.$3 <<EOF
$4 "$ww_ident" $5
int main(int argc, char** argv);
int main(int argc, char** argv) { return 0; }
EOF

    # "strings" won't always return the ident string.  objdump isn't
    # universal (e.g., OS X doesn't have it), and ...other
    # complications.  So just try to "grep" for the string in the
    # resulting object file.  If the ident is found in "strings" or
    # the grep succeeds, rule that we have this flavor of ident.

    echo "configure:__oline__: $1" >&5
    ww_output=`$ww_compiler $ww_flags -c conftest.$3 -o conftest.${OBJEXT} 2>&1 1>/dev/null`
    ww_status=$?
    AS_IF([test $ww_status = 0],
          [test -z "$ww_output"
           ww_status=$?])
    WW_LOG_MSG([\$? = $ww_status], 1)
    AS_IF([test $ww_status = 0 && test -f conftest.${OBJEXT}],
          [ww_output="`strings -a conftest.${OBJEXT} | grep $ww_ident`"
           grep $ww_ident conftest.${OBJEXT} 2>&1 1>/dev/null
           ww_status=$?
           AS_IF([test "$ww_output" != "" || test "$ww_status" = "0"],
                 [$6],
                 [$7])],
          [WW_LOG_MSG([the failed program was:])
           WW_LOG_FILE([conftest.$3])
           $7])

    unset ww_compiler ww_flags ww_output ww_status
    rm -rf conftest.* conftest${EXEEXT}
])dnl
