.section .text
	nop // TODO: why is this needed?
.global __mlibc_signal_restore
.type __mlibc_signal_restore, @function
__mlibc_signal_restore:
	unimp // TODO
.section .note.GNU-stack,"",%progbits

