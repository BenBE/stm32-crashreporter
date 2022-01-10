#include "crashreporter/memory.h"

const memory_region_t memory_map[] = {

    // FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
    {
        .offset = (const void*)0x08000000,
        .length = 1024 << 10,
        .flags = MF_READ | MF_EXEC | MF_PERSIST | MF_ATOMIC,
        .name = "FLASH"
    },

    // RAM (rwx)       : ORIGIN = 0x20000000, LENGTH = 96K
    {
        .offset = (const void*)0x20000000,
        .length = 96 << 10,
        .flags = MF_READ | MF_WRITE | MF_EXEC | MF_ATOMIC,
        .name = "RAM1"
    },

    // RAM2 (rwx)      : ORIGIN = 0x10000000, LENGTH = 32K
    {
        .offset = (const void*)0x10000000,
        .length = 32 << 10,
        .flags = MF_READ | MF_WRITE | MF_EXEC | MF_ATOMIC,
        .name = "RAM2"
    },

    // BB_RAM (rw)     : ORIGIN = 0x22000000, LENGTH = 32M
    {
        .offset = (const void*)0x22000000,
        .length = 4 << 23,
        .flags = MF_READ | MF_WRITE,
        .bitband_source = (const void*)0x20000000,
        .name = "BB_RAM"
    },

    // PERIPH (rw)     : ORIGIN = 0x40000000, LENGTH = 1M
    {
        .offset = (const void*)0x40000000,
        .length = 1 << 20,
        .flags = MF_READ | MF_WRITE | MF_PERIPHERAL | MF_ATOMIC,
        .name = "BB_PERIPH"
    },

    // BB_PERIPH (rw)  : ORIGIN = 0x42000000, LENGTH = 32M
    {
        .offset = (const void*)0x42000000,
        .length = 32 << 20,
        .flags = MF_READ | MF_WRITE | MF_PERIPHERAL,
        .bitband_source = (const void*)0x40000000,
        .name = "BB_PERIPH"
    },

    // EXTRAM (rwx)    : ORIGIN = 0x60000000, LENGTH = 1G
    {
        .offset = (const void*)0x60000000,
        .length = 1 << 30,
        .flags = MF_READ | MF_WRITE | MF_EXEC | MF_PERIPHERAL,
        .name = "EXTRAM"
    },

    // EXTDEV (rw)     : ORIGIN = 0xA0000000, LENGTH = 1G
    {
        .offset = (const void*)0xA0000000,
        .length = 1 << 30,
        .flags = MF_READ | MF_WRITE | MF_PERIPHERAL,
        .name = "EXTDEV"
    },

    // PRIVBUS (rw)     : ORIGIN = 0xE0000000, LENGTH = 1M
    {
        .offset = (const void*)0xE0000000,
        .length = 1 << 20,
        .flags = MF_PERIPHERAL,
        .name = "PRIVBUS"
    },

    // VENDOR (rw)     : ORIGIN = 0xE0100000, LENGTH = 511M
    {
        .offset = (const void*)0xE0100000,
        .length = 511 << 20,
        .flags = MF_PERIPHERAL,
        .name = "EXTDEV"
    },

    // End of descriptor
    {
        .offset = NULL,
        .length = 0,
        .flags = 0,
        .bitband_source = NULL,
        .name = NULL
    }
};
