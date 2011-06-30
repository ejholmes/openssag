#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>

/* Exposure time in milliseconds */
#define EXPOSURE_TIME 5000

enum requests {
    request_start_exposure = 18,
    request_init_1 = 19,
    request_init_2 = 20
};

/* Opens a usb_dev_handle based on the vendor id and product id */
static int usb_open_device(usb_dev_handle **device, int vendorID, int productId, const char *serial)
{
    struct usb_bus *bus;
    struct usb_device *dev;
    struct usb_dev_handle *handle = NULL;

    usb_init();

    usb_find_busses();
    usb_find_devices();

    for (bus = usb_get_busses(); bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (dev->descriptor.idVendor == vendorID &&
                    dev->descriptor.idProduct == productId) {
                handle = usb_open(dev);
                if (handle) {
                    if (serial == NULL) {
                        goto havedevice;
                    }
                    else {
                        char devserial[256];
                        int len = 0;
                        if (dev->descriptor.iSerialNumber > 0) {
                            len = usb_get_string_simple(handle, dev->descriptor.iSerialNumber, devserial, sizeof(devserial));
                        }
                        if ((len > 0) && (strcmp(devserial, serial) == 0)) {
                            goto havedevice;
                        }
                    }
                }
            }
        }
    }
    return 0;
havedevice:
    usb_set_configuration(handle, 1);
    usb_claim_interface(handle, 0);
#ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
    if (usb_detach_kernel_driver_np(handle, 0) < 0)
        fprintf(stderr, "Warning: Could not detach kernel driver: %s\nYou may need to run this as root or add yourself to the usb group\n", usb_strerror());
#endif
    *device = handle;
    return 1;
}

/* Magic init functions. Not sure what this actually does yet. */
void send_init_sequence(usb_dev_handle *handle)
{
    unsigned char init_1_data[] = {
        0x00, 0x3b, 0x00, 0x3b,
        0x00, 0x3b, 0x00, 0x3b,
        0x00, 0x0c, 0x00, 0x14,
        0x03, 0xff, 0x04, 0xff,
        0x04, 0x19
    };

    usb_control_msg(handle, 0x40, request_init_1, 0x6ac8, 24, init_1_data, 18, 5000);
    usb_control_msg(handle, 0x40, request_init_1, 0x6ac8, 24, init_1_data, 18, 5000);
    usb_control_msg(handle, 0x40, request_init_2, 0x3095, 0, NULL, 0, 5000);
}

void start_exposure(usb_dev_handle *handle)
{
    unsigned char response[2];

    usb_control_msg(handle, 0xc0, request_start_exposure, EXPOSURE_TIME, 0, response, sizeof(response), 5000);
}

void read_image(usb_dev_handle *handle)
{
    unsigned char image[1600000]; /* SSAG returns 1,600,000 total bytes of data */
    unsigned char data[16384];
    unsigned char end[512];
    
    int i;
    for (i = 0; i < 97; i++) {
        usb_bulk_read(handle, 2, data, sizeof(data), 5000);
    }
    usb_bulk_read(handle, 2, data, 10752, 5000);
    usb_bulk_read(handle, 2, end, 512, 5000);
}

int main(int argc, const char *argv[])
{
    usb_dev_handle *handle = NULL;
    if (usb_open_device(&handle, 0x1856, 0x0012, NULL)) {
        send_init_sequence(handle);
        start_exposure(handle);
        read_image(handle);
    }
    usb_close(handle);
    return 0;
}
