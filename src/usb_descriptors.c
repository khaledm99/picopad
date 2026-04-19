// Device Descriptor
#include "bsp/board_api.h"
#include "common/tusb_types.h"
#include "class/hid/hid_device.h"
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


uint8_t const * tud_descriptor_device_cb(void)
{
    return (uint8_t const *) &desc_device;
}

uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
    return (uint8_t const *) &descriptors;
}

uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance)
{
    return desc_hid_keyboard_report;
}

//--------------------------------------------------------------------+
// String Descriptors
// Copied from tinyusb hid_boot_interface example
//--------------------------------------------------------------------+

// String Descriptor Index
enum {
  STRID_LANGID = 0,
  STRID_MANUFACTURER,
  STRID_PRODUCT,
  STRID_SERIAL,
};

// array of pointer to string descriptors
char const *string_desc_arr[] =
{
  (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
  "Khaled Mograbee",                     // 1: Manufacturer
  "Picopad",              // 2: Product
  "01",                          // 3: Serials will use unique ID if possible
};

static uint16_t _desc_str[32 + 1];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void) langid;
  size_t chr_count;

  switch ( index ) {
    case STRID_LANGID:
      memcpy(&_desc_str[1], string_desc_arr[0], 2);
      chr_count = 1;
      break;

    case STRID_SERIAL:
      chr_count = board_usb_get_serial(_desc_str + 1, 32);
      break;

    default:
      // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
      // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

      if ( !(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) ) return NULL;

      const char *str = string_desc_arr[index];

      // Cap at max char
      chr_count = strlen(str);
      size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1; // -1 for string type
      if ( chr_count > max_count ) chr_count = max_count;

      // Convert ASCII string into UTF-16
      for ( size_t i = 0; i < chr_count; i++ ) {
        _desc_str[1 + i] = str[i];
      }
      break;
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

  return _desc_str;
}
