cmake_minimum_required(VERSION 3.13)

include(pico-sdk/pico_sdk_init.cmake)

project(macropad C CXX ASM)

pico_sdk_init()

add_executable(macropad
	src/main.c
)

target_link_libraries(macropad
	pico_stdlib
)

pico_add_extra_outputs(macropad)

