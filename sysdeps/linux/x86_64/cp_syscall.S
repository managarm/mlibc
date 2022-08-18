
.section .text
.global __mlibc_do_asm_cp_syscall
.global __mlibc_syscall_begin
.global __mlibc_syscall_end
.type __mlibc_do_asm_cp_syscall, "function"
__mlibc_do_asm_cp_syscall:
	mov %rdi, %rax
	mov %rsi, %rdi
	mov %rdx, %rsi
	mov %rcx, %rdx
	mov %r8, %r10
	mov %r9, %r8
	mov 8(%rsp), %r9
	mov %fs:0x30, %r11
__mlibc_syscall_begin:
	/* tcbCancelEnableBit && tcbCancelTriggerBit */
	and $((1 << 0) | (1 << 2)), %r11
	cmp $((1 << 0) | (1 << 2)), %r11
	je cancel
	syscall
__mlibc_syscall_end:
	ret

cancel:
	call __mlibc_do_cancel
	hlt
.section .note.GNU-stack,"",%progbits

