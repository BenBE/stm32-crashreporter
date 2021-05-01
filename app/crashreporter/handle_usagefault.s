/* Handler stub for handling the UsageFault ISR */

    .syntax unified
    .cpu    cortex-m4
    .thumb

.global     UsageFault_Handler

.section    .text.UsageFault_Handler, "ax", %progbits
.type       UsageFault_Handler, %function
UsageFault_Handler:
    push    {lr}
    bl      cr_cpuState_capture
    pop     {lr}
    mov     r0, lr
    mov     r1, sp
    b       cr_handle_UsageFault
    .size   UsageFault_Handler, . - UsageFault_Handler
