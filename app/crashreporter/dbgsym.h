#pragma once

#include <stdbool.h>
#include <stdint.h>

#define DBGSYM_MAGIC 0x00474244 /* .asciz "DBG" */

typedef struct dbgsym_header {
    uint32_t magic;
    uint32_t sym_count;
    uint32_t sym_flags;
    uint32_t checksum;
} dbgsym_header_t;

typedef struct dbgsym_symbol {
    uint32_t addr;
    union {
        uint32_t symdata;
        struct {
            uint32_t size : 24;
            uint32_t flags : 8;
        };
    };
} dbgsym_symbol_t;

// IMPORTANT: Symbol to be provided via the Linker Script
// MUST point to .dbgsym section, which must be the last
// section to written in the final image.
extern const void* dbgsym_data;

extern bool dbgsym_available();
extern bool dbgsym_find_symbol(uint32_t addr, uint32_t *index);

extern const dbgsym_header_t* dbgsym_get_header();
extern const dbgsym_symbol_t* dbgsym_get_symdata();
extern const char* dbgsym_get_symnames();

extern const dbgsym_symbol_t* dbgsym_symbol_data(uint32_t index);
extern const char* dbgsym_symbol_name(uint32_t index);
