OpenSSAG
============
This is an open source driver for the [Orion StarShoot
Autoguider](http://www.telescope.com/Astrophotography/Astrophotography-Cameras/Orion-StarShoot-AutoGuider/pc/-1/c/4/sc/58/p/52064.uts) for use on Linux and Mac OS X. The library also comes with a command line tool called openssag, that you can use as a reference for implementing OpenSSAG in your application.

Compiling From Source
---------------------
Download the most recent source package from the [downloads section](https://github.com/CortexAstronomy/OpenSSAG/downloads) and extract the contents.

**Linux**

```
$ ./configure --prefix=/usr
$ make
$ make install
```

**Mac OS X**  
Requires Developer Tools/Xcode. You made need to specify the location of libusb, like so:

```
$ ./configure --prefix=/usr LIBUSB_CFLAGS="<location of usb.h>" LIBUSB_LIBS"<location of libusb.a>"
$ make
$ make install
```

Or, if you prefer Xcode:

```
$ open OpenSSAG.xcodeproj
```

Usage
-----
The following shows a simple example of how to use the library in a C++ application.  

```
#include <stdio.h>

#include "openssag.h"

int main()
{
    OpenSSAG::SSAG *camera = new OpenSSAG::SSAG();
    
    if (camera->Connect()) {
        struct raw_image *image = camera->Expose(1000);
        FILE *fp = fopen("image", "w");
        fwrite(image->data, 1, image->width * image->height, fp);
        fclose(fp);
        camera->Disconnect();
    }
    else {
        printf("Could not find StarShoot Autoguider\n");
    }
}
```

Compile on Linux/Mac OS X

```
$ g++ main.cpp -lusb -lopenssag
$ ./a.out
$ convert -size 1280x1024 -depth 8 gray:image image.jpg
```
