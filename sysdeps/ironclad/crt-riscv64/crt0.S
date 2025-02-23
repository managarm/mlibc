.section .text
.global _start
_start:
	.weak __global_pointer$
	.hidden __global_pointer$
	.option push
	.option norelax
	lla gp, __global_pointer$
	.option pop

	mv a0, sp
	la a1, main
	call __mlibc_entry@plt
	unimp


.section .note.GNU-stack,"",%progbits

