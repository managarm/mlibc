#include <internal-config.h>

.section .text
.global _start
_start:
#if MLIBC_ENTRY_INVERTED_ORDER
	mov x1, sp
	adrp x0, main
	add x0, x0, :lo12:main
#else
	mov x0, sp
	adrp x1, main
	add x1, x1, :lo12:main
#endif
	bl __mlibc_entry
.section .note.GNU-stack,"",%progbits

