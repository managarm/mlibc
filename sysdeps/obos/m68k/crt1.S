.section .text
.global _start

.type _start, %function
.type main, %function
.type __mlibc_entry, %function

_start:
	suba.l %fp, %fp
	move.l %sp, %d0
	move.l #main, -(%sp)
	move.l %d0, -(%sp)
	jsr __mlibc_entry

.section .note.GNU-stack,"",%progbits
