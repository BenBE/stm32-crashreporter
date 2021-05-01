/* Helper function to retrieve low-level CPU state */

    .syntax unified
    .cpu    cortex-m4
    .thumb

.global     cr_cpuState_capture

.section    .text.crCaptureState, "ax", %progbits
.type       cr_cpuState_capture, %function
cr_cpuState_capture:
    ldr.w   r0, =cpustate

    // Read r0 from stack frame
    ldr     r1, [sp, #4]
    str     r1, [r0, #0]

    // Read r1 from stack frame
    ldr     r1, [sp, #8]
    str     r1, [r0, #4]

    // Read r2 from stack frame
    ldr     r1, [sp, #12]
    str     r1, [r0, #8]

    // Read r3 from stack frame
    ldr     r1, [sp, #16]
    str     r1, [r0, #12]

    // Read r12 from stack frame
    ldr     r1, [sp, #20]
    str     r1, [r0, #48]

    // Read lr from stack frame
    ldr     r1, [sp, #24]
    str     r1, [r0, #56]

    // Read pc from stack frame
    ldr     r1, [sp, #28]
    str     r1, [r0, #60]

    // Read xPSR from stack frame
    ldr     r1, [sp, #32]
    str     r1, [r0, #64]

    // Regenerate sp on entry to ISR
    add     r1, sp, #4
    str     r1, [r0, #52]

    // Store registers r4-r11 (unmodified so far)
    add     r0, r0, #16
    stm     r0, {r4-r11}

    bx      lr
    .size   cr_cpuState_capture, . - cr_cpuState_capture
