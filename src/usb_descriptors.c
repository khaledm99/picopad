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

typedef struct __attribute__((packed)){
    tusb_desc_configuration_t   config      ;
    tusb_desc_interface_t       interface   ;
    tusb_hid_descriptor_hid_t   hid         ;
    tusb_desc_endpoint_t        endpoint    ;

} packed_descriptors_t;

// HID Report Descriptor Macro as used in the TinyUSB examples.
// Chose to use this macro rather than spend the time learning the
// intricacies of building a report descriptor, which is out of
// the scope of this project
uint8_t const desc_hid_keyboard_report[] =
{
    TUD_HID_REPORT_DESC_KEYBOARD()
};

#define EP_NO  0x01

packed_descriptors_t const descriptors =
{
    .config = {
        .bLength            = sizeof(tusb_desc_configuration_t),
        .bDescriptorType    = TUSB_DESC_CONFIGURATION,
        .wTotalLength       = sizeof(tusb_desc_configuration_t) +
                              sizeof(tusb_desc_interface_t) +
                              sizeof(tusb_desc_endpoint_t) +
                              sizeof(tusb_hid_descriptor_hid_t),
        .bNumInterfaces     = 0x01,
        .bConfigurationValue = 0x01,
        .iConfiguration     = 0x00, // None
        .bmAttributes       = 1u << 7, // Bus-powered, using USB on Pico
        .bMaxPower          = 50    // 50*2 = 100mA
    },
    .interface = {
        .bLength            = sizeof(tusb_desc_interface_t),
        .bDescriptorType    = TUSB_DESC_INTERFACE,
        .bInterfaceNumber   = 0x00,
        .bAlternateSetting  = 0x00, 
        .bNumEndpoints      = 0x01,
        .bInterfaceClass    = 0x03, // HID code defined by USB
        .bInterfaceSubClass = HID_SUBCLASS_NONE,
        .bInterfaceProtocol = 0x00,
        .iInterface         = 0x00
    },
    .hid = {
        .bLength            = sizeof(tusb_hid_descriptor_hid_t),
        .bDescriptorType    = HID_DESC_TYPE_HID,
        .bcdHID             = 0x0111,   //HID spec 1.11
        .bCountryCode       = 0x00,
        .bNumDescriptors    = 0x01,
        .bReportType  = 0x22,
        .wReportLength  = sizeof(desc_hid_keyboard_report)
    },
    .endpoint = {
        .bLength = sizeof(tusb_desc_endpoint_t),
        .bDescriptorType    = TUSB_DESC_ENDPOINT,
        .bEndpointAddress   = TUSB_DIR_IN_MASK | EP_NO,
        .bmAttributes       = TUSB_XFER_INTERRUPT,
        .wMaxPacketSize     = CFG_TUD_HID_EP_BUFSIZE,
        .bInterval          = 1     // 1ms
    }
};
