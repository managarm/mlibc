.section .text
.global __mlibc_restorer
.hidden __mlibc_restorer
.type __mlibc_restorer, @function
__mlibc_restorer:
	lea -8(%rsp), %rdi
	mov $49, %rax
	syscall
	ud2
.size __mlibc_restorer, . - __mlibc_restorer

.section .note.GNU-stack,"",@progbits
