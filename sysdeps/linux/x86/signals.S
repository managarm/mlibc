.section .text

.global __mlibc_signal_restore
.type __mlibc_signal_restore, @function
__mlibc_signal_restore:
	popl %eax
	mov $119, %eax
	int $0x80
	ud2

.global __mlibc_signal_restore_rt
.type __mlibc_signal_restore_rt, @function
__mlibc_signal_restore_rt:
	mov $173, %eax
	int $0x80
	ud2

.section .note.GNU-stack,"",%progbits
