.section .init
	popl %eax
	ret

.section .fini
	popl %eax
	ret

.section .note.GNU-stack,"",%progbits
