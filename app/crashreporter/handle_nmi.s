/* Handler stub for handling the NMI ISR */

    .syntax unified
    .cpu    cortex-m4
    .thumb

.global     NMI_Handler

.section    .text.NMI_Handler, "ax", %progbits
.type       NMI_Handler, %function
NMI_Handler:
    push    {lr}
    bl      cr_cpuState_capture
    pop     {lr}
    mov     r0, lr
    mov     r1, sp
    b       cr_handle_NMI
    .size   NMI_Handler, . - NMI_Handler
