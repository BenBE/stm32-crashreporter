#include "crashreporter/handlers.h"

#include "version.h"

#include "crashreporter/isruart.h"
#include "crashreporter/macros.h"
#include "crashreporter/memory.h"

// Initialized by fault handlers before handing off to C implementations
cpustate_t cpustate;

static void internal_dump_hex_v(uint32_t value, size_t len) {
    static const char *hexdigits = "0123456789ABCDEF";

    for(size_t i = 0; i < len; i++) {
        cr_uart_putc(hexdigits[(value >> (4 * len - 4 * i - 4)) & 0x0F]);
    }
}

static void internal_dump_hex(uint32_t value) {
    internal_dump_hex_v(value, 8);
}

static void internal_dump_bin(uint32_t value) {
    for(size_t i = 0; i < 32; i++) {
        cr_uart_putc((value >> (31 - i)) & 0x01 ? '1' : '0');
    }
}

static void internal_dump_register(const char* reg, uint32_t value) {
    cr_uart_puts(reg);
    cr_uart_puts(":\t");
    internal_dump_hex(value);
    cr_uart_putc('\t');
    internal_dump_bin(value);
}

static void internal_dump_registers() {
    const char* regnames[] = {
        "r0", "r1", "r2", "r3",
        "r4", "r5", "r6", "r7",
        "r8", "r9", "r10", "r11",
        "r12", "sp", "lr", "pc"
        };

    for(size_t i = 0; i < NUM_ELEMS(regnames); i++) {
        internal_dump_register(regnames[i], cpustate.reg[i]);

        const memory_region_t* mr_ptr = cpustate.reg[i] ? cr_mm_getRegion((const void*)cpustate.reg[i]) : NULL;

        switch (i) {
        case 0: case 1: case 2: case 3:
        case 4: case 5: case 6: case 7:
        case 8: case 9: case 10: case 11:
        case 12:
            if(!cpustate.reg[i]) {
                cr_uart_puts("\t(NULL)");
                break;
            }

            if(!mr_ptr) {
                break;
            }

            cr_uart_putc('\t');
            cr_uart_puts(mr_ptr->name);

            if(!mr_ptr->bitband_source) {
                break;
            }

            memory_bitband_source_t bbsrc = cr_mm_getBBSourceAligned((const void*)cpustate.reg[i], mr_ptr, sizeof(uint32_t));
            if(bbsrc.ptr || bbsrc.bit) {
                cr_uart_puts("\tAlias: ");
                internal_dump_hex((uintptr_t)bbsrc.ptr);
                cr_uart_puts(", bit ");
                internal_dump_hex_v(bbsrc.bit, 2);

                const memory_region_t* mr_bbs = cr_mm_getRegion(bbsrc.ptr);
                if(mr_bbs) {
                    cr_uart_puts(" [");
                    cr_uart_puts(mr_bbs->name);
                    cr_uart_puts("]");
                }
            }
            break;

        case 13:
            if(mr_ptr) {
                cr_uart_putc('\t');
                cr_uart_puts(mr_ptr->name);
            } else {
                cr_uart_puts("\t(NULL)");
            }
            break;

        case 14:
        case 15:
            if(mr_ptr) {
                cr_uart_putc('\t');
                cr_uart_puts(mr_ptr->name);
            } else {
                cr_uart_puts("\t(NULL)");
                break;
            }

            cr_uart_puts(cpustate.reg[i] & 0x01 ? "\t(Thumb)" : "\t(ARM)");
            break;

        default:
            break;
        }
        cr_uart_puts("\r\n");
    }

    internal_dump_register("xPSR", cpustate.psr);
    cr_uart_puts("\tISR:");
    internal_dump_hex_v(cpustate.psr & 0x1FF, 3);
    cr_uart_puts(", Flags:");
    cr_uart_putc(cpustate.psr & 0x01000000 ? 'T' : '-');
    cr_uart_putc(' ');
    cr_uart_puts(cpustate.psr & 0x02000000 ? "IF" : "if");
    cr_uart_putc(' ');
    cr_uart_puts(cpustate.psr & 0x04000000 ? "ICI" : "ici");
    cr_uart_putc(' ');
    cr_uart_putc(cpustate.psr & 0x08000000 ? 'Q' : 'q');
    cr_uart_putc(cpustate.psr & 0x10000000 ? 'V' : 'v');
    cr_uart_putc(cpustate.psr & 0x20000000 ? 'C' : 'c');
    cr_uart_putc(cpustate.psr & 0x40000000 ? 'Z' : 'z');
    cr_uart_putc(cpustate.psr & 0x80000000 ? 'N' : 'n');
    cr_uart_puts("\r\n");
}

static void internal_dump_header(const char* header) {
    cr_uart_puts("\r\n########## ");
    cr_uart_puts(header);
    cr_uart_puts(" DETECTED ##########\r\n");

    cr_uart_puts("Firmware Product: ");
    cr_uart_puts(fw_product);
    cr_uart_puts("\r\n");

    cr_uart_puts("Firmware Version: ");
    cr_uart_puts(fw_version);
    cr_uart_puts("\r\n");

    cr_uart_puts("Firmware build:   ");
    cr_uart_puts(fw_builddate);
    cr_uart_puts("\r\n\r\n");

    cr_uart_puts("Register contents:\r\n");
    internal_dump_registers();
    cr_uart_puts("\r\n");
}

void cr_handle_NMI(uint32_t isr_lr, uint32_t isr_sp) {
    internal_dump_header("NON-MASKABLE INTERRUPT");

    // Nothing to do
}

void cr_handle_HardFault(uint32_t isr_lr, uint32_t isr_sp) {
    internal_dump_header("HARD FAULT");

    while(1) {
        //
    }
}

void cr_handle_MemManage(uint32_t isr_lr, uint32_t isr_sp) {
    internal_dump_header("MEMORY ACCESS FAULT");

    while(1) {
        //
    }
}

void cr_handle_BusFault(uint32_t isr_lr, uint32_t isr_sp) {
    internal_dump_header("BUS ACCESS FAULT");

    while(1) {
        //
    }
}

void cr_handle_UsageFault(uint32_t isr_lr, uint32_t isr_sp) {
    internal_dump_header("USAGE FAULT");

    while(1) {
        //
    }
}
