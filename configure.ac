AC_PREREQ(2.59)
AC_INIT(warewulf, 4.0, warewulf-devel@lbl.gov)
AC_CONFIG_SRCDIR([.])


if test -z "$prefix" -o "$prefix" = "NONE" ; then
  prefix=${ac_default_prefix}
fi
AC_SUBST(PREFIX, $prefix)

AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([.])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([.])
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_CC
AM_INIT_AUTOMAKE
AM_PROG_CC_C_O

AC_PATH_PROG(PERLBIN, perl)
if test -z "$PERLBIN"; then
   AC_MSG_ERROR([perl not found])
fi
AC_SUBST(PERLBIN)

AC_MSG_CHECKING(for Perl vendor lib path)
eval `perl -V:installvendorlib`
PERL_VENDORLIB=$installvendorlib
AC_ARG_WITH(perllibdir, [  --with-perllibdir=path  Manually specify where the perl modules should go], [
if test -n "$withval" -a "x$withval" != "xno" ; then
   PERL_VENDORLIB=$withval
fi
])
AC_MSG_RESULT($PERL_VENDORLIB)
AC_SUBST(PERL_VENDORLIB)


AC_OUTPUT(Makefile
    warewulf.spec
    bin/Makefile
    etc/Makefile
    lib/Makefile
    lib/Warewulf/ACVars.pm
    lib/Warewulf/Makefile
)

AC_OUTPUT
