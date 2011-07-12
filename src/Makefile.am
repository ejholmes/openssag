lib_LTLIBRARIES = libopenssag.la

libopenssag_la_SOURCES = openssag.cpp loader.cpp
libopenssag_la_LDFLAGS = -no-undefined -version-info 0:0:0
noinst_HEADERS = firmware.h openssag_priv.h
include_HEADERS = openssag.h

if ENABLE_TOOL
noinst_PROGRAMS = openssag
openssag_CPPFLAGS = $(LIBUSB_CFLAGS) $(MAGICKCORE_CFLAGS)
openssag_LDADD = libopenssag.la $(LIBUSB_LIBS) $(MAGICKCORE_LIBS)
openssag_SOURCES = commandline/ssag.cpp
endif
