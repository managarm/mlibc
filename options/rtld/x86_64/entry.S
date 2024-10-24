#include "mlibc-asm/helpers.h"

PROC_START(_start)
.cfi_undefined rip
	call relocateSelf

	mov %rsp, %rdi
	call interpreterMain

	jmp *%rax
PROC_END(_start)

GNU_STACK_NOTE()

