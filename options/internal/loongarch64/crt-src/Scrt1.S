#include <internal-config.h>

.extern __mlibc_entry@plt

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

	/* Align the stack to 16 bytes since elf doesn't guarantee that.
	andi zero extends the immediate on loongarch64 */
	li.d $t0, -16
	and $sp, $sp, $t0

	bl %plt(__mlibc_entry)
	break 0

.section .note.GNU-stack,"",%progbits
