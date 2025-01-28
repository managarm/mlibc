.section .text

.global __mlibc_signal_restore
.type __mlibc_signal_restore, @function
__mlibc_signal_restore:
	move.l (%sp)+, %d1
	move.l #119, %d0
	trap #0
	illegal

.global __mlibc_signal_restore_rt
.type __mlibc_signal_restore_rt, @function
__mlibc_signal_restore_rt:
	move.l (%sp)+, %d1
	move.l #173, %d0
	trap #0
	illegal

.section .note.GNU-stack,"",%progbits
