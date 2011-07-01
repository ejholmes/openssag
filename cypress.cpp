#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>

#include "util.h"
#include "openssag.h"
#include "firmware.h"

using namespace OpenSSAG;

static unsigned char firmware[] = { SSAG_FIRMWARE };

bool Cypress::Connect()
{
    this->EnterResetMode();
    return false;
}

void Cypress::Disconnect()
{
    this->ExitResetMode();
}

void Cypress::EnterResetMode()
{

}

void Cypress::ExitResetMode()
{

}

void Cypress::LoadFirmware()
{

}
