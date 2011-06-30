#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>

/* Micron MT9M001 datasheet:
 * http://download.micron.com/pdf/datasheets/imaging/mt9m001_1300_mono.pdf */

/* Exposure time in milliseconds */
#define EXPOSURE_TIME 1000
#define GAIN 0x3b

enum requests {
    request_guide = 16, /* Issues a guide command through the guider relays */
    request_start_exposure = 18, /* Starts an exposure sequence */
    request_set_magic_init_packet = 19, /* Set an 18 byte magic init packet */
    request_magic_init_2 = 20
};

enum cardinal_directions {
    cardinal_east = 0x10,
    cardinal_south = 0x20,
    cardinal_north = 0x40,
    cardinal_west = 0x80,
};

static int usb_open_device(usb_dev_handle **device, int vendorID, int productId, const char *serial);

/* Magic init functions. Not sure what this actually does yet. */
void send_init_sequence(usb_dev_handle *handle)
{
    unsigned char magic_init_packet[] = {
        /* Not so magic, 4 words repeating, controls gain. Why does it repeat? 
         * current value should be around 30% but it's not linear by the looks
         * of it so it's hard to really tell. Probably just need to graph it
         * out. */
        /* Update: see gain settings in MT9M001 datasheet. Why didn't they just
         * use register 0x35? Meh. */
        0x00, GAIN, 0x00, GAIN,
        0x00, GAIN, 0x00, GAIN,
        
        /* Controls enhanced noise reduction. Doubt this is really of any use,
         * current value turns it off */
        0x00, 0x0c, 0x00, 0x14,
        0x03, 0xff, 0x04, 0xff,

        0x04, 0x19 /* End ? */
    };

    usb_control_msg(handle, 0x40, request_set_magic_init_packet, 0x6ac8, 24, magic_init_packet, 18, 5000);
    usb_control_msg(handle, 0x40, request_magic_init_2, 0x3095, 0, NULL, 0, 5000);
}

void start_exposure(usb_dev_handle *handle)
{
    unsigned char response[2];

    usb_control_msg(handle, 0xc0, request_start_exposure, EXPOSURE_TIME, 0, response, sizeof(response), 5000);
}

void read_image(usb_dev_handle *handle)
{
    FILE *fp = fopen("data", "w");
    /* SSAG returns 1,600,000 total bytes of data */
    unsigned char image[1600000];
    /* SSAG dll splits up the 1,600,000 bytes into 97 * 16384 + 10752 */
    unsigned char data[16384];
    unsigned char end[512];
    
    int i;
    for (i = 0; i < 97; i++) {
        usb_bulk_read(handle, 2, data, sizeof(data), 5000);
        fwrite(data, 1, sizeof(data), fp);
    }
    usb_bulk_read(handle, 2, data, 10752, 5000);
    fwrite(data, 1, 10752, fp); /* 1,600,000 bytes by this point */
    usb_bulk_read(handle, 2, end, 512, 5000); /* TODO: find out why this is here */

    fclose(fp);
}

void write_image()
{
    char buffer[16000];
    FILE *data = fopen("data", "r");
    FILE *image = fopen("image", "w");

    /* fread(buffer, 1, 1527, data); */
    /* fwrite(&buffer[2], 1, 1280, image); [> First 2 bytes can be discarded <] */
    fseek(data, 3, SEEK_SET);

    int i;
    for (i = 0; i < 1024; i++) {
        fread(buffer, 1, 1524, data);
        fwrite(buffer, 1, 1280, image);
    }

    fclose(data);
    fclose(image);
}

/* Issues a guide command in direction for duration milliseconds */
void guide(usb_dev_handle *handle, enum cardinal_directions direction, int duration)
{
    char data[8];
    /* Another instance of duplicate data. Why do they do this? =\ */
    memcpy(data, &duration, 4);
    memcpy(data+4, &duration, 4);

    usb_control_msg(handle, 0x40, request_guide, 0, (int)direction, data, sizeof(data), 5000);
}

int main(int argc, const char *argv[])
{
    usb_dev_handle *handle = NULL;
    if (usb_open_device(&handle, 0x1856, 0x0012, NULL)) {
        send_init_sequence(handle);
        start_exposure(handle);
        read_image(handle);
        write_image();
        guide(handle, cardinal_north, 1000);
        // In terminal:
        // convert -size 1280x1024 -depth 8 gray:image image.jpg 
    }
    usb_close(handle);
    return 0;
}

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
