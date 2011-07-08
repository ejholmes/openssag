lib_LTLIBRARIES = libopenssag.la
bin_PROGRAMS = openssag
libopenssag_la_CPPFLAGS = $(LIBUSB_CFLAGS)
libopenssag_la_LIBADD = $(LIBUSB_LIBS)
libopenssag_la_SOURCES = openssag.cpp loader.cpp
libopenssag_la_LDFLAGS = -no-undefined -version-info 0:0:0
include_HEADERS = openssag.h firmware.h util.h

openssag_CPPFLAGS = $(LIBUSB_CFLAGS) $(IMAGEMAGICK_CFLAGS)
openssag_LDADD = libopenssag.la $(LIBUSB_LIBS) $(IMAGEMAGICK_LIBS)
openssag_SOURCES = commandline/ssag.cpp
