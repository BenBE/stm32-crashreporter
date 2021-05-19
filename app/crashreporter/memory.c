#include "crashreporter/memory.h"

#include "crashreporter/macros.h"

extern void* _etext;
extern void* _sdata;
extern void* _edata;
extern void* _sbss;
extern void* _ebss;

bool cr_mm_inRegion(const void* ptr, const memory_region_t* region) {
    if(ptr < region->offset) {
        return false;
    }

    if((char*)ptr - (char*)region->offset >= region->length) {
        return false;
    }

    return true;
}

memory_region_t reg_text, reg_data, reg_bss;

memory_region_t const * cr_mm_getRegion(const void* ptr)
{
    memory_region_t const *result = NULL;

    // Special handling for .text, .data and .bss

    if((char*)ptr >= (char*)(0xFF000000 & (uintptr_t)(char*)&_etext) && (char*)ptr < (char*)&_etext) {
        reg_text = (memory_region_t) {
            .offset = (char*)(0xFF000000 & (uintptr_t)(char*)&_etext),
            .length = ~0xFF000000 & (uintptr_t)(char*)&_etext,
            .flags = MF_READ | MF_EXEC | MF_ATOMIC | MF_PERSIST,
            .name = ".text",
            .bitband_source = NULL
        };
        return &reg_text;
    } else if((char*)ptr >= (char*)&_sdata && (char*)ptr < (char*)&_edata) {
        reg_data = (memory_region_t) {
            .offset = &_sdata,
            .length = (char*)&_edata - (char*)&_sdata,
            .flags = MF_READ | MF_WRITE | MF_ATOMIC,
            .name = ".data",
            .bitband_source = NULL
        };
        return &reg_data;
    } else if((char*)ptr >= (char*)&_sbss && (char*)ptr < (char*)&_ebss) {
        reg_bss = (memory_region_t) {
            .offset = &_sbss,
            .length = (char*)&_ebss - (char*)&_sbss,
            .flags = MF_READ | MF_WRITE | MF_ATOMIC,
            .name = ".bss",
            .bitband_source = NULL
        };
        return &reg_bss;
    }

    // Now for normal section info ...
    for(size_t i = 0; memory_map[i].name; i++) {
        memory_region_t const *curr = &memory_map[i];

        if(!cr_mm_inRegion(ptr, curr)) {
            continue;
        }

        result = curr;
    }

    return result;
}

memory_bitband_source_t cr_mm_getBBSource(const void* ptr, const memory_region_t* region) {
    if (!cr_mm_inRegion(ptr, region)) {
        return (memory_bitband_source_t){ .ptr = NULL, .bit = 0 };
    }

    size_t offset = (uint32_t*)ptr - (uint32_t*)region->offset;
    size_t woffset = offset / 8;
    size_t boffset = offset % 8;

    const void* target = (uint8_t*)region->bitband_source + woffset;

    memory_region_t const * bbsrc = cr_mm_getRegion(target);

    if (!bbsrc) {
        return (memory_bitband_source_t){ .ptr = NULL, .bit = 0 };
    }

    return (memory_bitband_source_t){ .ptr = target, .bit = boffset };
}

memory_bitband_source_t cr_mm_getBBSourceAligned(const void* ptr, const memory_region_t* region, size_t align) {
    memory_bitband_source_t result = cr_mm_getBBSource((const void*)((uintptr_t)ptr & ~0x03), region);
    if(!result.ptr && !result.bit) {
        return result;
    }

    uintptr_t ptrval = (uintptr_t)result.ptr;
    ptrval %= align;
    result.ptr = (const void*)((uintptr_t)result.ptr - ptrval);
    result.bit += 8 * ptrval;

    return result;
}
