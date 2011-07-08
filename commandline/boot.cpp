#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "openssag.h"

using namespace OpenSSAG;

int main(int argc, const char *argv[])
{
    SSAG *camera = new SSAG();
    if (camera->Connect(true)) {
        camera->Disconnect();
    } else {
        fprintf(stderr, "Camera not found\n");
    }
    return 0;
}
