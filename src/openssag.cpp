#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>
#include <time.h>

#include "openssag.h"
#include "openssag_priv.h"

/* USB commands to control the camera */
enum USB_REQUEST {
    USB_RQ_GUIDE = 16, /* 0x10 */
    USB_RQ_EXPOSE = 18, /* 0x12 */
    USB_RQ_SET_INIT_PACKET = 19, /* 0x13 */
    USB_RQ_PRE_EXPOSE = 20, /* 0x14 */

    /* These aren't tested yet */
    USB_RQ_CANCEL_GUIDE = 24, /* 0x18 */
    USB_RQ_CANCEL_GUIDE_NORTH_SOUTH = 34, /* 0x22 */
    USB_RQ_CANCEL_GUIDE_EAST_WEST = 33 /* 0x21 */
};

/* USB Bulk endpoint to grab data from */
#define BUFFER_ENDPOINT 2

/* Amount of data returned from camera */
#define BUFFER_SIZE 1600200
#define BUFFER_ROW_LENGTH 1524

/* Image size */
#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 1024

/* Number of pixel columns/rows to skip */
#define ROW_START 12
#define COLUMN_START 20

/* Hell if I know */
#define SHUTTER_WIDTH 1049
#define PIXEL_OFFSET 12440

/* Number of seconds to wait for camera to renumerate after loading firmware */
#define RENUMERATE_TIMEOUT 10

using namespace OpenSSAG;

SSAG::SSAG()
{
    usb_init();
    this->SetGain(6);
}

struct device_info *SSAG::EnumerateDevices()
{
    struct usb_bus *bus;
    struct usb_device *dev;
    struct usb_dev_handle *handle = NULL;
    struct device_info *head = NULL, *last = NULL, *current = NULL;

    usb_find_busses();
    usb_find_devices();

    for (bus = usb_get_busses(); bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (dev->descriptor.idVendor == SSAG_VENDOR_ID &&
                    dev->descriptor.idProduct == SSAG_PRODUCT_ID) {
                handle = usb_open(dev);
                if (handle) {
                    current = (struct device_info *)malloc(sizeof(struct device_info *));
                    current->next = NULL;
                    /* Copy serial */
                    usb_get_string_simple(handle, dev->descriptor.iSerialNumber, current->serial, sizeof(current->serial));
                    if (!head)
                        head = current;
                    if (last)
                        last->next = current;

                    last = current;
                }
            }
        }
    }

    return head;
}

bool SSAG::Connect(bool bootload)
{
    if (!usb_open_device(&this->handle, SSAG_VENDOR_ID, SSAG_PRODUCT_ID, NULL)) {
        if (bootload) {
            Loader *loader = new Loader();
            if (loader->Connect()) {
                if (!loader->LoadFirmware()) {
                    fprintf(stderr, "ERROR:  Failed to upload firmware to the device\n");
                    return false;
                }
                loader->Disconnect();
                for (time_t t = time(NULL) + RENUMERATE_TIMEOUT; time(NULL) < t;) {
                    DBG("Checking if camera has renumerated...");
                    if (EnumerateDevices()) {
                        DBG("Yes\n");
                        return this->Connect(false);
                    }
                    DBG("No\n");
                    sleep(1);
                }
                DBG("ERROR:  Device did not renumerate. Timed out.\n");
                /* Timed out */
                return false;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}

bool SSAG::Connect()
{
    this->Connect(true);
}

void SSAG::Disconnect()
{
    if (this->handle)
        usb_close(this->handle);
    this->handle = NULL;
}

bool SSAG::IsConnected()
{
    return (this->handle != NULL);
}

struct raw_image *SSAG::Expose(int duration)
{
    this->InitSequence();
    char data[16];
    usb_control_msg(this->handle, 0xc0, USB_RQ_EXPOSE, duration, 0, data, 2, 5000);

    struct raw_image *image = (raw_image *)malloc(sizeof(struct raw_image));
    image->width = IMAGE_WIDTH;
    image->height = IMAGE_HEIGHT;
    image->data = this->ReadBuffer(duration + 5000);

    if (!image->data)
        return NULL;

    return image;
}

void SSAG::CancelExposure()
{
    /* Not tested */
    char data = 0;
    usb_bulk_read(this->handle, 0, (char *)&data, 1, 5000);
}

void SSAG::Guide(int direction, int duration)
{
    this->Guide(direction, duration, duration);
}

void SSAG::Guide(int direction, int yduration, int xduration)
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

        /* Vertical Offset. Reg0x01 */
        0x00, ROW_START,

        /* Horizontal Offset. Reg0x02 */
        0x00, COLUMN_START,

        /* Image height - 1. Reg0x03 */
        (IMAGE_HEIGHT - 1) >> 8, (IMAGE_HEIGHT - 1) & 0xff,

        /* Image width - 1. Reg0x04 */
        (IMAGE_WIDTH - 1) >> 8, (IMAGE_WIDTH - 1) & 0xff,

        /* Shutter Width. Reg0x09 */
        (SHUTTER_WIDTH) >> 8, (SHUTTER_WIDTH) & 0xff
    };

    int wValue = BUFFER_SIZE & 0xffff;
    int wIndex = BUFFER_SIZE  >> 16;

    usb_control_msg(this->handle, 0x40, USB_RQ_SET_INIT_PACKET, wValue, wIndex, init_packet, sizeof(init_packet), 5000);
    usb_control_msg(this->handle, 0x40, USB_RQ_PRE_EXPOSE, PIXEL_OFFSET, 0, NULL, 0, 5000);
}

unsigned char *SSAG::ReadBuffer(int timeout)
{
    /* SSAG returns 1,600,200 total bytes of data */
    char *data = (char *)malloc(BUFFER_SIZE);
    char *dptr, *iptr;
    
    usb_bulk_read(this->handle, BUFFER_ENDPOINT, data, BUFFER_SIZE, timeout);

    char *image = (char *)malloc(IMAGE_WIDTH * IMAGE_HEIGHT);

    dptr = data;
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
    /* See the MT9M001 datasheet for more information on the following code. */
    if (gain < 1 || gain > 15) {
        return;
    } else if (gain <= 4) {
        this->gain = gain * 8;
    } else if (gain <= 8) {
        this->gain = (gain * 4) + 0x40;
    } else if (gain <= 15) {
        this->gain = (gain - 8) + 0x60;
    }
}

void SSAG::FreeRawImage(raw_image *image)
{
    free(image->data);
    free(image);
}
