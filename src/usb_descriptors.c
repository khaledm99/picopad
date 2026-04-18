// Device Descriptor
#include "common/tusb_types.h"
#include "tusb.h"
#include "tusb_config.h"
#include "tusb_option.h"
#include <stdint.h>

// Based on hid_boot_interface example in tinyusb
tusb_desc_device_t const desc_device = 
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200, // BUSB Spec Release Num, binary coded decimal
    .bDeviceClass       = 0x00, // 0, defer to interface descriptor
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xFADE,
    .idProduct          = 0x0001,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01, // string descriptor indices, starting at 1 because 0 is reserved by USB spec for language list
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

tusb_desc_configuration_t const desc_config =
{
    .bLength            = sizeof(tusb_desc_configuration_t),
    .bDescriptorType    = TUSB_DESC_CONFIGURATION,
    .wTotalLength       = 0,    // I don't know how to compute this yet
    .bNumInterfaces     = 0x01,
    .bConfigurationValue = 0x01,
    .iConfiguration     = 0x00, // None
    .bmAttributes       = 1u << 7, // Bus-powered, using USB on Pico
    .bMaxPower          = 50    // 50*2 = 100mA
};

tusb_desc_interface_t const desc_interface =
{
    .bLength            = sizeof(tusb_desc_interface_t),
    .bDescriptorType    = TUSB_DESC_INTERFACE,
    .bInterfaceNumber   = 0x00,
    .bAlternateSetting  = 0x00, 
    .bNumEndpoints      = 0x01,
    .bInterfaceClass    = 0x03, // HID code defined by USB
    .bInterfaceSubClass = 0x00, // not boot device as this is a macropad
    .bInterfaceProtocol = 0x01, // code for keyboard
    .iInterface         = 0x00
};

typedef struct __attribute__((packed)) {
    uint8_t     bLength                 ;
    uint8_t     bDescriptorType         ;
    uint16_t    bcdHID                  ;
    uint8_t     bCountryCode            ;
    uint8_t     bNumDescriptors         ;
    uint8_t     bReportDescriptorType   ;
    uint16_t    wDescriptorLength       ;
} desc_hid_class_t;

desc_hid_class_t const desc_hid_class =
{
    .bLength            = sizeof(desc_hid_class_t),
    .bDescriptorType    = 0x21, // HID descriptor type assigned by USB
    .bcdHID             = 0x0111,   //HID spec 1.11
    .bCountryCode       = 0x00,
    .bNumDescriptors    = 0x01,
    .bReportDescriptorType  = 0x22,
    .wDescriptorLength  = 0x00  // I don't know how to compute this value
};

