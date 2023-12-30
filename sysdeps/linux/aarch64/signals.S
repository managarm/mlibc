.section .text

.global __mlibc_signal_restore
.type __mlibc_signal_restore, @function
__mlibc_signal_restore:
.global __mlibc_signal_restore_rt
.type __mlibc_signal_restore_rt, @function
__mlibc_signal_restore_rt:
	mov x8,#139 // SYS_rt_sigreturn
	svc 0
.section .note.GNU-stack,"",%progbits

