#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([OpenSSAG], [0.1.2], [eric@ejholmes.net])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PREFIX_DEFAULT([/usr])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

AC_ARG_WITH(magick,
            AS_HELP_STRING([--with-magick], [Convert resulting image from raw values to image format with MagickCore]),
            [if test x$with_magick == xyes; then
                if test ! -z $PKG_CONFIG; then
                    PKG_CHECK_MODULES([MAGICKCORE], [MagickCore], AC_CHECK_LIB([MagickCore], [ConstituteImage],,),)
                fi
             fi])

AC_ARG_ENABLE(debug,
              AS_HELP_STRING([--enable-debug], [Enable debugging (disabled by default)]),
              [if test x$enableval = xyes; then
                   AC_DEFINE(DEBUG, 1, [Define this to enable debug messages and assert warnings.]) debug_support=yes
               fi])

# Check for libusb
AC_PATH_PROG([LIBUSB_CONFIG], [libusb-config], [], [$PATH$PATH_SEPARATOR/usr/local/Crosspack-AVR/bin])

if test ! -z $PKG_CONFIG; then
    PKG_CHECK_MODULES([LIBUSB], [libusb],,[PKG_CHECK_MODULES([LIBUSB], [libusb-legacy],,)])
else
    if test ! -z $LIBUSB_CONFIG; then
        LIBUSB_CFLAGS=`$LIBUSB_CONFIG --cflags`
        LIBUSB_LIBS=`$LIBUSB_CONFIG --libs`
        AC_SUBST([LIBUSB_CFLAGS])
        AC_SUBST([LIBUSB_LIBS])
    else
        AC_MSG_WARN([
* No suitable version of libusb-0.1 was found.
* If you know where libusb is located, specify it
* with the LIBUSB_CFLAGS and LIBUSB_LIBS variables.
*
* If you do not have libusb installed, you can
* download it from http://www.libusb.org/])
    fi
fi

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
