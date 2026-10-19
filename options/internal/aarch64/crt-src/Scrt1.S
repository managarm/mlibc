#include <internal-config.h>

.section .text
.global _start
_start:
#if MLIBC_ENTRY_INVERTED_ORDER
	mov x1, sp
	adrp x0, :got:main
	ldr x0, [x0, :got_lo12:main]
#else
	mov x0, sp
	adrp x1, :got:main
	ldr x1, [x1, :got_lo12:main]
#endif

	bl __mlibc_entry
	brk #0

.section .note.GNU-stack,"",%progbits
