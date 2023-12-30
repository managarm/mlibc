.section .text

.type __stack_chk_fail_local, %function
.weak __stack_chk_fail_local
__stack_chk_fail_local:
	call __stack_chk_fail@plt

.global _start

.type _start, %function
.type main, %function
.type __mlibc_entry, %function

.cfi_startproc
_start:
.cfi_undefined eip
	xor %ebp, %ebp
	mov %esp, %edi
	call 1f

1:
	pop %ebx
	addl $_GLOBAL_OFFSET_TABLE_+[.-1b], %ebx
	push main@GOT(%ebx)
	push %edi
	call __mlibc_entry@plt
.cfi_endproc

.section .note.GNU-stack,"",%progbits
