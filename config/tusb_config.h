#pragma once

#define CFG_TUSB_MCU OPT_MCU_RP2040  // identifies our MCU as the RP2040, which our pico uses
#define CFG_TUSB_OS OPT_OS_PICO    // uses the RPi Pico-SDK

#define CFG_TUD_HID 1
#define CFG_TUD_HID_EP_BUFSIZE  8


