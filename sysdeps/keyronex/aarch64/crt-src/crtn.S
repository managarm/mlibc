	.ident "aarch64-keyronex-mlibc crtn"

	.section .init
	ldp x29, x30, [sp], #16
	ret

	.section .fini
	ldp x29, x30, [sp], #16
	ret

	.section .note.GNU-stack,"",%progbits
