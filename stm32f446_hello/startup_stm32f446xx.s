    .syntax unified
    .cpu cortex-m4
    .thumb

    .section .isr_vector,"a",%progbits
    .type g_pfnVectors, %object
    .size g_pfnVectors, .-g_pfnVectors

    .globl g_pfnVectors
    g_pfnVectors:
    .word _estack
    .word Reset_Handler

    .weak Reset_Handler
    .type Reset_Handler, %function
Reset_Handler:
    bl SystemInit
    bl main
1:  b 1b

    .size Reset_Handler, .-Reset_Handler

    .section .stack
_estack:
    .space 0x400

    .end

