#include <internal-config.h>

.section .text
.global _start
_start:
#if MLIBC_ENTRY_INVERTED_ORDER
	// See generic/entry.cpp
	lea main(%rip), %rdi
	mov %rsp, %rsi
#else
	mov %rsp, %rdi
	lea main(%rip), %rsi
#endif
	call __mlibc_entry
.section .note.GNU-stack,"",%progbits
