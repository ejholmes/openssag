#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "openssag.h"

using namespace OpenSSAG;

int main(int argc, const char *argv[])
{
    SSAG *camera = new SSAG();
    if (camera->Connect()) {
        struct raw_image *image = NULL;

        image = camera->Expose(1000);
        printf("Width: %d, Height: %d\n", image->width, image->height);

        FILE *fp = fopen("image", "w");
        fwrite(image->data, 1, image->width * image->height, fp);
        fclose(fp);
        // In terminal:
        // convert -size 1280x1024 -depth 8 gray:image image.jpg 

        camera->Disconnect();
    } else {
        fprintf(stderr, "Camera not found\n");
    }
    return 0;
}
