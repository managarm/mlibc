.section .text

.global __mlibc_signal_restore
.type __mlibc_signal_restore, @function
__mlibc_signal_restore:
.global __mlibc_signal_restore_rt
.type __mlibc_signal_restore_rt, @function
__mlibc_signal_restore_rt:
	li a7, 139
	ecall
.section .note.GNU-stack,"",%progbits

