
.section .text
.global _start
_start:
	.weak __global_pointer$
	.hidden __global_pointer$
	.option push
	.option norelax
	lla gp, __global_pointer$
	.option pop

	la a0, main
	call __mlibc_entry

load_gp:
.option push
.option norelax
	lla   gp, __global_pointer$
.option pop
	ret

.section .preinit_array,"aw"
.dc.a load_gp


.section .note.GNU-stack,"",%progbits
