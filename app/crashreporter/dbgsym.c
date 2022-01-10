#include "crashreporter/dbgsym.h"

#include <stddef.h>

static bool dbgsym_available_checked = false;
static bool dbgsym_available_result = false;

bool dbgsym_available() {
    if (!dbgsym_available_checked) {
        dbgsym_available_result = false;
        dbgsym_available_checked = true;

        const dbgsym_header_t* hdr = dbgsym_data;
        if (DBGSYM_MAGIC != hdr->magic) {
            goto end;
        }

        if (hdr->sym_count >= (1<<20)) {
            goto end;
        }

        if (hdr->checksum) {
            goto end;
        }

        dbgsym_available_result = true;
    }

end:
    return dbgsym_available_result;
}

bool dbgsym_find_symbol(uint32_t addr, uint32_t *index) {
    if (index) {
        *index = 0;
    }

    if (!dbgsym_available()) {
        return false;
    }

    const dbgsym_header_t* header = dbgsym_get_header();
    const dbgsym_symbol_t* symdata = dbgsym_get_symdata();

    if (!header || !symdata) {
        return NULL;
    }

    // First version: simple linear search
    for (uint32_t i = 0; i < header->sym_count; i++) {
        if (addr >= symdata[i].addr && addr < symdata[i].addr + symdata[i].size) {
            if(index) {
                *index = i;
            }
            return true;
        }

        if (addr < symdata[i].addr) {
            break;
        }
    }

    return false;
}

const dbgsym_header_t* dbgsym_get_header() {
    if(!dbgsym_available()) {
        return NULL;
    }

    return dbgsym_data;
}

const dbgsym_symbol_t* dbgsym_get_symdata() {
    if(!dbgsym_available()) {
        return NULL;
    }

    const dbgsym_header_t* header = dbgsym_get_header();

    return (const dbgsym_symbol_t*)(((const char*)header) + 16);
}

const char* dbgsym_get_symnames() {
    if(!dbgsym_available()) {
        return NULL;
    }

    const dbgsym_header_t* header = dbgsym_get_header();
    const dbgsym_symbol_t* symdata = dbgsym_get_symdata();

    if (!header || !symdata) {
        return NULL;
    }

    uint32_t size = header->sym_count;
    return (const char*)(symdata + size);
}

const dbgsym_symbol_t* dbgsym_symbol_data(uint32_t index) {
    if(!dbgsym_available()) {
        return NULL;
    }

    const dbgsym_header_t* header = dbgsym_get_header();
    if (index >= header->sym_count) {
        return NULL;
    }

    const dbgsym_symbol_t* symdata = dbgsym_get_symdata();
    return symdata + index;
}

const char* dbgsym_symbol_name(uint32_t index) {
    if(!dbgsym_available()) {
        return NULL;
    }

    const dbgsym_header_t* header = dbgsym_get_header();
    if (index >= header->sym_count) {
        return NULL;
    }

    const char* result = dbgsym_get_symnames();
    if (!result) {
        return NULL;
    }

    for(; index; index--) {
        for(; *result; result++) {
            // Nothing to do
        }

        // Skip NUL terminator
        result++;
    }

    return result;
}
