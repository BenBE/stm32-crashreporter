#include "crashreporter/memory.h"

#include "crashreporter/macros.h"

bool cr_mm_inRegion(const void* ptr, const memory_region_t* region) {
    if(ptr < region->offset) {
        return false;
    }

    if((char*)ptr - (char*)region->offset >= region->length) {
        return false;
    }

    return true;
}

memory_region_t const * cr_mm_getRegion(const void* ptr)
{
    memory_region_t const *result = NULL;

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
