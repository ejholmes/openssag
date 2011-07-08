#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([OpenSSAG], [0.1], [eric@ejholmes.net])
AC_CONFIG_SRCDIR([util.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([usb], [usb_control_msg])

# Check for libusb
PKG_CHECK_MODULES([LIBUSB], [libusb],,[
                   AC_CHECK_LIB([usb], [usb_init],,[AC_MSG_ERROR[libusb not found]])
                   AC_CHECK_HEADERS([usb.h],,[AC_MSG_ERROR[usb.h not found]])
                   ])

PKG_CHECK_MODULES([IMAGEMAGICK], [MagickCore])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
