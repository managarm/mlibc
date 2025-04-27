.section .text
.global __mlibc_start_thread
__mlibc_start_thread:
	ld.d $a0, $sp, 0
	ld.d $a1, $sp, 8
	ld.d $a2, $sp, 16
	addi.d $sp, $sp, 16
	bstrins.d $sp, $sp, 3, 0
	b __mlibc_enter_thread
	break 0

.section .note.GNU-stack,"",%progbits
