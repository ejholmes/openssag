#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "openssag.h"

using namespace OpenSSAG;

void usage()
{
    printf("Usage: ssag [OPTION]...\n");
    printf("Capture images from an Orion StarShoot Autoguider.\n\n");
    
    printf("  -c, --capture [DURATION]             Capture an image from the camera. DURATION is the exposure time in ms.\n");
    printf("  -g, --gain [1-15]                    Set the gain to be used for the capture. Only accepts values between 1 and 15\n");
    printf("  -b, --boot                           Load the firmware onto the camera.\n");
}

int main(int argc, char **argv)
{
    SSAG *camera = new SSAG();
    int c;
    opterr = 0;
    while (1) {
        static struct option long_options[] = {
            {"help",        no_argument,       0, 'h'}, /* Help */
            {"boot",        no_argument,       0, 'b'}, /* Load firmware */
            {"capture",     required_argument, 0, 'c'}, /* Capture an image from the camera */
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        c = getopt_long(argc, argv, "hbc:", long_options, &option_index);

        if (c == 'h' || c == -1) {
            usage();
            goto done;
        }
        if (c == 'b') {
            Loader *base = new Loader();
            if (!base->Connect()) {
                fprintf(stderr, "Device not found or the device already has firmware loaded\n");
            } else {
                base->LoadFirmware();
                base->Disconnect();
            }
            goto done;
        }
        if (c == 'c') {
            if (!camera->Connect()) {
                fprintf(stderr, "Device not found or could not connect\n");
                goto done;
            }
        }
        if (c == 'g') {
            int gain = atoi(optarg);
            if (gain < 1 || gain > 15) {
                fprintf(stderr, "Ignoring invalid gain setting.\n");
                continue;
            }
            camera->SetGain(gain);
        }
        if (c == 'c') {
            int duration = 1000;
            struct raw_image *image = camera->Expose(duration);
            goto done;
        }
    }
done:
    if (camera)
        camera->Disconnect();
    return 0;
}
