#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct cpustate {
    uint32_t reg[16];   // registers r0..r12, sp(r13), lr(r14), pc(r15)
    uint32_t psr;
} cpustate_t;

extern cpustate_t cpustate;

void cr_handle_NMI(uint32_t isr_lr, uint32_t isr_sp);
void cr_handle_HardFault(uint32_t isr_lr, uint32_t isr_sp);
void cr_handle_MemManage(uint32_t isr_lr, uint32_t isr_sp);
void cr_handle_BusFault(uint32_t isr_lr, uint32_t isr_sp);
void cr_handle_UsageFault(uint32_t isr_lr, uint32_t isr_sp);
