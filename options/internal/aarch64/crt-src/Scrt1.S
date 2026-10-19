#include <internal-config.h>

.section .text
.global _start
_start:
#if MLIBC_ENTRY_INVERTED_ORDER
	mov x1, sp
	adr x0, main
#else
	mov x0, sp
	adr x1, main
#endif

	bl __mlibc_entry
	brk #0

.section .note.GNU-stack,"",%progbits

