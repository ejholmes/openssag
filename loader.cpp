#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>

#include "util.h"
#include "openssag.h"
#include "firmware.h"

#define CPUCS_ADDRESS 0xe600

#define CHUNK_SIZE 0x10

enum USB_REQUEST {
    USB_RQ_LOAD_FIRMWARE = 0xa0,
};

using namespace OpenSSAG;

static unsigned char bootloader[] = { SSAG_BOOTLOADER };
static unsigned char firmware[] = { SSAG_FIRMWARE };

bool Loader::Connect()
{
    if (!usb_open_device(&this->handle, LOADER_VENDOR_ID, LOADER_PRODUCT_ID, NULL)) {
        return false;
    }
    return true;
}

void Loader::Disconnect()
{
    if (this->handle)
        usb_close(this->handle);
    this->handle = NULL;
}

void Loader::EnterResetMode()
{
    char data = 0x01;
    usb_control_msg(this->handle, 0x40, USB_RQ_LOAD_FIRMWARE, 0x7f92, 0, &data, 1, 5000);
    usb_control_msg(this->handle, 0x40, USB_RQ_LOAD_FIRMWARE, CPUCS_ADDRESS, 0, &data, 1, 5000);
}

void Loader::ExitResetMode()
{
    char data = 0x00;
    usb_control_msg(this->handle, 0x40, USB_RQ_LOAD_FIRMWARE, 0x7f92, 0, &data, 1, 5000);
    usb_control_msg(this->handle, 0x40, USB_RQ_LOAD_FIRMWARE, CPUCS_ADDRESS, 0, &data, 1, 5000);
}

void Loader::LoadFirmware()
{
    unsigned char *data = NULL;

    /* Load bootloader */
    this->EnterResetMode();
    this->EnterResetMode();
    printf("Loading bootloader...");
    data = bootloader;
    for (;;) {
        unsigned char byte_count = *data;
        if (byte_count == 0)
            break;
        unsigned short address = *(unsigned int *)(data+1);
        usb_control_msg(this->handle, 0x40, USB_RQ_LOAD_FIRMWARE, address, 0, (char *)(data+3), byte_count, 5000);
        data += byte_count + 3;
    }
    printf("done\n");
    this->ExitResetMode(); /* Transfer execution to the reset vector */

    sleep(1);

    /* Load firmware */
    this->EnterResetMode();
    printf("Loading firmware...");
    data = firmware;
    for (;;) {
        unsigned char byte_count = *data;
        if (byte_count == 0)
            break;
        unsigned short address = *(unsigned int *)(data+1);
        usb_control_msg(this->handle, 0x40, USB_RQ_LOAD_FIRMWARE, address, 0, (char *)(data+3), byte_count, 5000);
        data += byte_count + 3;
    }
    printf("done\n");
    this->EnterResetMode(); /* Make sure the CPU is in reset */
    this->ExitResetMode(); /* Transfer execution to the reset vector */
}
