
.section .text
.global __mlibc_signal_restore
__mlibc_signal_restore:
	ldr x0, =0x80000006
	svc 0
	brk #1

.section .note.GNU-stack,"",%progbits

