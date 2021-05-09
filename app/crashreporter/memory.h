#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum memory_flags {
    MF_EXEC         = 1 << 0,
    MF_WRITE        = 1 << 1,
    MF_READ         = 1 << 2,
    MF_PERSIST      = 1 << 3,
    MF_ATOMIC       = 1 << 4,
    MF_PERIPHERAL   = 1 << 5
} memory_flags_t;

typedef struct memory_region {
    const void* offset;
    size_t length;
    uint32_t flags;
    const void* bitband_source;
    const char* name;
} memory_region_t;

typedef struct memory_bitband_source {
    const void* ptr;
    uint8_t bit;
} memory_bitband_source_t;

extern const memory_region_t memory_map[];

bool cr_mm_inRegion(const void* ptr, const memory_region_t* region);
memory_region_t const* cr_mm_getRegion(const void* ptr);

memory_bitband_source_t cr_mm_getBBSource(const void* ptr, const memory_region_t* region);
memory_bitband_source_t cr_mm_getBBSourceAligned(const void* ptr, const memory_region_t* region, size_t align);
