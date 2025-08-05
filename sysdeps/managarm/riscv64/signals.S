.section .text
.global __mlibc_signal_restore
__mlibc_signal_restore:
	li a0, 0x80000006
	ecall
	unimp
.section .note.GNU-stack,"",%progbits
