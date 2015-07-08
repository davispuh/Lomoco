#
# lomoco autoconf
#
# Process this file with autoconf to produce a configure script.

# lomoco version
m4_define([lomoco_VERSION], m4_esyscmd_s([sed -n -e '1s/lomoco .\([0-9]*.[0-9]*\)./\1/p' ChangeLog]))

AC_INIT([lomoco], [lomoco_VERSION])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/lomoco.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(usb,usb_init,,AC_MSG_ERROR(libusb not found.))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h])
AC_CHECK_HEADER(argp.h,,AC_MSG_ERROR(argp.h not found))
AC_CHECK_HEADER(usb.h,,AC_MSG_ERROR(usb.h not found))

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([strdup strspn])
AC_CHECK_FUNCS(usb_control_msg)

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
