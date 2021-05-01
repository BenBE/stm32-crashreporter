/* Handler stub for handling the BusFault ISR */

    .syntax unified
    .cpu    cortex-m4
    .thumb

.global     BusFault_Handler

.section    .text.BusFault_Handler, "ax", %progbits
.type       BusFault_Handler, %function
BusFault_Handler:
    push    {lr}
    bl      cr_cpuState_capture
    pop     {lr}
    mov     r0, lr
    mov     r1, sp
    b       cr_handle_BusFault
    .size   BusFault_Handler, . - BusFault_Handler
