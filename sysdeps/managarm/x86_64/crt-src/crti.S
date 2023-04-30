	.ident "x86_64-managarm-mlibc crti"

	.section .init
	.globl	_init
	.type	_init,@function
_init:

	.section .fini
	.globl	_fini
	.type	_fini,@function
_fini:

.section .note.GNU-stack,"",%progbits
