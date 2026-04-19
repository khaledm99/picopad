// Device Descriptor
#include "common/tusb_types.h"
#include "class/hid/hid.h"
#include "tusb.h"
#include "tusb_config.h"
#include "tusb_option.h"
#include <stdint.h>

// HID descriptor structs from tusb_types.h
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
    .bInterfaceSubClass = HID_SUBCLASS_NONE,
    .bInterfaceProtocol = HID_ITF_PROTOCOL_KEYBOARD,
    .iInterface         = 0x00
};

// USB HID descriptor from hid.h
tusb_hid_descriptor_hid_t const desc_hid_class =
{
    .bLength            = sizeof(tusb_hid_descriptor_hid_t),
    .bDescriptorType    = HID_DESC_TYPE_HID,
    .bcdHID             = 0x0111,   //HID spec 1.11
    .bCountryCode       = HID_LOCAL_US,
    .bNumDescriptors    = 0x01,
    .bReportType  = 0x22,
    .wReportLength  = 0x00  // I don't know how to compute this value
};

