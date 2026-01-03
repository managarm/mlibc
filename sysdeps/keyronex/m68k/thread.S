.section .text
.global __mlibc_thread_entry
__mlibc_thread_entry:
	jbsr __mlibc_thread_trampoline@PLTPC

.section .note.GNU-stack,"",%progbits
