
    .extern     CORE_SaveIRQ
.macro  M_SAVE_IRQ  IrqID
.ifdef DEBUG_IRQ_MONITOR
	mov     r11,	r13
	push    {r0-r3}
	mov		r0,		r11
	mov		r1,		\IrqID
	bl		CORE_SaveIRQ
	pop		{r0-r3}
.endif
.endm
