/* Handler stub for handling the HardFault ISR */

    .syntax unified
    .cpu    cortex-m4
    .thumb

.global     HardFault_Handler

.section    .text.HardFault_Handler, "ax", %progbits
.type       HardFault_Handler, %function
HardFault_Handler:
    push    {lr}
    bl      cr_cpuState_capture
    pop     {lr}
    mov     r0, lr
    mov     r1, sp
    b       cr_handle_HardFault
    .size   HardFault_Handler, . - HardFault_Handler
