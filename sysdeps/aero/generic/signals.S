.section .text
.global __mlibc_signal_restore

__mlibc_signal_restore:
	mov $39, %rax
	syscall
	ud2
.section .note.GNU-stack,"",%progbits

