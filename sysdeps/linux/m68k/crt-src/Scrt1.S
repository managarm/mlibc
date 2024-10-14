.section .text
.global _start

.type _start, %function
.type main, %function
.type __mlibc_entry, %function

_start:
	suba.l %fp, %fp
	move.l %sp, %d0
	lea _GLOBAL_OFFSET_TABLE_@GOTPC (%pc), %a5
	move.l main@GOT(%a5), -(%sp)
	move.l %d0, -(%sp)
	jbsr __mlibc_entry@PLTPC

.section .note.GNU-stack,"",%progbits
