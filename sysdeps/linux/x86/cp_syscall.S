
.section .text
.global __mlibc_do_asm_cp_syscall
.global __mlibc_syscall_begin
.global __mlibc_syscall_end
.type __mlibc_do_asm_cp_syscall, "function"
__mlibc_do_asm_cp_syscall:
	push %ebx
	push %esi
	push %edi
	push %ebp
	; mov 16(%esp), %eax
	mov 24(%esp), %ebx
	mov 28(%esp), %ecx
	mov 32(%esp), %edx
	mov 36(%esp), %esi
	mov 40(%esp), %edi
	mov 44(%esp), %ebp
	mov %gs:0x18, %al
__mlibc_syscall_begin:
	/* tcbCancelEnableBit && tcbCancelTriggerBit */
	and $((1 << 0) | (1 << 2)), %al
	cmp $((1 << 0) | (1 << 2)), %al
	je cancel
	mov 20(%esp), %eax
	int $0x80
__mlibc_syscall_end:
	pop %ebp
	pop %edi
	pop %esi
	pop %ebx
	ret

cancel:
	pop %ebp
	pop %edi
	pop %esi
	pop %ebx
	call __mlibc_do_cancel@PLT
	hlt

.section .note.GNU-stack,"",%progbits
