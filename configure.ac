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
PKG_PROG_PKG_CONFIG

AC_ARG_ENABLE(debug,
              [  --enable-debug          Enable debugging (disabled by default)],
              [if test x$enableval = xyes; then
                   AC_DEFINE(DEBUG, 1, [Define this to enable debug messages and assert warnings.]) debug_support=yes
               fi])

AC_PATH_PROG([PKG_CONFIG], pkg-config)
AC_PATH_PROG([LIBUSB_CONFIG], [libusb-config], [], [$PATH$PATH_SEPARATOR/usr/local/Crosspack-AVR/bin])

if test ! -z $PKG_CONFIG; then
    PKG_CHECK_MODULES([LIBUSB], [libusb],,)
    PKG_CHECK_MODULES([MAGICKCORE], [MagickCore], AC_CHECK_LIB([MagickCore], [ConstituteImage],,),)
else
    if test ! -z $LIBUSB_CONFIG; then
        LIBUSB_CFLAGS=`$LIBUSB_CONFIG --cflags`
        LIBUSB_LIBS=`$LIBUSB_CONFIG --libs`
        AC_SUBST([LIBUSB_CFLAGS])
        AC_SUBST([LIBUSB_LIBS])
    else
        AC_MSG_ERROR([
* No suitable version of libusb-0.1 was found.
* If you know where libusb is located, specify it
* with the LIBUSB_CFLAGS and LIBUSB_LIBS variables.
*
* If you do not have libusb installed, you can
* download it from http://www.libusb.org/])

        # AC_CHECK_LIB([libusb], [usb_init], [], AC_MSG_ERROR([
                      # * Error: no suitable version of libusb-0.1 was found.
                      # * If you know where libusb is located, specify it
                      # * with the LIBUSB_CFLAGS and LIBUSB_LIBS variables.
                      # *
                      # * If you do not have libusb installed, you can
                      # * download it from http://www.libusb.org/]))
    fi
fi

# AS_IF([test "$PKGCONFIG" -eq 1],
      # [PKG_CONFIG_MODULES[LIBUSB], [libusb]],
      # )

# if test "$PKGCONFIG" -eq 1; then
    # PKG_CHECK_MODULES([LIBUSB],[libusb],,)
    # PKG_CHECK_MODULES([MAGICKCORE],[MagickCore],AC_CHECK_LIB([MagickCore],[ConstituteImage],,),)
# if test "$LIBUSB_CONFIG" -eq 1; then
    # [LIBUSB_CFLAGS="`libusb-config --cflags`"]
    # AC_SUBST([LIBUSB_CFLAGS])
    # [LIBUSB_LIBS="`libusb-config --libs`"]
    # AC_SUBST([LIBUSB_LIBS])
# fi

# Check for libusb

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
