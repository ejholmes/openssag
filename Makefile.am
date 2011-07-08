lib_LTLIBRARIES = libopenssag.la
bin_PROGRAMS = ssag
libopenssag_la_CPPFLAGS = $(LIBUSB_CFLAGS)
libopenssag_la_LIBADD = $(LIBUSB_LIBS)
libopenssag_la_SOURCES = openssag.cpp loader.cpp
libopenssag_la_LDFLAGS = -no-undefined -version-info 0:0:0
include_HEADERS = openssag.h firmware.h util.h

ssag_CPPFLAGS = $(LIBUSB_CFLAGS)
ssag_LDADD = libopenssag.la $(LIBUSB_LIBS)
ssag_SOURCES = commandline/ssag.cpp
