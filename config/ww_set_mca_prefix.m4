# -*- shell-script -*-
#
# Copyright (c) 2014      Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2014-2016 Intel, Inc. All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

# WW_SET_MCA_PREFIX([mca_prefix]
#
# This macro sets a prefix for the MCA parameter system.  Specifically,
# OMPI_MCA_<foo> becomes <mca_prefix>_<foo>
#
# --------------------------------------------------------
AC_DEFUN([WW_SET_MCA_PREFIX],[
    AS_IF([test "$ww_mca_prefix_set" = "yes"],
          [AC_MSG_WARN([Warewulf mca prefix was already set!])
           AC_MSG_WARN([This is a configury programming error])
           AC_MSG_ERROR([Cannot continue])])

    MCA_PREFIX=$1
    ww_mca_prefix_set=yes
    AC_DEFINE_UNQUOTED([WW_MCA_PREFIX], ["$MCA_PREFIX"], [MCA prefix string for envars])
])dnl

#
# Set the MCA cmd line identifier - i.e., change "-mca" to "-<foo>"
#
AC_DEFUN([WW_SET_MCA_CMD_LINE_ID],[
    AS_IF([test "$ww_mca_cmd_id_set" = "yes"],
          [AC_MSG_WARN([Warewulf mca cmd line id was already set!])
           AC_MSG_WARN([This is a configury programming error])
           AC_MSG_ERROR([Cannot continue])])

    MCA_CMD_LINE_ID=$1
    ww_mca_cmd_id_set=yes
    AC_DEFINE_UNQUOTED([WW_MCA_CMD_LINE_ID], ["$MCA_CMD_LINE_ID"], [MCA cmd line identifier])
])dnl
