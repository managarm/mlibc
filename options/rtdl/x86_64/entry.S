
.global _start
_start:
	call relocateSelf

	mov %rsp, %rdi
	call interpreterMain

	jmp *%rax
