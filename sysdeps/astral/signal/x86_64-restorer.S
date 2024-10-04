.section .text
.global __mlibc_restorer
__mlibc_restorer:
	sub $8, %rsp
	mov $62, %rax
	syscall
.section .note.GNU-stack,"",%progbits
