cmake_minimum_required(VERSION 3.13)

include(pico-sdk/pico_sdk_init.cmake)

project(macropad C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(macropad
	src/main.c
    src/keys.c
    src/keymap.c
    src/usb_hid.c
    src/usb_descriptors.c
)

target_link_libraries(macropad
	pico_stdlib
    tinyusb_device
    tinyusb_board
)
target_include_directories(macropad
    PUBLIC
    config/
    src/
    ${CMAKE_SOURCE_DIR}/pico-sdk/lib/tinyusb/src/
    ${CMAKE_SOURCE_DIR}/pico-sdk/lib/tinyusb/hw/
)

pico_add_extra_outputs(macropad)

