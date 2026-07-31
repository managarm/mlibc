.type __setjmp, "function"
__setjmp:
	movea.l 4(%sp), %a0
	movem.l %d2-%d7/%a2-%a7, (%a0)
	move.l (%sp), 48(%a0)
	tst.l %d0
	bne 1f

	clr.l %d0
	rts

1:
	move.l #1, -(%sp)
	move.l %a0, -(%sp)
	jbsr __sigsetjmp@PLTPC
	addq.l #8, %sp
	rts


.global setjmp
.type setjmp, "function"
.global _setjmp
.type _setjmp, "function"
setjmp:
_setjmp:
	clr.l %d0
	jmp __setjmp

.global sigsetjmp
.type sigsetjmp, "function"
sigsetjmp:
	move.l #1, %d0
	jmp __setjmp

.global longjmp
.type longjmp, "function"
.global _longjmp
.type _longjmp, "function"
longjmp:
_longjmp:
	movea.l 4(%sp),%a0
	move.l 8(%sp),%d0
	bne 1f

	move.l #1,%d0

1:
	movem.l (%a0), %d2-%d7/%a2-%a7
	move.l 48(%a0), (%sp)
	rts

.section .note.GNU-stack,"",%progbits
