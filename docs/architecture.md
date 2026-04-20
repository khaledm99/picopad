# Macropad Firmware — Architecture Document

**Platform:** Raspberry Pi Pico (RP2040)  
**Language:** C (C11)  
**SDK:** Raspberry Pi Pico SDK with TinyUSB  
**Target OS:** Windows (primary), Linux (future)  
**Document status:** Living document — update when architecture decisions change

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Repository Structure](#2-repository-structure)
3. [Firmware Architecture](#3-firmware-architecture)
4. [Layer Specifications](#4-layer-specifications)
5. [Hardware Design](#5-hardware-design)
6. [Configuration System](#6-configuration-system)
7. [Key Scanning — Design Rationale](#7-key-scanning--design-rationale)
8. [GPIO Pin Assignments](#8-gpio-pin-assignments)
9. [Build System](#9-build-system)
10. [Development Phases](#10-development-phases)

---

## 1. Project Overview

A USB HID macropad built from scratch on the Raspberry Pi Pico. The device enumerates as a standard USB HID keyboard — no drivers required on Windows or Linux. Firmware is written entirely against the Pico SDK; no existing keyboard firmware (QMK, KMK, etc.) is used.

The design prioritises:

- **Scalability** — key count is a compile-time constant; adding or removing keys requires editing a single config file and recompiling, with no changes to firmware logic.
- **Clarity** — each firmware layer has a single responsibility and does not call upward into higher layers.
- **Portability** — no OS-specific USB drivers are required. Standard HID keyboard reports are used throughout.

---

## 2. Repository Structure

```
macropad/
├── CMakeLists.txt              Root build file
├── pico-sdk                    Pico SDK git submodule
│
├── src/
│   ├── main.c                  Entry point, peripheral init, main superloop
│   ├── keys.c                  Key scanner — GPIO polling and debounce state machine
│   ├── keys.h
│   ├── keymap.c                Keymap engine — physical index to HID keycode lookup
│   ├── keymap.h
│   ├── usb_descriptors.c       TinyUSB HID device and report descriptors
│   ├── usb_hid.c               HID report assembly and submission
│   └── usb_hid.h               
│
├── config/
│   ├── tusb_options.h
│   └── keymap_config.h         THE only file edited to change key count, pins, or mapping
│
├── hardware/
│   ├── schematic/              KiCad schematic source files
│   └── pcb/                    KiCad PCB layout files
│
└── docs/
    └── architecture.md         This document
```

All firmware logic lives under `src/`. The `config/` directory is intentionally separate — it is the only file a user needs to touch to customise the pad.

---

## 3. Firmware Architecture

The firmware is structured as four layers. Data flows strictly downward; no layer calls into a higher layer.

```
┌─────────────────────────────────────────┐
│          Windows PC (USB Host)          │
└──────────────────┬──────────────────────┘
                   │  USB HID (1 ms frames)
┌──────────────────▼──────────────────────┐
│            USB HID Layer                │
│   TinyUSB · 6-key HID keyboard report   │
└──────────────────┬──────────────────────┘
                   │  HID keycodes
┌──────────────────▼──────────────────────┐
│          Keymap / Macro Engine          │
│   Lookup table · configurable at        │
│   compile time via keymap_config.h      │
└──────────────────┬──────────────────────┘
                   │  key pressed/released events
┌──────────────────▼──────────────────────┐
│              Key Scanner                │
│   GPIO polling · debounce · event queue │
└──────────────────┬──────────────────────┘
                   │  GPIO reads
┌──────────────────▼──────────────────────┐
│        RP2040 GPIO (Hardware)           │
│   Internal pull-ups · N key inputs      │
└──────────────────┬──────────────────────┘
                   │
┌──────────────────▼──────────────────────┐
│       Physical Switches (N keys)        │
└─────────────────────────────────────────┘
```

The main loop in `main.c` orchestrates all layers on each iteration:

1. Call `keys_scan()` — reads GPIO state, runs debounce, populates event queue
2. Call `consume_event` — consumes events, produces active HID keycode set
3. Call `hid_task()` — assembles and submits HID report
4. Call `tud_task()` — services the TinyUSB stack

No RTOS is used. A tight superloop is appropriate at this complexity level and avoids the overhead and non-determinism of task scheduling.

---

## 4. Layer Specifications

### 4.1 Key Scanner (`keys.c` / `keys.h`)

**Responsibility:** Translate raw GPIO state into discrete key events.

**Mechanism:** A repeating timer interrupt fires every 1 ms and sets a `scan_pending` flag. The main loop checks this flag and calls the scan routine. Each GPIO pin is read and fed through a per-key debounce state machine. A key state change is only accepted after the pin has held a stable level for `DEBOUNCE_MS` (default: 5) consecutive milliseconds.

Accepted state changes are written as `KEY_EVENT_PRESS` or `KEY_EVENT_RELEASE` records into a small fixed-size ring buffer.

**What this layer does not know:** keycodes, USB, HID reports.

**Key types:**

```c
typedef enum { KEY_EVENT_PRESS, KEY_EVENT_RELEASE } key_event_type_t;

typedef struct {
    uint8_t          key_index;   // 0 .. KEY_COUNT-1
    key_event_type_t type;
} key_event_t;
```

### 4.2 Keymap Engine (`keymap.c` / `keymap.h`)

**Responsibility:** Map physical key indices to HID keycodes.

**Mechanism:** A flat array `keymap[KEY_COUNT]` defined in `keymap_config.h` maps each key index to a standard USB HID keycode (e.g. `HID_KEY_A`, `HID_KEY_F13`). On each press event the corresponding keycode is added to the active set; on release it is removed.

The active set is a fixed-size array of up to 6 simultaneous keycodes, matching the standard HID boot keyboard report format.

**Future extension point:** A second layer array `keymap_layer1[KEY_COUNT]` and a layer-toggle key can be added here when needed, without touching any other layer.

### 4.3 USB HID Layer (`usb_hid.c`, `usb_descriptors.c`)

**Responsibility:** Present the device as a USB HID keyboard and transmit key state to the host.

**Mechanism:** TinyUSB (bundled with the Pico SDK) handles USB enumeration, descriptor exchange, and interrupt endpoint management. `usb_descriptors.c` defines the USB device descriptor, HID report descriptor, and string descriptors. `usb_hid.c` calls `tud_hid_keyboard_report()` on each main loop iteration when the active keycode set has changed.

The HID report descriptor declares a standard keyboard. This ensures compatibility with Windows and Linux without any custom driver.

**`tud_task()`** must be called in the main loop to service TinyUSB's internal state machine.

### 4.4 Main Loop (`main.c`)

**Responsibility:** System initialisation and loop orchestration.

**Initialisation sequence:**

1. `stdio_init_all()` — UART debug output on GP0/GP1 (debug builds only)
2. `tusb_init()` — initialise TinyUSB
3. `keys_init()` — configure GPIO pins, enable pull-ups, start debounce timer

**Main loop:** Runs continuously with no blocking delays. All time-sensitive work is driven by the 1 ms timer flag set in the scan ISR.

---

## 5. Hardware Design

### 5.1 Prototype (Breadboard)

Each switch connects one leg to a GPIO pin and the other leg to GND. The RP2040's internal pull-up resistors are enabled in firmware (`gpio_pull_up()`), so no external resistors are needed on the breadboard.

- Key pressed → GPIO reads LOW
- Key released → GPIO reads HIGH (pulled up)

Power is supplied entirely from the USB port. No external regulator or power supply is required. The RP2040 draws approximately 25 mA typical in active operation; switches draw negligible current.

### 5.2 PCB Design

The Pico module is used as a component — soldering directly to the PCB via its castellated edge pads. This avoids the need to design RP2040 power, crystal, and USB circuitry from scratch on the first revision.

PCB design follows the guidance in the *Hardware design with RP2040* reference document. Key requirements:

- **Pull-up resistors:** Move from internal (firmware) to external 10 kΩ pull-ups on the PCB for noise immunity.
- **Decoupling capacitors:** 100 nF ceramic cap on each GPIO-adjacent VCC supply point per RP2040 hardware design guidelines.
- **SWD debug header:** Expose SWCLK and SWDIO as either test points or a 3-pin header (GND, SWCLK, SWDIO). This allows a Picoprobe to be connected without disassembly.
- **Layer count:** 2-layer board is sufficient for this design.
- **EDA tool:** KiCad. Source files live in `hardware/`.

---

## 6. Configuration System

All user-facing configuration is isolated in `config/keymap_config.h`. This is the only file that changes when the physical layout changes.

```c
// config/keymap_config.h

#define KEY_COUNT  4                          // Total number of physical keys

// GPIO pin for each key, indexed 0 .. KEY_COUNT-1
static const uint KEY_PINS[KEY_COUNT] = {
    2,   // Key 0
    3,   // Key 1
    4,   // Key 2
    5,   // Key 3
};

// HID keycode for each key, indexed 0 .. KEY_COUNT-1
// Use constants from pico-sdk/lib/tinyusb/src/class/hid/hid.h
static const uint8_t KEY_MAP[KEY_COUNT] = {
    HID_KEY_F13,   // Key 0
    HID_KEY_F14,   // Key 1
    HID_KEY_F15,   // Key 2
    HID_KEY_F16,   // Key 3
};
```

The scanner, keymap engine, and any diagnostic code derive all array sizes from `KEY_COUNT` at compile time via `sizeof` and static assertions. Adding a key means incrementing `KEY_COUNT`, adding a pin to `KEY_PINS`, and adding a keycode to `KEY_MAP`. No other files change.

---

## 7. Key Scanning — Design Rationale

### Why polling, not GPIO interrupts

Polling is the industry standard for keyboard firmware. QMK, the most widely used open-source keyboard firmware, uses a timer-driven scan loop — not GPIO change interrupts. The reasons are:

**Mechanical switch bounce makes interrupt-driven designs impractical.** When a mechanical switch closes, the contacts bounce — making and breaking contact many times over approximately 5 ms before settling. A GPIO interrupt fires on every edge, so a single keypress would generate 20–50 interrupts in rapid succession. Suppressing those spurious events requires a timer-based holdoff, at which point the complexity of interrupt-driven scanning has been incurred with no benefit over a simple polling loop.

**1 ms polling is fast enough for all practical purposes.** A 1 ms scan interval produces at most 1 ms of keypress detection latency. Human reaction time is approximately 150–200 ms. High-end gaming keyboards advertise 1 ms USB polling as a premium feature — it is the perceptible ceiling, not a baseline. The RP2040 at 125 MHz reads and debounces all GPIO pins in microseconds; a 16-key scan is negligible CPU overhead.

**Polling produces deterministic, testable behaviour.** Each scan tick processes the same logic in the same order. There are no race conditions between ISR context and main loop context, no shared state requiring critical sections, and no possibility of an interrupt starving the USB task.

### When interrupts would be appropriate

A GPIO wake interrupt would be appropriate if the device needed to enter deep sleep between keypresses to save power — the interrupt would bring the processor out of sleep, after which polling takes over for debounce. A USB bus-powered macropad that is always connected has no use case for this.

---

## 8. GPIO Pin Assignments

| Signal | GPIO Pin | Notes |
|---|---|---|
| Key 0 | GP2 | Internal pull-up enabled |
| Key 1 | GP3 | Internal pull-up enabled |
| Key 2 | GP4 | Internal pull-up enabled |
| Key 3 | GP5 | Internal pull-up enabled |
| UART TX (debug) | GP0 | Debug builds only — `stdio_init_all()` |
| UART RX (debug) | GP1 | Debug builds only |
| SWD SWCLK | GP26 | Reserved — expose on PCB as test point |
| SWD SWDIO | GP27 | Reserved — expose on PCB as test point |
| USB D+ | GP16 | Internal — do not route externally |
| USB D- | GP17 | Internal — do not route externally |

GP0 and GP1 are reserved for UART debug output during development and stripped in release builds via `#ifdef DEBUG`. GP2–GP5 are the 4-key prototype assignment; they are consecutive, easy to trace on a breadboard, and clear of all reserved functions.

When the key count is finalised for the PCB revision, additional keys continue sequentially from GP6 onward, skipping GP23 (SMPS mode), GP24 (VBUS detect), GP25 (onboard LED), GP26–GP28 (ADC / SWD).

---

## 9. Build System

The project uses CMake with the Pico SDK toolchain.

### Prerequisites

- `arm-none-eabi-gcc` (ARM GCC cross-compiler)
- `cmake` ≥ 3.13
- `ninja` or `make`
- Pico SDK cloned and `PICO_SDK_PATH` set in the environment

### Build

```bash
./setup.sh
cd build && ninja
```

This produces `macropad.uf2` in the build directory.

### Flashing

**BOOTSEL method (no debugger required):**
1. Hold the BOOTSEL button on the Pico
2. Connect USB to the PC
3. Release BOOTSEL — the Pico appears as a USB mass storage device
4. Copy `macropad.uf2` to the drive — it reboots automatically



---

## 10. Development Phases

### Phase 1 — Breadboard prototype (complete)

- 4 keys on breadboard using GP2–GP5
- Pico powered and communicating over USB
- Validate key scanner, debounce, and HID report pipeline
- Success criterion: Windows recognises the device as a keyboard and receives correct HID keycodes for all 4 keys with no spurious events

### Phase 2 — Key count expansion (current)

- Acquire remaining switches; finalise key count
- Update `KEY_COUNT`, `KEY_PINS`, and `KEY_MAP` in `keymap_config.h`
- No firmware logic changes required
- Validate on Windows; begin Linux smoke testing

### Phase 3 — PCB

- Draw schematic in KiCad against the final key count
- Review against *Hardware design with RP2040* guidelines
- Route 2-layer PCB; move pull-ups to external 10 kΩ resistors
- Order prototype boards (e.g. JLCPCB or PCBWay)
- Validate assembled PCB behaviour matches breadboard prototype

### Phase 4 — Polish

- Linux HID compatibility validation
- Optional: runtime keymap storage via on-chip flash (user can remap without recompiling)
- Strip UART debug output from production build
- Finalise enclosure / mounting design if required

---

*Last updated: project inception.*  
*Update this document whenever a significant architecture decision is made or revised.*
