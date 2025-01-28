.ident "x86_64-managarm-mlibc crtn"

.section .init
	pop %rax
	ret

.section .fini
	pop %rax
	ret

.section .note.GNU-stack,"",%progbits
