#include "crashreporter/handlers.h"

// Initialized by fault handlers before handing off to C implementations
cpustate_t cpustate;

void cr_handle_NMI(uint32_t isr_lr, uint32_t isr_sp) {
    // Nothing to do
}

void cr_handle_HardFault(uint32_t isr_lr, uint32_t isr_sp) {
    while(1) {
        //
    }
}

void cr_handle_MemManage(uint32_t isr_lr, uint32_t isr_sp) {
    while(1) {
        //
    }
}

void cr_handle_BusFault(uint32_t isr_lr, uint32_t isr_sp) {
    while(1) {
        //
    }
}

void cr_handle_UsageFault(uint32_t isr_lr, uint32_t isr_sp) {
    while(1) {
        //
    }
}
