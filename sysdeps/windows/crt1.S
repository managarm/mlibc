.section .text
.global mainCRTStartup
mainCRTStartup:
	andq $-16, %rsp
	subq $32, %rsp
	leaq __CTOR_LIST__(%rip), %rcx
	leaq main(%rip), %rdx
	call __mlibc_entry
	ud2

.global __main
__main:
	ret
