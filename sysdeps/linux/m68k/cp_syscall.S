
.section .text
.global __mlibc_do_asm_cp_syscall
.global __mlibc_syscall_begin
.global __mlibc_syscall_end
.type __mlibc_do_asm_cp_syscall, "function"
__mlibc_do_asm_cp_syscall:
	movem.l %d2-%d5, -(%sp)
	jbsr __m68k_read_tp@PLTPC
	/* cancelBits is at TP - 0x7030; LSB at -0x702d */
	move.b -0x702d(%a0), %d0
__mlibc_syscall_begin:
	/* tcbCancelEnableBit && tcbCancelTriggerBit */
	andi.b #0x5, %d0
	cmpi.b #0x5, %d0
	beq cancel
	movem.l 20(%sp), %d0-%d5/%a0
	trap #0
__mlibc_syscall_end:
	movem.l (%sp)+, %d2-%d5
	rts

cancel:
	movem.l (%sp)+, %d2-%d5
	jbsr __mlibc_do_cancel@PLTPC
	illegal

.section .note.GNU-stack,"",%progbits
