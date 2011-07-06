#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>

#include "util.h"
#include "openssag.h"

enum USB_REQUEST {
    USB_RQ_GUIDE = 16, /* 0x10 */
    USB_RQ_EXPOSE = 18, /* 0x12 */
    USB_RQ_SET_INIT_PACKET = 19, /* 0x13 */
    USB_RQ_PRE_EXPOSE = 20, /* 0x14 */
    USB_RQ_CANCEL_GUIDE = 24, /* 0x18 */
    USB_RQ_CANCEL_GUIDE_NORTH_SOUTH = 34, /* 0x22 */
    USB_RQ_CANCEL_GUIDE_EAST_WEST = 33 /* 0x21 */
};


#define BUFFER_ENDPOINT 2
#define BULK_READ_LENGTH 16384

#define BUFFER_SIZE 1600200
#define BUFFER_ROW_LENGTH 1524

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 1024

using namespace OpenSSAG;

SSAG::SSAG()
{
    this->gain = 0x3b;
}
bool SSAG::Connect(bool bootload)
{
    if (!usb_open_device(&this->handle, VENDOR_ID, PRODUCT_ID, NULL)) {
        if (bootload) {
            Loader *loader = new Loader();
            if (loader->Connect()) {
                loader->LoadFirmware();
                loader->Disconnect();
                sleep(2);
                return this->Connect(false);
            } else {
                return false;
            }
        }else {
            return false;
        }
    }

    return true;
}

bool SSAG::Connect()
{
    this->Connect(false);
}

void SSAG::Disconnect()
{
    if (this->handle)
        usb_close(this->handle);
    this->handle = NULL;
}

struct raw_image *SSAG::Expose(int duration)
{
    this->InitSequence();
    usb_control_msg(this->handle, 0xc0, USB_RQ_EXPOSE, duration, 0, NULL, 2, 5000);

    struct raw_image *image = (raw_image *)malloc(sizeof(raw_image));
    image->width = IMAGE_WIDTH;
    image->height = IMAGE_HEIGHT;
    image->data = this->ReadBuffer();

    return image;
}

void SSAG::CancelExposure()
{
    // Send 0x00 over EP 0 ?
}

void SSAG::Guide(enum guide_direction direction, int duration)
{
    this->Guide(direction, duration, duration);
}

void SSAG::Guide(enum guide_direction direction, int yduration, int xduration)
{
    char data[8];

    memcpy(data    , &xduration, 4);
    memcpy(data + 4, &yduration, 4);

    usb_control_msg(this->handle, 0x40, USB_RQ_GUIDE, 0, (int)direction, data, sizeof(data), 5000);
}

void SSAG::InitSequence()
{
    char init_packet[18] = {
        /* Gain settings */
        0x00, this->gain, /* G1 Gain */
        0x00, this->gain, /* B  Gain */
        0x00, this->gain, /* R  Gain */
        0x00, this->gain, /* G2 Gain */

        /* Vertical Offset */
        0x00, 0x0c, /* Offset by 12 pixels */

        /* It would make sense for this to be the horizontal offset, but i'm
         * not sure. */
        0x00, 0x14,

        /* Image height - 1 */
        (IMAGE_HEIGHT - 1) >> 8, (IMAGE_HEIGHT - 1) & 0xff,

        /* Image width - 1 */
        (IMAGE_WIDTH - 1) >> 8, (IMAGE_WIDTH - 1) & 0xff,

        /* End? */
        0x04, 0x19 /* 1049 */
    };

    int wValue = BUFFER_SIZE & 0xffff;
    int wIndex = BUFFER_SIZE  >> 16;

    usb_control_msg(this->handle, 0x40, USB_RQ_SET_INIT_PACKET, wValue, wIndex, init_packet, sizeof(init_packet), 5000);
    usb_control_msg(this->handle, 0x40, USB_RQ_PRE_EXPOSE, 0x3095, 0, NULL, 0, 5000);
}

unsigned char *SSAG::ReadBuffer()
{
    /* SSAG returns 1,600,200 total bytes of data */
    char *data = (char *)malloc(BUFFER_SIZE);
    char *dptr, *iptr;
    
    dptr = data;
    for (int i = 0; i < 97; i++) {
        usb_bulk_read(this->handle, BUFFER_ENDPOINT, dptr, BULK_READ_LENGTH, 5000);
        dptr += BULK_READ_LENGTH;
    }
    usb_bulk_read(this->handle, BUFFER_ENDPOINT, dptr, 10752, 5000);
    dptr += 10752;
    usb_bulk_read(this->handle, BUFFER_ENDPOINT, dptr, 512, 5000);

    char *image = (char *)malloc(IMAGE_WIDTH * IMAGE_HEIGHT);

    dptr = data+3; /* First 3 bytes can be ignored */
    iptr = image;
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        memcpy(iptr, dptr, IMAGE_WIDTH);
        dptr += BUFFER_ROW_LENGTH; /* Bytes between IMAGE_WIDTH and BUFFER_ROW_LENGTH can be ignored */
        iptr += IMAGE_WIDTH;
    }

    free(data);

    return (unsigned char *)image;
}

void SSAG::SetGain(int gain)
{
    // TODO: Set gain based on MT9M001 datasheet
}

void SSAG::FreeRawImage(raw_image *image)
{
    free(image->data);
    free(image);
}
