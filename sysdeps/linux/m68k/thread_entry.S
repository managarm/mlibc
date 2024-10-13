.section .text
.global __mlibc_spawn_thread
.type __mlibc_spawn_thread, "function"
/* int (int flags, void *stack, void *pid_out, void *child_tid, void *tcb); */
__mlibc_spawn_thread:
	movem.l %d2-%d5, -(%sp)

	move.l #120, %d0
	movem.l 20(%sp), %d1-%d5
	addi.l #0x7048, %d5
	trap #0

	tst.l %d0
	beq 1f

	movem.l (%sp)+, %d2-%d5
	rts

1:
	jbsr __mlibc_enter_thread@PLTPC
	illegal

.section .note.GNU-stack,"",%progbits

