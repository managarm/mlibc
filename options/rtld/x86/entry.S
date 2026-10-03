#include "mlibc-asm/helpers.h"

PROC_START(_start)
.cfi_undefined eip
	mov %esp, %esi

	/* The ABI requires the stack to be aligned to 16 bytes, but the older ABI didn't guarantee this on process entry */
	andl $-16, %esp

	call relocateSelf

	lea -12(%esp), %esp
	push %esi
	call interpreterMain
	
	mov %esi, %esp
	jmp *%eax
PROC_END(_start)

GNU_STACK_NOTE()
