#include <internal-config.h>

.extern __mlibc_entry

.section .text
.global _start
_start:
#if MLIBC_ENTRY_INVERTED_ORDER
	move $a1, $sp
	la $a0, main
#else
	move $a0, $sp
	la $a1, main
#endif
	b __mlibc_entry
	break 0

.section .note.GNU-stack,"",%progbits
