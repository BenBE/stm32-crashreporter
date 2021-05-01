/* Handler stub for handling the MemManage ISR */

    .syntax unified
    .cpu    cortex-m4
    .thumb

.global     MemManage_Handler

.section    .text.MemManage_Handler, "ax", %progbits
.type       MemManage_Handler, %function
MemManage_Handler:
    push    {lr}
    bl      cr_cpuState_capture
    pop     {lr}
    mov     r0, lr
    mov     r1, sp
    b       cr_handle_MemManage
    .size   MemManage_Handler, . - MemManage_Handler
