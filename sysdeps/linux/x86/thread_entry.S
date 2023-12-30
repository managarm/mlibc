#define FLAGS 4
#define STACK FLAGS+4
#define PTID STACK+4
#define CTID PTID+4
#define TLS CTID+4

.section .text
.global __mlibc_spawn_thread
.type __mlibc_spawn_thread, "function"
.cfi_startproc
__mlibc_spawn_thread:
	push %ebx
	.cfi_adjust_cfa_offset 4
	.cfi_rel_offset ebx, 0
	push %esi
	.cfi_adjust_cfa_offset 4
	.cfi_rel_offset esi, 0
	push %edi
	.cfi_adjust_cfa_offset 4
	.cfi_rel_offset edi, 0

	xor %eax, %eax
	mov 12+FLAGS(%esp), %ebx
	mov 12+STACK(%esp), %ecx
	mov 12+PTID(%esp), %edx
	/* On x86-32 tls and child_tid have to be reversed */
	mov 12+TLS(%esp), %esi
	mov 12+CTID(%esp), %edi
	mov $120, %al

	int $0x80

	test %eax, %eax
	jnz .parent_exit

	xor %ebp, %ebp
	.cfi_undefined %eip
	.cfi_undefined %ebp

	call 1f
1:
	pop %ebx
	addl $_GLOBAL_OFFSET_TABLE_+[.-1b], %ebx

	call __mlibc_enter_thread@plt
	hlt

.parent_exit:
	pop %edi
	.cfi_adjust_cfa_offset -4
	.cfi_restore edi
	pop %esi
	.cfi_adjust_cfa_offset -4
	.cfi_restore esi
	pop %ebx
	.cfi_adjust_cfa_offset -4
	.cfi_restore ebx
	ret
.cfi_endproc

.section .note.GNU-stack,"",%progbits
