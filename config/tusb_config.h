#pragma once


//--------------------------------------------------------------------+
// Board Specific Configuration
// Copied from tinyusb example
//--------------------------------------------------------------------+

// RHPort number used for device can be defined by board.mk, default to port 0
#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT      0
#endif

// RHPort max operational speed can defined by board.mk
#ifndef BOARD_TUD_MAX_SPEED
#define BOARD_TUD_MAX_SPEED   OPT_MODE_DEFAULT_SPEED
#endif

#define CFG_TUSB_RHPORT0_MODE   OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED
#define CFG_TUD_ENABLED     1
#define CFG_TUSB_MCU OPT_MCU_RP2040  // identifies our MCU as the RP2040, which our pico uses
#define CFG_TUSB_OS OPT_OS_PICO    // uses the RPi Pico-SDK

#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED

#define CFG_TUD_HID 1               // Enabled HID
#define CFG_TUD_HID_EP_BUFSIZE  8   // HID Endpoint buffer size 

#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE    64
#endif



