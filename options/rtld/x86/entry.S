.global _start
_start:
	call relocateSelf

	push %esp
	call interpreterMain

	jmp *%eax

.section .note.GNU-stack,"",%progbits
