#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>

#include "util.h"
#include "openssag.h"

#define USB_RQ_GUIDE 16
#define USB_RQ_EXPOSE 18
#define USB_RQ_SET_INIT_PACKET 19
#define USB_RQ_PRE_EXPOSE 20

#define BUFFER_ENDPOINT 2
#define BULK_READ_LENGTH 16384

#define SENSOR_WIDTH 1280
#define SENSOR_HEIGHT 1024
#define SENSOR_ROW_LENGTH 1524


using namespace OpenSSAG;

bool SSAG::Connect()
{
    if (!usb_open_device(&this->handle, VENDOR_ID, PRODUCT_ID, NULL)) {
        return false;
    }
}

void SSAG::Disconnect()
{
    if (this->handle)
        usb_close(this->handle);
    handle = NULL;
}

raw_image *SSAG::Expose(int duration)
{
    raw_image *image = (raw_image *)malloc(sizeof(raw_image));
    this->InitSequence();
    usb_control_msg(this->handle, 0xc0, USB_RQ_EXPOSE, duration, 0, NULL, 2, 5000);

    image->width = SENSOR_WIDTH;
    image->height = SENSOR_HEIGHT;
    image->data = this->ReadBuffer();

    return image;
}

void SSAG::Guide(enum guide_direction direction, int duration)
{
    char data[8];
    memcpy(data, &duration, 4);
    memcpy(data+4, &duration, 4);

    usb_control_msg(this->handle, 0x40, USB_RQ_GUIDE, 0, (int)direction, data, sizeof(data), 5000);
}

void SSAG::InitSequence()
{
    char init_packet[18] = {
        // TODO
    };

    usb_control_msg(this->handle, 0x40, USB_RQ_SET_INIT_PACKET, 0x6ac8, 24, init_packet, sizeof(init_packet), 5000);
    usb_control_msg(this->handle, 0x40, USB_RQ_PRE_EXPOSE, 0x3095, 0, NULL, 0, 5000);
}

unsigned char *SSAG::ReadBuffer()
{
    /* SSAG returns 1,600,000 total bytes of data */
    char *data = (char *)malloc(1600000);
    char end[512];
    char *dptr, *iptr;
    
    dptr = data;
    for (int i = 0; i < 97; i++) {
        usb_bulk_read(this->handle, BUFFER_ENDPOINT, dptr, BULK_READ_LENGTH, 5000);
        dptr += BULK_READ_LENGTH * i;
    }
    usb_bulk_read(this->handle, BUFFER_ENDPOINT, dptr, 10752, 5000);
    usb_bulk_read(this->handle, BUFFER_ENDPOINT, end, 512, 5000);
    data += 3; /* First 3 bytes can be ignored */

    char *image = (char *)malloc(1310720);

    dptr = data;
    iptr = image;
    for (int i = 0; i < SENSOR_HEIGHT; i++) {
        memcpy(iptr, dptr, SENSOR_WIDTH);
        dptr += SENSOR_ROW_LENGTH; /* Bytes between SENSOR_WIDTH and SENSOR_ROW_LENGTH can be ignored */
        iptr += SENSOR_WIDTH;
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
