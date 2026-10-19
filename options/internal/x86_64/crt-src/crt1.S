#include <internal-config.h>

.section .text
.global _start
_start:
#if MLIBC_ENTRY_INVERTED_ORDER
	// See generic/entry.cpp
	mov $main, %rdi
	mov %rsp, %rsi
#else
	mov %rsp, %rdi
	mov $main, %rsi
#endif
	call __mlibc_entry
.section .note.GNU-stack,"",%progbits
