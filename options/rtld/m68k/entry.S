.global _start
_start:
	lea (_GLOBAL_OFFSET_TABLE_@GOTPC,%pc), %a0
	lea (_DYNAMIC,%pc), %a1

	move.l %a1, %a5
	sub.l (%a0), %a5

	move.l %a5, -(%sp)
	move.l %a1, -(%sp)
	jbsr relocateSelf68k@PLTPC
	addq.l #8, %sp

	move.l %sp, -(%sp)
	jbsr interpreterMain@PLTPC

	jmp (%a0)

.section .note.GNU-stack,"",%progbits
