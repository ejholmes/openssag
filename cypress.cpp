#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>

#include "util.h"
#include "openssag.h"
#include "firmware.h"

#define CPUCS_ADDRESS 0xe600

enum USB_REQUEST {
    USB_RQ_LOAD_FIRMWARE = 0xa0,
};

using namespace OpenSSAG;

static unsigned char bootloader[] = { SSAG_BOOTLOADER };
static unsigned char firmware[] = { SSAG_FIRMWARE };

bool Cypress::Connect()
{
    if (!usb_open_device(&this->handle, CYPRESS_VENDOR_ID, CYPRESS_PRODUCT_ID, NULL)) {
        return false;
    }
    this->EnterResetMode();
    return false;
}

void Cypress::Disconnect()
{
    this->ExitResetMode();
}

void Cypress::EnterResetMode()
{
    char data = 0x01;
    usb_control_msg(this->handle, 0x40, USB_RQ_LOAD_FIRMWARE, 0x7f92, 0, &data, 1, 5000);
    usb_control_msg(this->handle, 0x40, USB_RQ_LOAD_FIRMWARE, CPUCS_ADDRESS, 0, &data, 1, 5000);
}

void Cypress::ExitResetMode()
{
    char data = 0x00;
    usb_control_msg(this->handle, 0x40, USB_RQ_LOAD_FIRMWARE, 0x7f92, 0, &data, 1, 5000);
    usb_control_msg(this->handle, 0x40, USB_RQ_LOAD_FIRMWARE, CPUCS_ADDRESS, 0, &data, 1, 5000);
}

void Cypress::LoadFirmware()
{
    /* Load bootloader */
    this->EnterResetMode();
    this->EnterResetMode();
    // TODO: Do bootloader loading
    this->ExitResetMode(); /* Transfer execution to the reset vector */

    /* Load firmware */
    this->EnterResetMode();
    // TODO: Do firmware loading
    this->EnterResetMode(); /* Make sure the CPU is in reset */
    this->ExitResetMode(); /* Transfer execution to the reset vector */
}
