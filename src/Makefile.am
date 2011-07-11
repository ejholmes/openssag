lib_LTLIBRARIES = libopenssag.la
noinst_PROGRAMS = openssag
libopenssag_la_CPPFLAGS = $(LIBUSB_CFLAGS)
libopenssag_la_LIBADD = $(LIBUSB_LIBS)
libopenssag_la_SOURCES = openssag.cpp loader.cpp
libopenssag_la_LDFLAGS = -no-undefined -version-info 0:0:0
noinst_HEADERS = firmware.h openssag_priv.h
include_HEADERS = openssag.h

openssag_CPPFLAGS = $(LIBUSB_CFLAGS) $(MAGICKCORE_CFLAGS)
openssag_LDADD = libopenssag.la $(LIBUSB_LIBS) $(MAGICKCORE_LIBS)
openssag_SOURCES = commandline/ssag.cpp
