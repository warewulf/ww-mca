dnl -*- shell-script -*-
dnl
dnl Copyright (c) 2016      Los Alamos National Security, LLC. All rights
dnl                         reserved.
dnl Copyright (c) 2016      Cisco Systems, Inc.  All rights reserved.
dnl Copyright (c) 2016      Research Organization for Information Science
dnl                         and Technology (RIST). All rights reserved.
dnl Copyright (c) 2016      Intel, Inc. All rights reserved.
dnl $COPYRIGHT$
dnl
dnl Additional copyrights may follow
dnl
dnl $HEADER$
dnl
AC_DEFUN([WW_SUMMARY_ADD],[
    WW_VAR_SCOPE_PUSH([ww_summary_section ww_summary_line ww_summary_section_current])

    dnl need to replace spaces in the section name with something else. _ seems like a reasonable
    dnl choice. if this changes remember to change WW_PRINT_SUMMARY as well.
    ww_summary_section=$(echo $1 | tr ' ' '_')
    ww_summary_line="$2: $4"
    ww_summary_section_current=$(eval echo \$ww_summary_values_$ww_summary_section)

    if test -z "$ww_summary_section_current" ; then
        if test -z "$ww_summary_sections" ; then
            ww_summary_sections=$ww_summary_section
        else
            ww_summary_sections="$ww_summary_sections $ww_summary_section"
        fi
        eval ww_summary_values_$ww_summary_section=\"$ww_summary_line\"
    else
        eval ww_summary_values_$ww_summary_section=\"$ww_summary_section_current,$ww_summary_line\"
    fi

    WW_VAR_SCOPE_POP
])

AC_DEFUN([WW_SUMMARY_PRINT],[
    WW_VAR_SCOPE_PUSH([ww_summary_section ww_summary_section_name])
    cat <<EOF

Warewulf configuration:
-----------------------
Version: $WW_MAJOR_VERSION.$WW_MINOR_VERSION.$WW_RELEASE_VERSION$WW_GREEK_VERSION
EOF

    if test $WANT_DEBUG = 0 ; then
        echo "Debug build: no"
    else
        echo "Debug build: yes"
    fi

    if test ! -z $with_platform ; then
        echo "Platform file: $with_platform"
    else
        echo "Platform file: (none)"
    fi

    echo

    for ww_summary_section in $(echo $ww_summary_sections) ; do
        ww_summary_section_name=$(echo $ww_summary_section | tr '_' ' ')
        echo "$ww_summary_section_name"
        echo "-----------------------"
        echo "$(eval echo \$ww_summary_values_$ww_summary_section)" | tr ',' $'\n' | sort -f
        echo " "
    done

    if test $WANT_DEBUG = 1 ; then
        cat <<EOF
*****************************************************************************
 THIS IS A DEBUG BUILD!  DO NOT USE THIS BUILD FOR PERFORMANCE MEASUREMENTS!
*****************************************************************************

EOF
    fi

    WW_VAR_SCOPE_POP
])
