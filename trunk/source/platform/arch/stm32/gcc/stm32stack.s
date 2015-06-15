
  .syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.thumb
    .section .text
    .align 4

.word _Stack_Init

    .global CORE_GetIdleStackBuffer
    .type	CORE_GetIdleStackBuffer, %function
CORE_GetIdleStackBuffer:
    ldr r0, = _Stack_Init
    bx  lr
.end
