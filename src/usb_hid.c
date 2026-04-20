// usb_hid.c - HID Layer
// Implements callback functions from TinyUSB
//
//

#include "class/hid/hid_device.h"



// This function does nothing as our macropad has nothing
// requiring a SET command from the host (LEDs, etc.),
// but must be implemented for TinyUSB
void tud_hid_set_report_cb(uint8_t instance, 
                           uint8_t report_id, 
                           hid_report_type_t report_type, 
                           uint8_t const* buffer, 
                           uint16_t bufsize)
{
    return;
};

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) 
{
    return 0;
};
// This function must also be implemented for TinyUSB, but we need nothing from it
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
    return;
}

// Wrapper to send keyboard hid report to host
// Invokes TinyUSB helper function tud_hid_keyboard_report to send the report
void hid_task(const uint8_t keycode[6])
{
    static bool buf_empty_flag;
    bool test;

    // Search for non-zero keycode. If found, immediately set
    // empty flag to false and send report before returning
    for(int i=0; i<6; i++){
        if(keycode[i] != HID_KEY_NONE){
            buf_empty_flag = false;
            test = tud_hid_keyboard_report(0, 0, keycode);
            return;
        }
    }
    // If we've escaped the loop, then the buffer is all HID_KEY_NONE
    // If the flag is nonempty, that indicates a state change from nonempty
    // to empty, so we need to send one more empty report
    if(!buf_empty_flag) {
        test = tud_hid_keyboard_report(0, 0, keycode);
    }
    if(test) {
        buf_empty_flag = true;
    }
}

