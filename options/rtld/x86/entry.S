#include "mlibc-asm/helpers.h"

PROC_START(_start)
.cfi_undefined eip
	call relocateSelf

	push %esp
	call interpreterMain

	jmp *%eax
PROC_END(_start)

GNU_STACK_NOTE()
