.section .text
.global _start

.type _start, %function
.type main, %function
.type __mlibc_entry, %function

_start:
	suba.l %fp, %fp
	lea _GLOBAL_OFFSET_TABLE_@GOTPC (%pc), %a5
	move.l main@GOT(%a5), -(%sp)
	move.l %sp, -(%sp)
	jbsr __mlibc_entry@PLTPC

.section .note.GNU-stack,"",%progbits
