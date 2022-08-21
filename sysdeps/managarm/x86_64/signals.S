
.section .text
.global __mlibc_signal_restore
__mlibc_signal_restore:
	mov $0x80000006, %rdi
	syscall
	ud2

.section .note.GNU-stack,"",%progbits

