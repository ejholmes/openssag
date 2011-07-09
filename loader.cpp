#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>

#ifdef __WIN32__
#include "windows.h"
#define sleep(n) Sleep(1000 * n)
#endif

#include "util.h"
#include "openssag.h"
#include "firmware.h"

#define CPUCS_ADDRESS 0xe600

enum USB_REQUEST {
    USB_RQ_LOAD_FIRMWARE = 0xa0,
    USB_RQ_WRITE_SMALL_EEPROM = 0xa2
};

using namespace OpenSSAG;

static unsigned char bootloader[] = { SSAG_BOOTLOADER };
static unsigned char firmware[] = { SSAG_FIRMWARE };
static unsigned char eeprom[] = { SSAG_EEPROM };

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

void Loader::Upload(unsigned char *data)
{
    for (;;) {
        unsigned char byte_count = *data;
        if (byte_count == 0)
            break;
        unsigned short address = *(unsigned int *)(data+1);
        usb_control_msg(this->handle, 0x40, USB_RQ_LOAD_FIRMWARE, address, 0, (char *)(data+3), byte_count, 5000);
        data += byte_count + 3;
    }
}

void Loader::LoadFirmware()
{
    unsigned char *data = NULL;

    /* Load bootloader */
    this->EnterResetMode();
    this->EnterResetMode();
    printf("Loading bootloader...");
    this->Upload(bootloader);
    printf("done\n");
    this->ExitResetMode(); /* Transfer execution to the reset vector */

    sleep(1); /* Wait for renumeration */

    /* Load firmware */
    this->EnterResetMode();
    printf("Loading firmware...");
    this->Upload(firmware);
    printf("done\n");
    this->EnterResetMode(); /* Make sure the CPU is in reset */
    this->ExitResetMode(); /* Transfer execution to the reset vector */
}

void Loader::LoadEEPROM()
{
    size_t length = *eeprom;
    char *data = (char *)(eeprom+3);
    usb_control_msg(this->handle, 0x40, USB_RQ_WRITE_SMALL_EEPROM, 0x00, 0xBEEF, data, length, 5000);
}
