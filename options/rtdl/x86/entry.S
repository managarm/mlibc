.global _start
_start:
	call relocateSelf

	mov %esp, %edi
	call interpreterMain

	jmp *%eax
