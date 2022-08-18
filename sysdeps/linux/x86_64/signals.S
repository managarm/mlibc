
.section .text
	nop
.global __mlibc_signal_restore
.type __mlibc_signal_restore, @function
__mlibc_signal_restore:
	mov $15, %rax
	syscall
	ud2
.section .note.GNU-stack,"",%progbits

